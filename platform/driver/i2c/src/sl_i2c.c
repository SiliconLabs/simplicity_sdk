/***************************************************************************//**
 * @file
 * @brief Inter-Integrated Circuit (I2C) driver API
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <string.h>

#include "sl_assert.h"
#include "sl_udelay.h"
#include "sl_status.h"
#include "sl_interrupt_manager.h"
#include "sl_clock_manager.h"
#include "sl_sleeptimer.h"
#include "sl_hal_gpio.h"
#include "sl_hal_i2c.h"
#include "sl_i2c.h"
#include "sli_i2c.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/**
 * @brief I2C transfer timeout in milliseconds.
 *
 * @note
 *   Set to 30000 milliseconds to allow enough time for I2C operations to complete,
 *   especially for slower devices or larger data transfers.
 *   This value ensures the transfer doesn't hang indefinitely
 *   and allows the system to handle transfer failures.
 */
#define I2C_TRANSFER_TIMEOUT 30000

/**
 * @brief SCL hold time in microseconds.
 *
 * @note
 *   Set to 100 microseconds to ensure proper synchronization
 *   between leader and follower devices.
 *   This provides enough time for data processing on the SDA line
 *   and ensures reliable communication, even with slower devices.
 */
#ifndef I2C_SCL_HOLD_TIME_US
#define I2C_SCL_HOLD_TIME_US 100
#endif

/**
 * @brief Number of clock cycles for bus recovery.
 *
 * @note
 *   Set to 10 clock cycles to ensure the bus can be cleared effectively,
 *   allowing for proper recovery.
 *   This helps resume communication quickly without leaving the bus in a stalled state.
 */
#define I2C_RECOVER_NUM_CLOCKS  10

// Validation macro for I2C freq mode.
#define I2C_FREQ_MODE_IS_VALID(freq_mode)    ((freq_mode == SL_I2C_FREQ_STANDARD_MODE) \
                                              || (freq_mode == SL_I2C_FREQ_FAST_MODE)  \
                                              || (freq_mode == SL_I2C_FREQ_FASTPLUS_MODE))

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static sl_status_t i2c_get_peripheral_instance(I2C_TypeDef *i2c_base_addr, sl_peripheral_t* i2c_peripheral);
static uint32_t get_current_time_ms(void);
static sl_status_t i2c_leader_mode_blocking_state_machine(sli_i2c_instance_t *sl_i2c_instance, uint32_t timeout);
static sl_status_t i2c_follower_mode_blocking_state_machine(sli_i2c_instance_t *sl_i2c_instance, uint32_t timeout);
static void i2c_leader_mode_non_blocking_dispatch_interrupt(sli_i2c_instance_t *sl_i2c_instance);
static void i2c_follower_mode_non_blocking_dispatch_interrupt(sli_i2c_instance_t *sl_i2c_instance);
static void i2c_common_irq_handler(sli_i2c_instance_t *sl_i2c_instance);

/*******************************************************************************
 *****************************   GLOBAL VARIABLES   ****************************
 ******************************************************************************/
/// Array to hold each I2C device instance.
static sli_i2c_instance_t sl_i2c_device_instance[I2C_COUNT];

/// Temporary buffer for I2C operations.
static uint8_t temp_buffer[] = { 0xFF };

/***************************************************************************//**
 * This function initializes the I2C Module.
 ******************************************************************************/
sl_status_t sl_i2c_init(const sl_i2c_init_params_t *init_params,
                        sl_i2c_handle_t *i2c_handle)
{
  CORE_DECLARE_IRQ_STATE;
  sli_i2c_instance_t *sl_i2c_instance = NULL;
  int8_t i2c_instance_num;
  bool is_10bit_addr;
  uint8_t clk_pulses;
  sl_peripheral_t i2c_peripheral;
  sl_bus_clock_t i2c_bus_clk;

  // Parameter validation
  if (init_params == NULL || i2c_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Validate I2C base address
  if (!SL_HAL_I2C_REF_VALID(init_params->i2c_base_addr)) {
    return SL_STATUS_NOT_SUPPORTED;
  }
  // Validate operating mode
  if (!SL_I2C_OPERATING_MODE_IS_VALID(init_params->operating_mode)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();

  // Validate follower address (7-bit or 10-bit or invalid)
  if (init_params->follower_address <= 0x7F) {
    is_10bit_addr = false;
  } else if (init_params->follower_address <= 0x3FF) {
    is_10bit_addr = true;
  } else {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_RANGE;
  }

  // Retrieve I2C instance number
  i2c_instance_num = I2C_NUM(init_params->i2c_base_addr);

  // Check if the I2C instance is already initialized
  if (sl_i2c_device_instance[i2c_instance_num].i2c_base_addr != NULL) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALREADY_INITIALIZED;
  }

  // GPIO Configuration
  sli_i2c_configure_gpio(init_params->i2c_base_addr,
                         init_params->sda_gpio,
                         init_params->scl_gpio,
                         true);

  // Generate 9 clock pulses to recover the follower device from an unknown state.
  // Refer to the 'Bus Lock-Up' section in the I2C part of the reference manual.
  if (init_params->operating_mode == SL_I2C_LEADER_MODE) {
    for (clk_pulses = 0; clk_pulses < I2C_RECOVER_NUM_CLOCKS; clk_pulses++) {
      sl_hal_gpio_clear_pin(&(init_params->scl_gpio));
      sl_udelay_wait(I2C_SCL_HOLD_TIME_US);
      sl_hal_gpio_set_pin(&(init_params->scl_gpio));
      sl_udelay_wait(I2C_SCL_HOLD_TIME_US);
    }
  }

  // Get the I2C peripheral instance and Enable I2C bus clock
  i2c_get_peripheral_instance(init_params->i2c_base_addr, &i2c_peripheral);
  i2c_bus_clk = sl_device_peripheral_get_bus_clock(i2c_peripheral);
  sl_clock_manager_enable_bus_clock(i2c_bus_clk);

  // Configure I2C instance
  sli_i2c_instance_configuration(init_params);

  // Set follower address and address mode (7-bit/10-bit)
  if (init_params->operating_mode == SL_I2C_FOLLOWER_MODE) {
    sli_i2c_set_follower_address(init_params->i2c_base_addr,
                                 init_params->follower_address,
                                 is_10bit_addr);
  }

  // Configure DMA settings
  DMADRV_Init();

  // Populate I2C instance
  sl_i2c_instance = &sl_i2c_device_instance[i2c_instance_num];
  sl_i2c_instance->i2c_base_addr = init_params->i2c_base_addr;
  sl_i2c_instance->operating_mode = init_params->operating_mode;
  sl_i2c_instance->follower_address = (init_params->follower_address << 1);
  sl_i2c_instance->is_10bit_addr = is_10bit_addr;

  if ((DMADRV_AllocateChannel(&(sl_i2c_instance->dma_channel.dma_tx_channel), NULL) != ECODE_EMDRV_DMADRV_OK)
      || (DMADRV_AllocateChannel(&(sl_i2c_instance->dma_channel.dma_rx_channel), NULL) != ECODE_EMDRV_DMADRV_OK)) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Assign instance to the I2C handle
  *i2c_handle = (sl_i2c_handle_t)sl_i2c_instance;

  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * This function Deinitializes and resets the I2C Instance.
 ******************************************************************************/
sl_status_t sl_i2c_deinit(sl_i2c_handle_t i2c_handle)
{
  CORE_DECLARE_IRQ_STATE;
  sl_peripheral_t i2c_peripheral;
  sl_bus_clock_t i2c_bus_clk;

  // Validate handle
  if (i2c_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance and base address from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;
  I2C_TypeDef *i2c_base_addr = sl_i2c_instance->i2c_base_addr;

  // Get peripheral information
  i2c_get_peripheral_instance(i2c_base_addr, &i2c_peripheral);

  // Flush buffers and reset I2C peripheral
  sl_hal_i2c_flush_buffers(i2c_base_addr);
  sl_hal_i2c_reset(i2c_base_addr);
  sl_hal_i2c_set_follower_address(i2c_base_addr, 0x00, false);

  // Disable the clock for the I2C peripheral
  i2c_bus_clk = sl_device_peripheral_get_bus_clock(i2c_peripheral);
  sl_clock_manager_disable_bus_clock(i2c_bus_clk);

  // Deinitialize DMA channels and driver
  DMADRV_FreeChannel(sl_i2c_instance->dma_channel.dma_tx_channel);
  DMADRV_FreeChannel(sl_i2c_instance->dma_channel.dma_rx_channel);

  // Reset GPIO configuration
  sli_i2c_configure_gpio(i2c_base_addr,
                         sl_i2c_instance->sda_gpio,
                         sl_i2c_instance->scl_gpio,
                         false);

  // Clear instance-specific data
  memset(sl_i2c_instance, 0, sizeof(sli_i2c_instance_t));

  // Set the handle to NULL
  i2c_handle = NULL;

  CORE_EXIT_ATOMIC();
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * This function sets the frequency of I2C Bus.
 ******************************************************************************/
sl_status_t sl_i2c_set_frequency(sl_i2c_handle_t i2c_handle,
                                 sl_i2c_freq_mode_t freq_mode)
{
  CORE_DECLARE_IRQ_STATE;
  uint32_t freq = 0, max_freq = 0;
  sl_i2c_clock_hlr_t clhr = 0;
  sl_peripheral_t i2c_peripheral;
  sl_clock_branch_t i2c_clk_branch;

  // Validate handle
  if (i2c_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Validate frequency mode
  if (!I2C_FREQ_MODE_IS_VALID(freq_mode)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Get the I2C peripheral
  i2c_get_peripheral_instance(sl_i2c_instance->i2c_base_addr, &i2c_peripheral);

  // Retrieve the clock frequency for the I2C peripheral
  i2c_clk_branch = sl_device_peripheral_get_clock_branch(i2c_peripheral);
  sl_clock_manager_get_clock_branch_frequency(i2c_clk_branch, &freq);

  // Configure max frequency and clock settings based on frequency mode
  switch (freq_mode) {
    case SL_I2C_FREQ_STANDARD_MODE:
      max_freq = SL_HAL_I2C_FREQ_STANDARD_MAX;
      clhr = SL_I2C_CLK_HLR_STANDARD;
      break;

    case SL_I2C_FREQ_FAST_MODE:
      max_freq = SL_HAL_I2C_FREQ_FAST_MAX;
      clhr = SL_I2C_CLK_HLR_ASYMMETRIC;
      break;

    case SL_I2C_FREQ_FASTPLUS_MODE:
      max_freq = SL_HAL_I2C_FREQ_FASTPLUS_MAX;
      clhr = SL_I2C_CLK_HLR_FAST;
      break;
  }

  // Set the I2C bus clock frequency
  sl_hal_i2c_set_clock_frequency(sl_i2c_instance->i2c_base_addr, freq, max_freq, clhr);

  CORE_EXIT_ATOMIC();
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * This function gets the current frequency of I2C module.
 ******************************************************************************/
sl_status_t sl_i2c_get_frequency(sl_i2c_handle_t i2c_handle,
                                 uint32_t *frequency)
{
  CORE_DECLARE_IRQ_STATE;
  uint32_t clock_frequency;
  sl_peripheral_t i2c_peripheral;
  sl_clock_branch_t i2c_clk_branch;

  // Validate input pointers
  if (i2c_handle == NULL || frequency == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Get the I2C peripheral instance
  i2c_get_peripheral_instance(sl_i2c_instance->i2c_base_addr, &i2c_peripheral);

  // Retrieve the clock branch frequency for the I2C peripheral
  i2c_clk_branch = sl_device_peripheral_get_clock_branch(i2c_peripheral);
  sl_clock_manager_get_clock_branch_frequency(i2c_clk_branch, &clock_frequency);

  // Retrieve the current I2C bus frequency
  *frequency = sl_hal_i2c_get_clock_frequency(sl_i2c_instance->i2c_base_addr, clock_frequency);

  CORE_EXIT_ATOMIC();
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * This API is supported only in Leader mode and it is used to set the follower address.
 * It is used in Multi Follower mode (addressing a new follower which has similar
 * configurations set during Init API).
 ******************************************************************************/
sl_status_t sl_i2c_set_follower_address(sl_i2c_handle_t i2c_handle,
                                        uint16_t follower_address)
{
  CORE_DECLARE_IRQ_STATE;

  // Validate input pointer
  if (i2c_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Validate and determine the address type (7-bit or 10-bit)
  if (follower_address <= 0x7F) {
    sl_i2c_instance->is_10bit_addr = false;
  } else if (follower_address <= 0x3FF) {
    sl_i2c_instance->is_10bit_addr = true;
  } else {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Set the follower address, left-aligned for the I2C address frame
  sl_i2c_instance->follower_address = (follower_address << 1);

  CORE_EXIT_ATOMIC();
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * This is an optional API for configuring the DMA for the I2C instance.
 ******************************************************************************/
sl_status_t sl_i2c_configure_dma(sl_i2c_handle_t i2c_handle,
                                 sl_i2c_dma_channel_info_t dma_channel)
{
  CORE_DECLARE_IRQ_STATE;

  // Validate input pointers
  if (i2c_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Allocate new DMA channels for TX and RX operations and update I2C instance.
  if ((sl_i2c_instance->dma_channel.dma_tx_channel != dma_channel.dma_tx_channel)
      || (sl_i2c_instance->dma_channel.dma_rx_channel != dma_channel.dma_rx_channel)) {
    if (sl_i2c_instance->dma_channel.dma_tx_channel != dma_channel.dma_tx_channel) {
      if (DMADRV_AllocateChannel(&(dma_channel.dma_tx_channel), NULL) != ECODE_EMDRV_DMADRV_OK) {
        CORE_EXIT_ATOMIC();
        return SL_STATUS_ALLOCATION_FAILED;
      }
      DMADRV_FreeChannel(sl_i2c_instance->dma_channel.dma_tx_channel);
      sl_i2c_instance->dma_channel.dma_tx_channel = dma_channel.dma_tx_channel;
    }

    if (sl_i2c_instance->dma_channel.dma_rx_channel != dma_channel.dma_rx_channel) {
      if (DMADRV_AllocateChannel(&(dma_channel.dma_rx_channel), NULL) != ECODE_EMDRV_DMADRV_OK) {
        CORE_EXIT_ATOMIC();
        return SL_STATUS_ALLOCATION_FAILED;
      }
      DMADRV_FreeChannel(sl_i2c_instance->dma_channel.dma_rx_channel);
      sl_i2c_instance->dma_channel.dma_rx_channel = dma_channel.dma_rx_channel;
    }
  }

  CORE_EXIT_ATOMIC();
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Leader Mode : This function is used to send the data to the follower configured during Init API.
 * Follower Mode : This function is used to send the data to the addressed I2C Leader.
 * Returns on transfer complete or on error.
 ******************************************************************************/
sl_status_t sl_i2c_send_blocking(sl_i2c_handle_t i2c_handle,
                                 const uint8_t *tx_buffer,
                                 uint16_t tx_len,
                                 uint32_t timeout)
{
  CORE_DECLARE_IRQ_STATE;
  sl_status_t status = SL_STATUS_OK;

  // Validate input parameters
  if (i2c_handle == NULL || tx_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Validate the length of data to send
  if (tx_len == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Validate operating mode
  if (!SL_I2C_OPERATING_MODE_IS_VALID(sl_i2c_instance->operating_mode)) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize transaction parameters
  sl_i2c_instance->tx_buffer = tx_buffer;
  sl_i2c_instance->tx_len = tx_len;
  sl_i2c_instance->tx_offset = 0;
  sl_i2c_instance->transfer_seq = SL_I2C_WRITE;
  sl_i2c_instance->transfer_mode = SLI_I2C_BLOCKING_TRANSFER;

  // Initialize the I2C transfer
  sli_i2c_transfer_init(sl_i2c_instance->i2c_base_addr,
                        sl_i2c_instance->operating_mode,
                        SL_I2C_WRITE);

  if (sl_i2c_instance->operating_mode == SL_I2C_LEADER_MODE) {
    sl_i2c_instance->state = SLI_I2C_STATE_SEND_START_AND_ADDR;
    status = i2c_leader_mode_blocking_state_machine(sl_i2c_instance, timeout);
  } else {  // Follower mode
    status = i2c_follower_mode_blocking_state_machine(sl_i2c_instance, timeout);
  }

  CORE_EXIT_ATOMIC();
  return status;
}

/***************************************************************************//**
 * Leader Mode : This function is used to receive the data from the follower configured during Init API.
 * Follower Mode : This function is used to receive the data from the addressed I2C Leader.
 * Returns on transfer complete or on error.
 ******************************************************************************/
sl_status_t sl_i2c_receive_blocking(sl_i2c_handle_t i2c_handle,
                                    uint8_t *rx_buffer,
                                    uint16_t rx_len,
                                    uint32_t timeout)
{
  CORE_DECLARE_IRQ_STATE;
  sl_status_t status = SL_STATUS_OK;

  // Null pointer validation
  if (i2c_handle == NULL || rx_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Validate the length of data to receive
  if (rx_len == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Validate operating mode
  if (!SL_I2C_OPERATING_MODE_IS_VALID(sl_i2c_instance->operating_mode)) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize the I2C instance for receiving data
  sl_i2c_instance->rx_buffer = rx_buffer;
  sl_i2c_instance->rx_len = rx_len;
  sl_i2c_instance->rx_offset = 0;
  sl_i2c_instance->transfer_seq = SL_I2C_READ;
  sl_i2c_instance->transfer_mode = SLI_I2C_BLOCKING_TRANSFER;

  // Prepare I2C transfer configuration
  sli_i2c_transfer_init(sl_i2c_instance->i2c_base_addr,
                        sl_i2c_instance->operating_mode,
                        SL_I2C_READ);

  if (sl_i2c_instance->operating_mode == SL_I2C_LEADER_MODE) {
    sl_i2c_instance->state = SLI_I2C_STATE_SEND_START_AND_ADDR;
    status = i2c_leader_mode_blocking_state_machine(sl_i2c_instance, timeout);
  } else {  // Follower mode
    status = i2c_follower_mode_blocking_state_machine(sl_i2c_instance, timeout);
  }

  CORE_EXIT_ATOMIC();
  return status;
}

/***************************************************************************//**
 * This function is used to send and receive the data from the follower configured during Init API.
 * This API is supported only in Leader Mode and for blocking transfer.
 * Returns on transfer complete or on error.
 ******************************************************************************/
sl_status_t sl_i2c_transfer(sl_i2c_handle_t i2c_handle,
                            const uint8_t *tx_buffer,
                            uint16_t tx_len,
                            uint8_t *rx_buffer,
                            uint16_t rx_len)
{
  CORE_DECLARE_IRQ_STATE;
  sl_status_t status = SL_STATUS_OK;

  // Null pointer validation
  if (i2c_handle == NULL || tx_buffer == NULL || rx_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Validate the lengths of Tx and Rx buffers
  if (tx_len == 0 || rx_len == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Ensure the operating mode is in Leader mode
  if (sl_i2c_instance->operating_mode != SL_I2C_LEADER_MODE) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Set up I2C instance for the transfer operation
  sl_i2c_instance->tx_buffer = tx_buffer;
  sl_i2c_instance->tx_len = tx_len;
  sl_i2c_instance->tx_offset = 0;
  sl_i2c_instance->rx_buffer = rx_buffer;
  sl_i2c_instance->rx_len = rx_len;
  sl_i2c_instance->rx_offset = 0;
  sl_i2c_instance->transfer_seq = SL_I2C_WRITE_READ;
  sl_i2c_instance->transfer_mode = SLI_I2C_BLOCKING_TRANSFER;

  sli_i2c_transfer_init(sl_i2c_instance->i2c_base_addr,
                        sl_i2c_instance->operating_mode,
                        SL_I2C_WRITE_READ);

  sl_i2c_instance->state = SLI_I2C_STATE_SEND_START_AND_ADDR;
  status = i2c_leader_mode_blocking_state_machine(sl_i2c_instance, I2C_TRANSFER_TIMEOUT);

  CORE_EXIT_ATOMIC();
  return status;
}

/***************************************************************************//**
 * Leader Mode : This function uses DMA and Interrupt, to send the data to the follower set up during Init API.
 * Follower Mode : This function uses DMA and Interrupt, to send the data to the addressed I2C Leader.
 * User will be notified with the callback function.
 ******************************************************************************/
sl_status_t sl_i2c_send_non_blocking(sl_i2c_handle_t i2c_handle,
                                     const uint8_t *tx_buffer,
                                     uint16_t tx_len,
                                     sl_i2c_irq_callback_t i2c_callback,
                                     void *context)
{
  CORE_DECLARE_IRQ_STATE;

  // Null pointer validation
  if (i2c_handle == NULL || tx_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Tx length validation
  if (tx_len == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from the handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Validate operating mode
  if (!SL_I2C_OPERATING_MODE_IS_VALID(sl_i2c_instance->operating_mode)) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize the I2C instance structure
  sl_i2c_instance->tx_buffer = tx_buffer;
  sl_i2c_instance->tx_len = tx_len;
  sl_i2c_instance->tx_offset = 0;
  sl_i2c_instance->transfer_seq = SL_I2C_WRITE;
  sl_i2c_instance->transfer_mode = SLI_I2C_NON_BLOCKING_TRANSFER;
  sl_i2c_instance->callback = i2c_callback;
  sl_i2c_instance->transfer_event = SL_I2C_EVENT_IDLE;
  sl_i2c_instance->context = context;
  memset(sl_i2c_instance->addr_buffer, 0, sizeof(sl_i2c_instance->addr_buffer));

  // Set the state based on the operating mode (Leader or Follower)
  if (sl_i2c_instance->operating_mode == SL_I2C_LEADER_MODE) {
    sl_i2c_instance->state = SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK;
  } else {  // Follower mode
    sl_i2c_instance->state = SLI_I2C_STATE_ADDRESS_MATCH;
  }

  sli_i2c_dma_transfer_init(sl_i2c_instance);

  CORE_EXIT_ATOMIC();
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Leader Mode : This function uses DMA and Interrupt, to receive the data from the follower set up during Init API.
 * Follower Mode : This function uses DMA and Interrupt, to receive the data from the addressed I2C Leader.
 * User will be notified with the callback function.
 ******************************************************************************/
sl_status_t sl_i2c_receive_non_blocking(sl_i2c_handle_t i2c_handle,
                                        uint8_t *rx_buffer,
                                        uint16_t rx_len,
                                        sl_i2c_irq_callback_t i2c_callback,
                                        void *context)
{
  CORE_DECLARE_IRQ_STATE;

  // Null pointer validation
  if (i2c_handle == NULL || rx_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Rx length validation
  if (rx_len == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_ENTER_ATOMIC();

  // Get the I2C instance from handle
  sli_i2c_instance_t *sl_i2c_instance = (sli_i2c_instance_t *)i2c_handle;

  // Validate operating mode
  if (!SL_I2C_OPERATING_MODE_IS_VALID(sl_i2c_instance->operating_mode)) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Fill the internal instance structure for the non-blocking receive
  sl_i2c_instance->rx_buffer = rx_buffer;
  sl_i2c_instance->rx_len = rx_len;
  sl_i2c_instance->rx_offset = 0;
  sl_i2c_instance->transfer_seq = SL_I2C_READ;
  sl_i2c_instance->transfer_mode = SLI_I2C_NON_BLOCKING_TRANSFER;
  sl_i2c_instance->callback = i2c_callback;
  sl_i2c_instance->context = context;
  sl_i2c_instance->transfer_event = SL_I2C_EVENT_IDLE;
  memset(sl_i2c_instance->addr_buffer, 0, sizeof(sl_i2c_instance->addr_buffer));

  // Set the state based on the operating mode (Leader or Follower)
  if (sl_i2c_instance->operating_mode == SL_I2C_LEADER_MODE) {
    sl_i2c_instance->state = SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK;
  } else {  // Follower mode
    sl_i2c_instance->state = SLI_I2C_STATE_ADDRESS_MATCH;
  }

  sli_i2c_dma_transfer_init(sl_i2c_instance);

  CORE_EXIT_ATOMIC();
  return SL_STATUS_OK;
}

/*******************************************************************************
 **************************   INTERNAL FUNCTIONS   *****************************
 ******************************************************************************/

/***************************************************************************//**
 * This function configures the I2C module.
 ******************************************************************************/
sl_status_t sli_i2c_instance_configuration(const sl_i2c_init_params_t *init_params)
{
  uint32_t ref_freq, max_freq = 0;
  sl_peripheral_t i2c_peripheral;
  sl_i2c_clock_hlr_t clhr = 0;
  sl_clock_branch_t i2c_clk_branch;

  // Null pointer validation
  if (init_params == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Get I2C peripheral based on the provided base address
  i2c_get_peripheral_instance(init_params->i2c_base_addr, &i2c_peripheral);

  // Retrieve the I2C peripheral clock frequency
  i2c_clk_branch = sl_device_peripheral_get_clock_branch(i2c_peripheral);
  sl_clock_manager_get_clock_branch_frequency(i2c_clk_branch, &ref_freq);

  // Clear and disable all I2C interrupts to ensure clean state
  sl_hal_i2c_clear_interrupts(init_params->i2c_base_addr, _I2C_IF_MASK);
  sl_hal_i2c_disable_interrupts(init_params->i2c_base_addr, _I2C_IEN_MASK);

  // Initialize the I2C module with the appropriate operating mode (leader or follower)
  sl_hal_i2c_init(init_params->i2c_base_addr, init_params->operating_mode);

  // FIFO configuration, if present, using the specified thresholds for TX and RX
#if defined(SL_CATALOG_I2C_FIFO_PRESENT)
  if (init_params->fifo_threshold != NULL) {
    sl_hal_i2c_set_tx_fifo_threshold(init_params->i2c_base_addr, init_params->fifo_threshold->tx_threshold_val);
    sl_hal_i2c_set_rx_fifo_threshold(init_params->i2c_base_addr, init_params->fifo_threshold->rx_threshold_val);
  }
#endif

  // Configure the I2C frequency based on the operating mode and the frequency mode
  if (init_params->operating_mode == SL_I2C_LEADER_MODE) {
    switch (init_params->freq_mode) {
      case SL_I2C_FREQ_STANDARD_MODE:
        max_freq = SL_HAL_I2C_FREQ_STANDARD_MAX;
        clhr = SL_I2C_CLK_HLR_STANDARD;
        break;
      case SL_I2C_FREQ_FAST_MODE:
        max_freq = SL_HAL_I2C_FREQ_FAST_MAX;
        clhr = SL_I2C_CLK_HLR_ASYMMETRIC;
        break;
      case SL_I2C_FREQ_FASTPLUS_MODE:
        max_freq = SL_HAL_I2C_FREQ_FASTPLUS_MAX;
        clhr = SL_I2C_CLK_HLR_FAST;
        break;
    }

    // Set the I2C bus frequency based on the reference clock and the max frequency
    sl_hal_i2c_set_clock_frequency(init_params->i2c_base_addr, ref_freq, max_freq, clhr);
  }

  // Enable the I2C peripheral
  sl_hal_i2c_enable(init_params->i2c_base_addr);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * This function is used for the setting the follower address for follower.
 ******************************************************************************/
void sli_i2c_set_follower_address(I2C_TypeDef *i2c_base_addr,
                                  uint16_t follower_address,
                                  bool is_10bit_addr)
{
  sl_hal_i2c_set_follower_address(i2c_base_addr, (follower_address << 1), is_10bit_addr);
  sl_hal_i2c_set_follower_mask_address(i2c_base_addr, 0xFE); // 0xFE means exact address match
}

/***************************************************************************//**
 * This function configures the GPIO module.
 ******************************************************************************/
void sli_i2c_configure_gpio(I2C_TypeDef *i2c_base_addr,
                            sl_gpio_t sda_gpio,
                            sl_gpio_t scl_gpio,
                            bool enable)
{
  // Retrieve I2C instance number based on the base address
  int8_t i2c_instance_num = I2C_NUM(i2c_base_addr);

  if (enable) {
    sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

    // Configure the SDA and SCL pins as wired AND with pull-up enabled
    // Output value must be set to 1 to avoid driving the lines low
    sl_hal_gpio_set_pin_mode(&scl_gpio, SL_GPIO_MODE_WIRED_AND_PULLUP, 1);
    sl_hal_gpio_set_pin_mode(&sda_gpio, SL_GPIO_MODE_WIRED_AND_PULLUP, 1);

    // Enable I2C routing for SDA and SCL pins
    GPIO->I2CROUTE[i2c_instance_num].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
    GPIO->I2CROUTE[i2c_instance_num].SCLROUTE = (uint32_t)((scl_gpio.pin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                                           | (scl_gpio.port << _GPIO_I2C_SCLROUTE_PORT_SHIFT));
    GPIO->I2CROUTE[i2c_instance_num].SDAROUTE = (uint32_t)((sda_gpio.pin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                                           | (sda_gpio.port << _GPIO_I2C_SDAROUTE_PORT_SHIFT));
  } else {
    GPIO->I2CROUTE[i2c_instance_num].ROUTEEN = _GPIO_I2C_ROUTEEN_RESETVALUE;
    GPIO->I2CROUTE[i2c_instance_num].SCLROUTE = _GPIO_I2C_SCLROUTE_RESETVALUE;
    GPIO->I2CROUTE[i2c_instance_num].SDAROUTE = _GPIO_I2C_SDAROUTE_RESETVALUE;

    sl_hal_gpio_set_pin_mode(&scl_gpio, SL_GPIO_MODE_DISABLED, 0);
    sl_hal_gpio_set_pin_mode(&sda_gpio, SL_GPIO_MODE_DISABLED, 0);

    sl_clock_manager_disable_bus_clock(SL_BUS_CLOCK_GPIO);
  }
}

/***************************************************************************//**
 * This function is used for the configuring I2C instance.
 ******************************************************************************/
void sli_i2c_transfer_init(I2C_TypeDef *i2c_base_addr,
                           sl_i2c_operating_mode_t operating_mode,
                           sl_i2c_transfer_seq_t transfer_seq)
{
  sl_hal_i2c_flush_buffers(i2c_base_addr);
  sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);
  sl_hal_i2c_enable_interrupts(i2c_base_addr, I2C_IEN_ACK | I2C_IEN_NACK | SL_HAL_I2C_IEN_ERRORS);

  switch (operating_mode) {
    case SL_I2C_LEADER_MODE:
      sl_hal_i2c_enable_interrupts(i2c_base_addr, I2C_IEN_MSTOP);
      if (transfer_seq != SL_I2C_WRITE) {
        sl_hal_i2c_enable_interrupts(i2c_base_addr, I2C_IEN_RXDATAV);
      }
      break;

    case SL_I2C_FOLLOWER_MODE:
      sl_hal_i2c_enable_interrupts(i2c_base_addr, I2C_IEN_SSTOP | I2C_IEN_RXDATAV | I2C_IEN_ADDR);
      break;
  }
}

/***************************************************************************//**
 * This function is used for the initialization of I2C DMA transfer.
 ******************************************************************************/
sl_status_t sli_i2c_dma_transfer_init(sli_i2c_instance_t *i2c_instance)
{
  // Null pointer validation
  if (i2c_instance == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  I2C_TypeDef *i2c_base_addr = i2c_instance->i2c_base_addr;
  bool is_10bit_addr = i2c_instance->is_10bit_addr;
  uint16_t follower_address = (i2c_instance->follower_address >> 1);
  uint8_t *addr_buffer = i2c_instance->addr_buffer, addr_buffer_count = 0;
  sl_i2c_operating_mode_t operating_mode = i2c_instance->operating_mode;
  sl_i2c_dma_channel_info_t *dma_channel = &(i2c_instance->dma_channel);
  DMADRV_PeripheralSignal_t dma_tx_trigger_source, dma_rx_trigger_source;

  const uint8_t *data_buffer = NULL;
  uint16_t data_len = 0;
  if (i2c_instance->transfer_seq == SL_I2C_WRITE) {
    data_buffer = i2c_instance->tx_buffer;
    data_len = i2c_instance->tx_len;
  } else {  // SL_I2C_READ
    data_buffer = i2c_instance->rx_buffer;
    data_len = i2c_instance->rx_len;
  }

  sl_hal_i2c_flush_buffers(i2c_base_addr);

  switch (I2C_NUM(i2c_base_addr)) {
#if defined(LDMAXBAR_CH_REQSEL_SIGSEL_I2C0TXBL) && defined(LDMAXBAR_CH_REQSEL_SIGSEL_I2C0RXDATAV) \
    || defined(LDMAXBAR0_CH_REQSEL_SIGSEL_I2C0TXBL) && defined(LDMAXBAR0_CH_REQSEL_SIGSEL_I2C0RXDATAV)
    case 0:
      dma_tx_trigger_source = dmadrvPeripheralSignal_I2C0_TXBL;
      dma_rx_trigger_source = dmadrvPeripheralSignal_I2C0_RXDATAV;
      sl_interrupt_manager_clear_irq_pending(I2C0_IRQn);
      sl_interrupt_manager_enable_irq(I2C0_IRQn);
      break;
#endif

#if defined(LDMAXBAR_CH_REQSEL_SIGSEL_I2C1TXBL) && defined(LDMAXBAR_CH_REQSEL_SIGSEL_I2C1RXDATAV) \
    || defined(LDMAXBAR0_CH_REQSEL_SIGSEL_I2C1TXBL) && defined(LDMAXBAR0_CH_REQSEL_SIGSEL_I2C1RXDATAV)
    case 1:
      dma_tx_trigger_source = dmadrvPeripheralSignal_I2C1_TXBL;
      dma_rx_trigger_source = dmadrvPeripheralSignal_I2C1_RXDATAV;
      sl_interrupt_manager_clear_irq_pending(I2C1_IRQn);
      sl_interrupt_manager_enable_irq(I2C1_IRQn);
      break;
#endif

#if defined(LDMAXBAR0_CH_REQSEL_SIGSEL_I2C2TXBL) && defined(LDMAXBAR0_CH_REQSEL_SIGSEL_I2C2RXDATAV)
    case 2:
      dma_tx_trigger_source = dmadrvPeripheralSignal_I2C2_TXBL;
      dma_rx_trigger_source = dmadrvPeripheralSignal_I2C2_RXDATAV;
      sl_interrupt_manager_clear_irq_pending(I2C2_IRQn);
      sl_interrupt_manager_enable_irq(I2C2_IRQn);
      break;
#endif

    default:
      return SL_STATUS_INVALID_CONFIGURATION;
  }

#if defined(EMDRV_DMADRV_LDMA)
  LDMA_TransferCfg_t tx_cfg = LDMA_TRANSFER_CFG_PERIPHERAL(dma_tx_trigger_source);
  LDMA_TransferCfg_t rx_cfg = LDMA_TRANSFER_CFG_PERIPHERAL(dma_rx_trigger_source);

  sl_interrupt_manager_disable_irq(LDMA_IRQn);
  LDMA_IntDisable(1 << dma_channel->dma_tx_channel);
  LDMA_IntDisable(1 << dma_channel->dma_rx_channel);
#elif defined(EMDRV_DMADRV_LDMA_S3)
  sl_hal_ldma_transfer_config_t tx_cfg = SL_HAL_LDMA_TRANSFER_CFG_PERIPHERAL(dma_tx_trigger_source);
  sl_hal_ldma_transfer_config_t rx_cfg = SL_HAL_LDMA_TRANSFER_CFG_PERIPHERAL(dma_rx_trigger_source);

  sl_hal_ldma_disable_interrupts(LDMA0, _LDMA_IEN_MASK);
#endif

  if (operating_mode == SL_I2C_LEADER_MODE) {
    sl_hal_i2c_enable_interrupts(i2c_base_addr, (I2C_IEN_ACK | I2C_IEN_NACK | I2C_IEN_MSTOP | SL_HAL_I2C_IEN_ERRORS));

    if (is_10bit_addr) {
      addr_buffer[0] = ((((follower_address << 1) >> 8) & 0x06) | (SL_I2C_FIRST_BYTE_10BIT_ADDR_MASK));
      addr_buffer[1] = ((follower_address << 1) & 0xFF);
      if (i2c_instance->transfer_seq == SL_I2C_READ) {
        addr_buffer[2] = addr_buffer[0] | 1;
      }
      addr_buffer_count = 2;
    } else {  // 7-bit address
      addr_buffer[0] = ((follower_address << 1) & SL_I2C_7BIT_FOLLOWER_ADDRESS_MASK);
      if (i2c_instance->transfer_seq == SL_I2C_READ) {
        addr_buffer[0] |= 1;
      }
      addr_buffer_count = 1;
    }

#if defined(EMDRV_DMADRV_LDMA)
    if (i2c_instance->transfer_seq == SL_I2C_WRITE) {
      (i2c_base_addr)->CTRL_SET = I2C_CTRL_AUTOSE;
      i2c_instance->tx_desc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE((void*)(addr_buffer), &((i2c_base_addr)->TXDATA), addr_buffer_count, 1);
      i2c_instance->tx_desc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE((void*)data_buffer, &((i2c_base_addr)->TXDATA), data_len);
    } else if (i2c_instance->transfer_seq == SL_I2C_READ) {
      (i2c_base_addr)->CTRL_SET = I2C_CTRL_AUTOACK;
      i2c_instance->tx_desc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE((void*)(addr_buffer), &((i2c_base_addr)->TXDATA), addr_buffer_count);
      // For 10 bit and 7 bit, receive operations are similar
      if (data_len == 1) {
        i2c_instance->rx_desc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&((i2c_base_addr)->RXDATA), (void*)data_buffer, 1, 1);
        i2c_instance->rx_desc[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_WRITE((I2C_CMD_NACK | I2C_CMD_STOP), (uint32_t)(&(i2c_base_addr)->CMD));
      } else {
        i2c_instance->rx_desc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&((i2c_base_addr)->RXDATA), (void*)data_buffer, data_len - 1, 1);
        i2c_instance->rx_desc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE((uint32_t)(&(i2c_base_addr)->RXDATA), (uint32_t)(data_buffer + data_len - 1), 1, 1);
        i2c_instance->rx_desc[3] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_WRITE((I2C_CMD_NACK | I2C_CMD_STOP), (uint32_t)(&(i2c_base_addr)->CMD));
      }
    }
#elif defined(EMDRV_DMADRV_LDMA_S3)
    if (i2c_instance->transfer_seq == SL_I2C_WRITE) {
      (i2c_base_addr)->CTRL_SET = I2C_CTRL_AUTOSE;
      i2c_instance->tx_desc[0] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_M2P(SL_HAL_LDMA_CTRL_SIZE_BYTE, (void*)(addr_buffer), &(i2c_base_addr)->TXDATA, addr_buffer_count, 1);
      i2c_instance->tx_desc[1] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_SINGLE_M2P(SL_HAL_LDMA_CTRL_SIZE_BYTE, (void*)data_buffer, &((i2c_base_addr)->TXDATA), data_len);
    } else if (i2c_instance->transfer_seq == SL_I2C_READ) {
      (i2c_base_addr)->CTRL_SET = I2C_CTRL_AUTOACK;
      i2c_instance->tx_desc[0] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_SINGLE_M2P(SL_HAL_LDMA_CTRL_SIZE_BYTE, (void*)(addr_buffer), &((i2c_base_addr)->TXDATA), addr_buffer_count);
      // For 10 bit and 7 bit, receive operations are similar
      if (data_len == 1) {
        i2c_instance->rx_desc[0] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[1] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, &((i2c_base_addr)->RXDATA), (void*)data_buffer, 1, 1);
        i2c_instance->rx_desc[2] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_SINGLE_WRITE((I2C_CMD_NACK | I2C_CMD_STOP), (uint32_t)(&(i2c_base_addr)->CMD));
      } else {
        i2c_instance->rx_desc[0] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, &((i2c_base_addr)->RXDATA), (void*)data_buffer, data_len - 1, 1);
        i2c_instance->rx_desc[1] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[2] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, (uint32_t)(&(i2c_base_addr)->RXDATA), (uint32_t)(data_buffer + data_len - 1), 1, 1);
        i2c_instance->rx_desc[3] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_SINGLE_WRITE((I2C_CMD_NACK | I2C_CMD_STOP), (uint32_t)(&(i2c_base_addr)->CMD));
      }
    }
#endif
    DMADRV_LdmaStartTransfer(dma_channel->dma_tx_channel,
                             &tx_cfg,
                             (void*)&(i2c_instance->tx_desc),
                             NULL, NULL);
    if (i2c_instance->transfer_seq == SL_I2C_READ) {
      DMADRV_LdmaStartTransfer(dma_channel->dma_rx_channel,
                               &rx_cfg,
                               (void*)&(i2c_instance->rx_desc),
                               NULL, NULL);
    }

    sl_hal_i2c_start_cmd(i2c_base_addr);
  } else if (operating_mode == SL_I2C_FOLLOWER_MODE) {
    sl_hal_i2c_enable_interrupts(i2c_base_addr, (I2C_IEN_ADDR | SL_HAL_I2C_IEN_ERRORS));
    (i2c_base_addr)->CTRL_SET = I2C_CTRL_AUTOACK;
#if defined(EMDRV_DMADRV_LDMA)
    if (i2c_instance->transfer_seq == SL_I2C_READ) {
      addr_buffer_count = is_10bit_addr ? 2 : 1;
      i2c_instance->rx_desc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&((i2c_base_addr)->RXDATA), (void*)(addr_buffer), addr_buffer_count, 1);
      if (data_len == 1) {
        i2c_instance->rx_desc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&((i2c_base_addr)->RXDATA), (void*)data_buffer, 1, 1);
        i2c_instance->rx_desc[3] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_WRITE(I2C_CMD_NACK, (uint32_t)(&(i2c_base_addr)->CMD));
      } else {
        i2c_instance->rx_desc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&((i2c_base_addr)->RXDATA), (void*)data_buffer, data_len - 1, 1);
        i2c_instance->rx_desc[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[3] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE((uint32_t)(&(i2c_base_addr)->RXDATA), (uint32_t)(data_buffer + data_len - 1), 1, 1);
        i2c_instance->rx_desc[4] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_WRITE(I2C_CMD_NACK, (uint32_t)(&(i2c_base_addr)->CMD));
      }
    } else if (i2c_instance->transfer_seq == SL_I2C_WRITE) {
      addr_buffer_count = is_10bit_addr ? 3 : 1;
      i2c_instance->rx_desc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&((i2c_base_addr)->RXDATA), (void*)(addr_buffer), 3, 1);
      i2c_instance->rx_desc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR));
      i2c_instance->tx_desc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE((void*)data_buffer, &((i2c_base_addr)->TXDATA), data_len, 1);
      i2c_instance->tx_desc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE((void*)temp_buffer, &((i2c_base_addr)->TXDATA), 1, 0);
    }
#elif defined(EMDRV_DMADRV_LDMA_S3)
    if (i2c_instance->transfer_seq == SL_I2C_READ) {
      addr_buffer_count = is_10bit_addr ? 2 : 1;
      i2c_instance->rx_desc[0] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, &((i2c_base_addr)->RXDATA), (void*)(addr_buffer), addr_buffer_count, 1);
      if (data_len == 1) {
        i2c_instance->rx_desc[1] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[2] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, &((i2c_base_addr)->RXDATA), (void*)data_buffer, 1, 1);
        i2c_instance->rx_desc[3] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_SINGLE_WRITE(I2C_CMD_NACK, (uint32_t)(&(i2c_base_addr)->CMD));
      } else {
        i2c_instance->rx_desc[1] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, &((i2c_base_addr)->RXDATA), (void*)data_buffer, data_len - 1, 1);
        i2c_instance->rx_desc[2] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR), 1);
        i2c_instance->rx_desc[3] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, (uint32_t)(&(i2c_base_addr)->RXDATA), (uint32_t)(data_buffer + data_len - 1), 1, 1);
        i2c_instance->rx_desc[4] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_SINGLE_WRITE(I2C_CMD_NACK, (uint32_t)(&(i2c_base_addr)->CMD));
      }
    } else if (i2c_instance->transfer_seq == SL_I2C_WRITE) {
      addr_buffer_count = is_10bit_addr ? 3 : 1;
      i2c_instance->rx_desc[0] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_P2M(SL_HAL_LDMA_CTRL_SIZE_BYTE, &((i2c_base_addr)->RXDATA), (void*)(addr_buffer), addr_buffer_count, 1);
      i2c_instance->rx_desc[1] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_SINGLE_WRITE(I2C_CTRL_AUTOACK, (uint32_t)(&(i2c_base_addr)->CTRL_CLR));
      i2c_instance->tx_desc[0] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_M2P(SL_HAL_LDMA_CTRL_SIZE_BYTE, (void*)data_buffer, &((i2c_base_addr)->TXDATA), data_len, 1);
      i2c_instance->tx_desc[1] = (sl_hal_ldma_descriptor_t)SL_HAL_LDMA_DESCRIPTOR_LINKREL_M2P(SL_HAL_LDMA_CTRL_SIZE_BYTE, (void*)temp_buffer, &((i2c_base_addr)->TXDATA), 1, 0);
    }
#endif

    DMADRV_LdmaStartTransfer(dma_channel->dma_rx_channel,
                             &rx_cfg,
                             (void*)&(i2c_instance->rx_desc),
                             NULL, NULL);
    if (i2c_instance->transfer_seq == SL_I2C_WRITE) {
      DMADRV_LdmaStartTransfer(dma_channel->dma_tx_channel,
                               &tx_cfg,
                               (void*)&(i2c_instance->tx_desc),
                               NULL, NULL);
    }
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * This function is used for getting the respective i2c peripheral instance
 *
 * @param[in] i2c_base_addr     i2c base address instance
 * @param[out] i2c_peripheral   i2c peripheral instance
 *
 * @return return status.
 ******************************************************************************/
static sl_status_t i2c_get_peripheral_instance(I2C_TypeDef *i2c_base_addr,
                                               sl_peripheral_t* i2c_peripheral)
{
  // Null pointer validation.
  if (i2c_peripheral == NULL || i2c_base_addr == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  switch (I2C_NUM(i2c_base_addr)) {
    case 0: *i2c_peripheral = SL_PERIPHERAL_I2C0;
      break;
    case 1: *i2c_peripheral = SL_PERIPHERAL_I2C1;
      break;
    case 2: *i2c_peripheral = SL_PERIPHERAL_I2C2;
      break;
    case 3: *i2c_peripheral = SL_PERIPHERAL_I2C3;
      break;
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * This function is used to get the current time in ms.
 ******************************************************************************/
static uint32_t get_current_time_ms(void)
{
  return sl_sleeptimer_tick_to_ms(sl_sleeptimer_get_tick_count());
}

/***************************************************************************//**
 * State machine for leader mode in blocking I2C transfer.
 * This function manages the state transitions for an I2C leader
 * operating in blocking transfer mode.
 *
 * @param[in] sl_i2c_instance   Pointer to the I2C instance.
 * @param[in] timeout           Timeout duration for the blocking transfer.
 *
 * @note Error Handling:
 *       - Arbitration Fault: This occurs if a follower device
 *         does not respond as expected or if another leader is
 *         present on the bus.
 *       - Bus Error: A misplaced START or STOP condition may indicate
 *         an unexpected bus state. In leader mode, this should not happen
 *         under normal conditions if controlled by this software.
 *
 * @return returns status.
 ******************************************************************************/
static sl_status_t i2c_leader_mode_blocking_state_machine(sli_i2c_instance_t *sl_i2c_instance,
                                                          uint32_t timeout)
{
  sl_status_t status = SL_STATUS_OK;
  bool exit = false;
  uint32_t pending_irq, current_time;
  uint8_t follower_address;
  I2C_TypeDef *i2c_base_addr = sl_i2c_instance->i2c_base_addr;
  uint32_t start_time = get_current_time_ms();

  while ((!exit)) {
    // check for timeout.
    current_time = get_current_time_ms();
    if ((current_time - start_time) > timeout) {
      (i2c_base_addr)->CMD = I2C_CMD_ABORT;
      sl_i2c_instance->state = SLI_I2C_STATE_TIMEOUT;
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_TIMEOUT;
      status = SL_STATUS_TIMEOUT;
      exit = true;
      break;
    }

    pending_irq = sl_hal_i2c_get_enabled_pending_interrupts(i2c_base_addr);

    // Handle errors
    if (pending_irq & SL_HAL_I2C_IF_ERRORS) {
      if (pending_irq & I2C_IF_ARBLOST) {
        sl_i2c_instance->transfer_event = SL_I2C_EVENT_ARBITRATION_LOST;
      } else if (pending_irq & I2C_IF_BUSERR) {
        sl_i2c_instance->transfer_event = SL_I2C_EVENT_BUS_ERROR;
      }
      sl_i2c_instance->state = SLI_I2C_STATE_ERROR;
      sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);
      (i2c_base_addr)->CMD = I2C_CMD_ABORT;
      status = SL_STATUS_ABORT;
      break;
    }

    switch (sl_i2c_instance->state) {
      /********************************************************/
      /* Handles the following states:                        */
      /* - SLI_I2C_STATE_SEND_START_AND_ADDR:                 */
      /*   Sends the first start condition with the address   */
      /*   (includes the first byte for 10-bit addresses).    */
      /* - SLI_I2C_STATE_SEND_REPEATED_START_AND_ADDR:        */
      /*   Sends a repeated start condition with the address. */
      /********************************************************/
      case SLI_I2C_STATE_SEND_START_AND_ADDR:
      case SLI_I2C_STATE_SEND_REPEATED_START_AND_ADDR:
        if (sl_i2c_instance->is_10bit_addr) {
          follower_address = (((sl_i2c_instance->follower_address >> 8) & 0x06) | (SL_I2C_FIRST_BYTE_10BIT_ADDR_MASK));
        } else {  // 7-bit address
          follower_address = (sl_i2c_instance->follower_address & SL_I2C_7BIT_FOLLOWER_ADDRESS_MASK);
          if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
            follower_address |= 1;
          }
        }

        if (sl_i2c_instance->state == SLI_I2C_STATE_SEND_START_AND_ADDR) {
          sl_hal_i2c_tx(i2c_base_addr, follower_address);
          sl_hal_i2c_start_cmd(i2c_base_addr);
          sl_i2c_instance->state = SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK;
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
        } else if (sl_i2c_instance->state == SLI_I2C_STATE_SEND_REPEATED_START_AND_ADDR) {
          if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE_READ) {
            follower_address |= 1;
          }
          sl_hal_i2c_start_cmd(i2c_base_addr);
          sl_hal_i2c_tx(i2c_base_addr, follower_address);
          sl_i2c_instance->state = SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK;
        }
        break;

      /****************************************************************************/
      /* Handles ACK/NACK responses for different address transmission scenarios. */
      /*                                                                          */
      /* This case manages:                                                       */
      /* 1. SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK                               */
      /*    - Waits for an ACK/NACK after sending the first address byte.         */
      /*    - If ACK is received:                                                 */
      /*        - For 10-bit mode, sends the second address byte and waits for    */
      /*          another ACK/NACK.                                               */
      /*        - For 7-bit mode, determines if it's a read or write operation    */
      /*          and transitions accordingly.                                    */
      /*                                                                          */
      /* 2. SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK                */
      /*    - Waits for ACK/NACK after sending the second byte in 10-bit mode.    */
      /*    - If ACK is received:                                                 */
      /*        - If a read operation, issues a repeated start.                   */
      /*        - Otherwise, transitions to data transmission.                    */
      /*                                                                          */
      /* 3. SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK                      */
      /*    - Waits for ACK/NACK after a repeated START + address.                */
      /*    - If ACK is received:                                                 */
      /*        - In 10-bit mode, transitions to receiving data.                  */
      /*        - In 7-bit write-read sequences, transitions to receiving data.   */
      /*    - If NACK is received at any stage, the transfer is aborted, and a    */
      /*      STOP condition is issued.                                           */
      /****************************************************************************/
      case SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK:
      case SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK:
      case SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK:
        if (pending_irq & I2C_IF_NACK) {
          /* Handle NACK response */
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_NACK);
          sl_hal_i2c_stop_cmd(i2c_base_addr);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_NACK_RECEIVED;
          sl_i2c_instance->state = SLI_I2C_STATE_SEND_STOP;
          status = SL_STATUS_ABORT;
        } else if (pending_irq & I2C_IF_ACK) {
          /* Handle ACK response */
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_ACK);

          if (sl_i2c_instance->state == SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK) {
            if (sl_i2c_instance->is_10bit_addr) {
              sl_hal_i2c_tx(i2c_base_addr, (sl_i2c_instance->follower_address & 0xFF));
              sl_i2c_instance->state = SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK;
            } else {  // 7-Bit address
              if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
                sl_i2c_instance->state = SLI_I2C_STATE_RECEIVE_DATA;
              } else {  // WRITE OR WRITE-READ
                sl_i2c_instance->state = SLI_I2C_STATE_SEND_DATA;
              }
            }
          } else if (sl_i2c_instance->state == SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK) {
            if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
              sl_hal_i2c_start_cmd(i2c_base_addr);
              uint8_t follower_address = (((sl_i2c_instance->follower_address >> 8) & 0x06) | SL_I2C_FIRST_BYTE_10BIT_ADDR_MASK) | 1;
              sl_hal_i2c_tx(i2c_base_addr, follower_address);
              sl_i2c_instance->state = SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK;
            } else { // WRITE or WRITE_READ
              sl_i2c_instance->state = SLI_I2C_STATE_SEND_DATA;
            }
          } else if (sl_i2c_instance->state == SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK) {
            if ((sl_i2c_instance->is_10bit_addr) && (sl_i2c_instance->transfer_seq != SL_I2C_WRITE)) { // READ or WRITE_READ
              sl_i2c_instance->state = SLI_I2C_STATE_RECEIVE_DATA;
            } else if ((!sl_i2c_instance->is_10bit_addr) && (sl_i2c_instance->transfer_seq == SL_I2C_WRITE_READ)) {
              sl_i2c_instance->state = SLI_I2C_STATE_RECEIVE_DATA;
            }
          }
        }
        break;

      /************************************/
      /* Send a data byte to the follower */
      /************************************/
      case SLI_I2C_STATE_SEND_DATA:
        if (sl_i2c_instance->tx_offset < sl_i2c_instance->tx_len) {
          sl_hal_i2c_tx(i2c_base_addr, sl_i2c_instance->tx_buffer[sl_i2c_instance->tx_offset]);
          sl_i2c_instance->tx_offset++;
          sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_ACK_OR_NACK;
        } else if (sl_i2c_instance->tx_offset == sl_i2c_instance->tx_len) {
          if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE_READ) {
            sl_i2c_instance->state = SLI_I2C_STATE_SEND_REPEATED_START_AND_ADDR;
          } else {
            sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
            sl_i2c_instance->state = SLI_I2C_STATE_SEND_STOP;
            sl_hal_i2c_stop_cmd(i2c_base_addr);
          }
        }

        break;

      /*****************************************************************/
      /* Wait for ACK/NACK from the follower after sending data to it. */
      /*****************************************************************/
      case SLI_I2C_STATE_WAIT_FOR_ACK_OR_NACK:
        if (pending_irq & I2C_IF_NACK) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_NACK);
          if ((sl_i2c_instance->transfer_seq == SL_I2C_WRITE_READ) && (sl_i2c_instance->tx_offset == sl_i2c_instance->tx_len)) {
            sl_i2c_instance->state = SLI_I2C_STATE_SEND_REPEATED_START_AND_ADDR;
          } else {
            sl_hal_i2c_stop_cmd(i2c_base_addr);
            sl_i2c_instance->transfer_event = SL_I2C_EVENT_NACK_RECEIVED;
            sl_i2c_instance->state = SLI_I2C_STATE_SEND_STOP;
          }
        } else if (pending_irq & I2C_IF_ACK) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_ACK);
          sl_i2c_instance->state = SLI_I2C_STATE_SEND_DATA;
        }

        break;

      /*****************************************/
      /* Receive a data byte from the follower */
      /*****************************************/
      case SLI_I2C_STATE_RECEIVE_DATA:
        if (pending_irq & I2C_IF_RXDATAV) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_RXDATAV);

          if (sl_i2c_instance->rx_offset < sl_i2c_instance->rx_len) {
            sl_i2c_instance->rx_buffer[sl_i2c_instance->rx_offset] = sl_hal_i2c_rx(i2c_base_addr);
            sl_i2c_instance->rx_offset++;

            if (sl_i2c_instance->rx_offset == sl_i2c_instance->rx_len) {
              sl_hal_i2c_send_nack(i2c_base_addr);
              sl_hal_i2c_stop_cmd(i2c_base_addr);
              sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
              sl_i2c_instance->state = SLI_I2C_STATE_SEND_STOP;
            } else {
              sl_hal_i2c_send_ack(i2c_base_addr);
            }
          }
        }
        break;

      /***********************************/
      /* Wait for STOP to have been sent */
      /***********************************/
      case SLI_I2C_STATE_SEND_STOP:
        if (pending_irq & I2C_IF_MSTOP) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_MSTOP);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
          sl_i2c_instance->state = SLI_I2C_STATE_DONE;
          exit = true;
        }

        break;

      default:
        sl_i2c_instance->transfer_event = SL_I2C_EVENT_SW_FAULT;
        sl_i2c_instance->state = SLI_I2C_STATE_ERROR;
        status = SL_STATUS_ABORT;
        exit = true;
        break;
    }
  }

  return status;
}

/***************************************************************************//**
 * State machine for leader mode in blocking I2C transfer.
 * This function manages the state transitions for an I2C leader operating
 * in blocking transfer mode.
 *
 * @param[in] sl_i2c_instance   Pointer to the I2C instance.
 * @param[in] timeout           Timeout duration for the blocking transfer.
 *
 * @note Error Handling:
 *       - Arbitration Fault: Occurs when a follower device does not
 *         respond as expected or another leader is present on the bus.
 *       - Bus Error: A misplaced START or STOP condition may indicate
 *         an unexpected bus state. In leader mode, this should not happen
 *         under normal conditions if controlled by this software.
 *
 * @return returns status.
 ******************************************************************************/
static sl_status_t i2c_follower_mode_blocking_state_machine(sli_i2c_instance_t *sl_i2c_instance,
                                                            uint32_t timeout)
{
  sl_status_t status = SL_STATUS_OK;
  bool exit = false, address_match = false;
  uint32_t pending_irq, current_time;
  uint16_t rx_data;
  I2C_TypeDef *i2c_base_addr = sl_i2c_instance->i2c_base_addr;
  uint32_t start_time = get_current_time_ms();

  while (!address_match) {
    // check for timeout.
    current_time = get_current_time_ms();
    if ((current_time - start_time) > timeout) {
      (i2c_base_addr)->CMD = I2C_CMD_ABORT;
      sl_i2c_instance->state = SLI_I2C_STATE_TIMEOUT;
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_TIMEOUT;
      status = SL_STATUS_TIMEOUT;
      exit = true;
      break;
    }

    pending_irq = sl_hal_i2c_get_enabled_pending_interrupts(i2c_base_addr);

    // Handle errors
    if (pending_irq & SL_HAL_I2C_IF_ERRORS) {
      if (pending_irq & I2C_IF_ARBLOST) {
        sl_i2c_instance->transfer_event = SL_I2C_EVENT_ARBITRATION_LOST;
      } else if (pending_irq & I2C_IF_BUSERR) {
        sl_i2c_instance->transfer_event = SL_I2C_EVENT_BUS_ERROR;
      }
      sl_i2c_instance->state = SLI_I2C_STATE_ERROR;
      sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);
      (i2c_base_addr)->CMD = I2C_CMD_ABORT;
      status = SL_STATUS_ABORT;
      exit = true;
      break;
    }

    // Match the address (first byte if 10 bit)
    if (pending_irq & I2C_IF_ADDR) {
      sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);
      rx_data = sl_hal_i2c_rx(i2c_base_addr);
      sl_hal_i2c_send_ack(i2c_base_addr);

      if (sl_i2c_instance->is_10bit_addr) {
        sl_i2c_instance->state = SLI_I2C_STATE_10BIT_ADDRESS_MATCH;
      } else {
        if (rx_data & 0x01) {
          sl_i2c_instance->state = SLI_I2C_STATE_SEND_DATA;
        } else {
          sl_i2c_instance->state = SLI_I2C_STATE_RECEIVE_DATA;
        }
      }
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
      address_match = true;
    }
  }

  while ((!exit)) {
    // check for timeout.
    current_time = get_current_time_ms();
    if ((current_time - start_time) > timeout) {
      (i2c_base_addr)->CMD = I2C_CMD_ABORT;
      sl_i2c_instance->state = SLI_I2C_STATE_TIMEOUT;
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_TIMEOUT;
      status = SL_STATUS_TIMEOUT;
      exit = true;
      break;
    }

    pending_irq = sl_hal_i2c_get_enabled_pending_interrupts(i2c_base_addr);

    switch (sl_i2c_instance->state) {
      /***************************************************/
      /* Match the second byte of the 10 address       . */
      /***************************************************/
      case SLI_I2C_STATE_10BIT_ADDRESS_MATCH:
        if (pending_irq & I2C_IF_RXDATAV) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_RXDATAV);
          rx_data = sl_hal_i2c_rx(i2c_base_addr);

          if ((sl_i2c_instance->follower_address & 0xFF) == rx_data) {
            sl_hal_i2c_send_ack(i2c_base_addr);
            if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE) {
              sl_i2c_instance->state = SLI_I2C_STATE_REP_ADDR_MATCH;
            } else {
              sl_i2c_instance->state = SLI_I2C_STATE_RECEIVE_DATA;
            }
          } else {
            sl_hal_i2c_send_nack(i2c_base_addr);
            sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
            sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_STOP;
          }
        }
        break;

      /***********************************************************************/
      /* Wait for match the repeated start + address (first byte if 10 bit). */
      /***********************************************************************/
      case SLI_I2C_STATE_REP_ADDR_MATCH:
        if (pending_irq & I2C_IF_ADDR) {
          sl_hal_i2c_send_ack(i2c_base_addr);
          sl_hal_i2c_clear_interrupts(i2c_base_addr, (I2C_IF_ADDR | I2C_IF_RXDATAV));
          rx_data = sl_hal_i2c_rx(i2c_base_addr);

          if (sl_i2c_instance->is_10bit_addr) {
            if (rx_data & 0x01) {
              sl_i2c_instance->state = SLI_I2C_STATE_SEND_DATA;
            } else {
              sl_hal_i2c_send_nack(i2c_base_addr);
              sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_STOP;
              status = SL_STATUS_ABORT;
            }
          }
        }
        break;

      /***************************************/
      /* Receive a data byte from the leader */
      /***************************************/
      case SLI_I2C_STATE_RECEIVE_DATA:
        if (pending_irq & I2C_IF_RXDATAV) {
          rx_data = sl_hal_i2c_rx(i2c_base_addr);
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_RXDATAV);

          if (sl_i2c_instance->rx_offset < sl_i2c_instance->rx_len) {
            sl_i2c_instance->rx_buffer[sl_i2c_instance->rx_offset] = rx_data;
            sl_i2c_instance->rx_offset++;

            if (sl_i2c_instance->rx_offset == sl_i2c_instance->rx_len) {
              sl_hal_i2c_send_nack(i2c_base_addr);
              sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
              sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_STOP;
            } else {
              sl_hal_i2c_send_ack(i2c_base_addr);
            }
          }
        } else if (pending_irq & I2C_IF_SSTOP) {
          sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_STOP;
        }
        break;

      /**********************************/
      /* Send a data byte to the leader */
      /**********************************/
      case SLI_I2C_STATE_SEND_DATA:
        if (sl_i2c_instance->tx_offset < sl_i2c_instance->tx_len) {
          sl_hal_i2c_tx(i2c_base_addr, sl_i2c_instance->tx_buffer[sl_i2c_instance->tx_offset]);
          sl_i2c_instance->tx_offset++;
          sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_ACK_OR_NACK;
        } else if (sl_i2c_instance->tx_offset == sl_i2c_instance->tx_len) {
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
          sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_STOP;
          sl_i2c_instance->tx_offset++;
        }
        break;

      /***************************************************************/
      /* Wait for ACK/NACK from the leader after sending data to it. */
      /***************************************************************/
      case SLI_I2C_STATE_WAIT_FOR_ACK_OR_NACK:
        if (pending_irq & I2C_IF_NACK) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_NACK);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_NACK_RECEIVED;
          sl_i2c_instance->state = SLI_I2C_STATE_WAIT_FOR_STOP;
        } else if (pending_irq & I2C_IF_ACK) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_ACK);
          sl_i2c_instance->state = SLI_I2C_STATE_SEND_DATA;
        }
        break;

      /********************************************/
      /* Wait for STOP to receive from the leader */
      /********************************************/
      case SLI_I2C_STATE_WAIT_FOR_STOP:
        if (pending_irq & I2C_IF_SSTOP) {
          sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_SSTOP);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
          sl_i2c_instance->state = SLI_I2C_STATE_DONE;
          exit = true;
        } else if (sl_i2c_instance->tx_offset >= sl_i2c_instance->tx_len) {
          sl_hal_i2c_tx(i2c_base_addr, 0xFF);
        }
        break;

      default:
        sl_i2c_instance->transfer_event = SL_I2C_EVENT_SW_FAULT;
        sl_i2c_instance->state = SLI_I2C_STATE_ERROR;
        exit = true;
        break;
    }
  }

  return status;
}

/***************************************************************************//**
 * Function called by the IRQHandlers for handling the interrupts for non blocking mode leader
 *
 * @details This function is called when I2C interrupts are handled by the IRQHandlers.
 *          Function gets pending interrupt flags and performs the necessary actions
 *          according to the pending interrupt flags.
 *
 * @param sl_i2c_instance Pointer to the I2C instance structure.
 ******************************************************************************/
static void i2c_leader_mode_non_blocking_dispatch_interrupt(sli_i2c_instance_t *sl_i2c_instance)
{
  I2C_TypeDef *i2c_base_addr = sl_i2c_instance->i2c_base_addr;
  uint32_t pending_irq = sl_hal_i2c_get_enabled_pending_interrupts(i2c_base_addr);

  if (pending_irq & I2C_IF_MSTOP) {
    sl_hal_i2c_disable_interrupts(i2c_base_addr, _I2C_IEN_MASK);
    sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);

    if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE) {
      DMADRV_StopTransfer(sl_i2c_instance->dma_channel.dma_tx_channel);
    } else if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
      DMADRV_StopTransfer(sl_i2c_instance->dma_channel.dma_rx_channel);
    }

    (i2c_base_addr)->CTRL = _I2C_CTRL_RESETVALUE;

    if (sl_i2c_instance->transfer_event == SL_I2C_EVENT_IN_PROGRESS) {
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
    }
    if (sl_i2c_instance->callback) {
      sl_i2c_instance->callback(sl_i2c_instance->transfer_event, sl_i2c_instance->context);
    }
  } else if (pending_irq & I2C_IF_NACK) {
    sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_NACK);
    switch (sl_i2c_instance->state) {
      case SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK:
      case SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK:
      case SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK:
        sl_i2c_instance->transfer_event = SL_I2C_EVENT_INVALID_ADDR;
        break;
    }
    sl_hal_i2c_stop_cmd(i2c_base_addr);
  } else if (pending_irq & I2C_IF_ACK) {
    sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_ACK);

    switch (sl_i2c_instance->state) {
      case SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK:
        if (sl_i2c_instance->is_10bit_addr) {
          sl_i2c_instance->state = SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK;
        } else {
          sl_hal_i2c_disable_interrupts(i2c_base_addr, I2C_IEN_ACK);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
        }
        break;

      case SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK:
        if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE) {
          sl_hal_i2c_disable_interrupts(i2c_base_addr, I2C_IEN_ACK);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
        } else if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
          sl_hal_i2c_start_cmd(i2c_base_addr);
          sl_hal_i2c_tx(i2c_base_addr, sl_i2c_instance->addr_buffer[2]);
          sl_i2c_instance->state = SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK;
        }
        break;

      case SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK:
        if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
          sl_hal_i2c_disable_interrupts(i2c_base_addr, I2C_IEN_ACK);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
        }
        break;
    }
  } else {
    if (pending_irq & I2C_IF_ARBLOST) {
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_ARBITRATION_LOST;
    } else if (pending_irq & I2C_IF_BUSERR) {
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_BUS_ERROR;
    }
    sl_i2c_instance->state = SLI_I2C_STATE_ERROR;
    sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);
    // Abort on error
    (i2c_base_addr)->CMD = I2C_CMD_ABORT;
  }
}

/***************************************************************************//**
 * Function called by the IRQHandlers for handling the interrupts for non blocking mode follower
 *
 * @details This function is called when I2C interrupts are handled by the IRQHandlers.
 *          Function gets pending interrupt flags and performs the necessary actions
 *          according to the pending interrupt flags.
 *
 * @param sl_i2c_instance Pointer to the I2C instance structure.
 ******************************************************************************/
static void i2c_follower_mode_non_blocking_dispatch_interrupt(sli_i2c_instance_t *sl_i2c_instance)
{
  I2C_TypeDef *i2c_base_addr = sl_i2c_instance->i2c_base_addr;
  uint32_t pending_irq = sl_hal_i2c_get_enabled_pending_interrupts(i2c_base_addr);

  if (pending_irq & I2C_IF_SSTOP) {
    sl_hal_i2c_disable_interrupts(i2c_base_addr, _I2C_IEN_MASK);
    sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);

    if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE) {
      DMADRV_StopTransfer(sl_i2c_instance->dma_channel.dma_tx_channel);
    } else if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
      DMADRV_StopTransfer(sl_i2c_instance->dma_channel.dma_rx_channel);
    }

    (i2c_base_addr)->CTRL = _I2C_CTRL_RESETVALUE;

    if (sl_i2c_instance->transfer_event == SL_I2C_EVENT_IN_PROGRESS) {
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_COMPLETED;
    }
    if (sl_i2c_instance->callback) {
      sl_i2c_instance->callback(sl_i2c_instance->transfer_event, sl_i2c_instance->context);
    }
  } else if (pending_irq & I2C_IF_ADDR) {
    (i2c_base_addr)->CTRL_SET = I2C_CTRL_AUTOACK;
    sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);
    sl_hal_i2c_enable_interrupts(i2c_base_addr, (I2C_IEN_NACK | I2C_IEN_RXDATAV | I2C_IEN_SSTOP));

    switch (sl_i2c_instance->state) {
      case SLI_I2C_STATE_ADDRESS_MATCH:
        if (sl_i2c_instance->is_10bit_addr) {
          sl_i2c_instance->state = SLI_I2C_STATE_10BIT_ADDRESS_MATCH;
        } else {
          sl_hal_i2c_disable_interrupts(i2c_base_addr, (I2C_IEN_ADDR | I2C_IEN_RXDATAV));
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
        }
        break;

      case SLI_I2C_STATE_REP_ADDR_MATCH:
        if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE) {
          sl_hal_i2c_disable_interrupts(i2c_base_addr, I2C_IEN_ADDR);
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
        }
        break;
    }
  } else if (pending_irq & I2C_IF_RXDATAV) {
    sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_RXDATAV);
    sl_hal_i2c_disable_interrupts(i2c_base_addr, I2C_IEN_RXDATAV);
    if (sl_i2c_instance->state == SLI_I2C_STATE_10BIT_ADDRESS_MATCH) {
      if ((sl_i2c_instance->follower_address & 0XFF) == sl_i2c_instance->addr_buffer[1]) {
        if (sl_i2c_instance->transfer_seq == SL_I2C_READ) {
          sl_i2c_instance->transfer_event = SL_I2C_EVENT_IN_PROGRESS;
        } else if (sl_i2c_instance->transfer_seq == SL_I2C_WRITE) {
          sl_i2c_instance->state = SLI_I2C_STATE_REP_ADDR_MATCH;
        }
      } else {
        sl_hal_i2c_send_nack(i2c_base_addr);
      }
    }
  } else if (pending_irq & I2C_IF_NACK) {
    sl_hal_i2c_clear_interrupts(i2c_base_addr, I2C_IF_NACK);
  } else {
    if (pending_irq & I2C_IF_ARBLOST) {
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_ARBITRATION_LOST;
    } else if (pending_irq & I2C_IF_BUSERR) {
      sl_i2c_instance->transfer_event = SL_I2C_EVENT_BUS_ERROR;
    }
    sl_i2c_instance->state = SLI_I2C_STATE_ERROR;
    sl_hal_i2c_clear_interrupts(i2c_base_addr, _I2C_IF_MASK);
    // Abort on error
    (i2c_base_addr)->CMD = I2C_CMD_ABORT;
  }
}

/***************************************************************************//**
 * Function called by IRQ handlers to process I2C interrupts.
 *
 * @details This function handles interrupts for I2C. It dispatches
 *          the interrupt handling based on the operating mode (leader or follower).
 *
 * @param[in] sl_i2c_instance Pointer to the I2C instance structure.
 ******************************************************************************/
static void i2c_common_irq_handler(sli_i2c_instance_t *sl_i2c_instance)
{
  if (sl_i2c_instance->transfer_mode == SLI_I2C_NON_BLOCKING_TRANSFER) {
    if (sl_i2c_instance->operating_mode == SL_I2C_LEADER_MODE) {
      i2c_leader_mode_non_blocking_dispatch_interrupt(sl_i2c_instance);
    } else {  // Follower mode
      i2c_follower_mode_non_blocking_dispatch_interrupt(sl_i2c_instance);
    }
  }
}

#if defined(I2C0)
void I2C0_IRQHandler(void)
{
  sli_i2c_instance_t *sl_i2c_instance = &sl_i2c_device_instance[0];
  i2c_common_irq_handler(sl_i2c_instance);
}
#endif  // I2C0

#if defined(I2C1)
void I2C1_IRQHandler(void)
{
  sli_i2c_instance_t *sl_i2c_instance = &sl_i2c_device_instance[1];
  i2c_common_irq_handler(sl_i2c_instance);
}
#endif  // I2C1

#if defined(I2C2)
void I2C2_IRQHandler(void)
{
  sli_i2c_instance_t *sl_i2c_instance = &sl_i2c_device_instance[2];
  i2c_common_irq_handler(sl_i2c_instance);
}
#endif  // I2C2
