/***************************************************************************//**
 * @file
 * @brief I2C simple poll-based master mode driver
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>
#include "sl_assert.h"
#include "sl_i2cspm.h"
#include "sl_udelay.h"
#include "sl_clock_manager.h"

#if defined(_SILICON_LABS_32B_SERIES_3)
#include "sl_device_peripheral.h"
#include "sl_hal_i2c.h"
#endif //_SILICON_LABS_32B_SERIES_3

#include "sl_gpio.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Transfer timeout (how many polls).
#define SL_I2CSPM_TRANSFER_TIMEOUT 300000

// SCL hold time in microseconds.
#ifndef SL_I2CSPM_SCL_HOLD_TIME_US
#define SL_I2CSPM_SCL_HOLD_TIME_US 100
#endif

// Clock cycles for bus recovery.
#define SL_I2CSPM_RECOVER_NUM_CLOCKS  10

#if defined(_SILICON_LABS_32B_SERIES_3)

// I2C Write and Read direction.
#define SL_I2CSPM_WRITE_DIRECTION      0x00
#define SL_I2CSPM_READ_DIRECTION       0x01

// Leader Mode.
#define SL_I2CSPM_LEADER_MODE    (sl_i2c_operating_mode_t)(0)

// I2C Errors.
#define SL_I2CSPM_IF_ERRORS    (I2C_IF_BUSERR | I2C_IF_ARBLOST)
#define SL_I2CSPM_IEN_ERRORS   (I2C_IEN_BUSERR | I2C_IEN_ARBLOST)

// First byte of 10 bit addressing mode.
#define SL_I2CSPM_FIRST_BYTE_10BIT_ADDR_MASK   (0xF0)

// Invalid 10bit follower address.
#define SL_I2CSPM_INVALID_10BIT_FOLLOWER_ADDRESS  (0x3FF)

// Invalid 7bit follower address.
#define SL_I2CSPM_INVALID_7BIT_FOLLOWER_ADDRESS   (0xFF)

// 7bit follower address mask.
#define SL_I2CSPM_7BIT_FOLLOWER_ADDRESS_MASK   (0xFE)

#endif //_SILICON_LABS_32B_SERIES_3

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_3)
/// I2C transfer states.
typedef enum {
  i2cStateInProgress = 0,
  i2cState7bitAdd,                             ///< I2C 7 Bit address transfer.
  i2cState10bitAdd,                            ///< I2C 10 Bit address transfer.
  i2cState10bitAddWithRepStart,                ///< I2C 10 Bit address transfer with Repeated Start.
  i2cStateDataReceive,                         ///< I2C Receive data after tranferrign
                                               ///< 10 Bit address transfer with Repeated Start .
  i2cStateAck,                                 ///< I2C ACK.
  i2cStateNack,                                ///< I2C NACK.
  i2cStateError,                               ///< I2C Error.
  i2cStateDone,                                ///< I2C data transfer complete.
}I2C_StateFlag_TypeDef;
#endif //_SILICON_LABS_32B_SERIES_3

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_3)
/// I2C Device handle.
typedef struct {
  I2C_TypeDef  *port;                  ///< Peripheral port.

  uint16_t followerAddress;            ///< Follower Address.

  uint8_t *txBuffer;                   ///< Tx Data buffer.

  uint8_t *rxBuffer;                   ///< Rx Data buffer.

  uint32_t txLen;                      ///< Tx Length.

  uint32_t rxLen;                      ///< Rx Length.

  uint16_t  offset;                    ///< Offset in the current sequence buffer.

  uint16_t flags;                      ///< Flags defining sequence type.

  I2C_StateFlag_TypeDef state;         ///< State of the transaction.

  I2C_TransferReturn_TypeDef result;   ///< Result return code.
} I2C_Device_Handle_TypeDef;
#endif //_SILICON_LABS_32B_SERIES_3

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_3)
static I2C_TransferReturn_TypeDef send(I2C_Device_Handle_TypeDef *i2cDevice);
static I2C_TransferReturn_TypeDef receive(I2C_Device_Handle_TypeDef *i2cDevice);
static I2C_TransferReturn_TypeDef transfer(I2C_Device_Handle_TypeDef *i2cDevice);
static void transmit_handler(I2C_Device_Handle_TypeDef *i2cDevice);
static void receive_handler(I2C_Device_Handle_TypeDef *i2cDevice);
static void send_follower_address(bool is10BitAddr,
                                  uint8_t rwDirection,
                                  I2C_Device_Handle_TypeDef *i2cDevice);
static I2C_Device_Handle_TypeDef* get_i2c_device(int8_t instance);
static void i2cspm_state_machine(I2C_Device_Handle_TypeDef *i2cDevice);
#endif //_SILICON_LABS_32B_SERIES_3

/*******************************************************************************
 *****************************   GLOBAL VARIABLES   ****************************
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_3)
static I2C_Device_Handle_TypeDef i2cDeviceHandle[I2C_COUNT];
#endif //_SILICON_LABS_32B_SERIES_3

/*******************************************************************************
 *   Initalize I2C peripheral
 ******************************************************************************/
void I2CSPM_Init(I2CSPM_Init_TypeDef *init)
{
  int clkPulses;
  int8_t i2cInstance = 0;
  sl_bus_clock_t i2cBusClock;
#if defined(_SILICON_LABS_32B_SERIES_3)
  sl_peripheral_t peripheral;
  sl_clock_branch_t clockBranch;
  I2C_Device_Handle_TypeDef *i2cDevice = NULL;
#else
  I2C_Init_TypeDef i2cInit;
#endif //_SILICON_LABS_32B_SERIES_3

  EFM_ASSERT(init != NULL);

  // Get the instance from port number.
  i2cInstance = I2C_NUM(init->port);

#if defined(_SILICON_LABS_32B_SERIES_3)
  // Select I2C peripheral clock.
  if (init->port == I2C0) {
    i2cBusClock = SL_BUS_CLOCK_I2C0;
    peripheral = SL_PERIPHERAL_I2C0;
#if defined(I2C1)
  } else if (init->port == I2C1) {
    i2cBusClock = SL_BUS_CLOCK_I2C1;
    peripheral = SL_PERIPHERAL_I2C1;
#endif
#if defined(I2C2)
  } else if (init->port == I2C2) {
    i2cBusClock = SL_BUS_CLOCK_I2C2;
    peripheral = SL_PERIPHERAL_I2C2;
#endif
  } else {
    EFM_ASSERT(false);
    return;
  }
#else
  // Select I2C peripheral clock.
  if (init->port == I2C0) {
    i2cBusClock = SL_BUS_CLOCK_I2C0;
#if defined(I2C1)
  } else if (init->port == I2C1) {
    i2cBusClock = SL_BUS_CLOCK_I2C1;
#endif
#if defined(I2C2)
  } else if (init->port == I2C2) {
    i2cBusClock = SL_BUS_CLOCK_I2C2;
#endif
  } else {
    EFM_ASSERT(false);
    return;
  }
#endif //_SILICON_LABS_32B_SERIES_3

  // Enabling I2C bus clock.
  sl_clock_manager_enable_bus_clock(i2cBusClock);

#if defined(_SILICON_LABS_32B_SERIES_3)
  // Get the I2C Clk.
  if (init->i2cRefFreq == 0) {
    clockBranch = sl_device_peripheral_get_clock_branch(peripheral);
    sl_clock_manager_get_clock_branch_frequency(clockBranch, &init->i2cRefFreq);
  }
  // Get the I2C device handle of an instance.
  i2cDevice = get_i2c_device(i2cInstance);

  // Update I2C device handle of an instance.
  i2cDevice->port = init->port;
#else
  if (i2cInstance < 0 || i2cInstance >= I2C_COUNT) {
    // Assert on invalid instance number.
    EFM_ASSERT(false);
  }
#endif //_SILICON_LABS_32B_SERIES_3

  // Output value must be set to 1 to not drive lines low.
  // Set SCL first, to ensure it is high before changing SDA.
  sl_gpio_t scl_gpio = {
    .port = init->sclPort,
    .pin = init->sclPin
  };
  sl_gpio_t sda_gpio = {
    .port = init->sdaPort,
    .pin = init->sdaPin
  };
  sl_gpio_set_pin_mode(&scl_gpio, SL_GPIO_MODE_WIRED_AND_PULLUP, 1);
  sl_gpio_set_pin_mode(&sda_gpio, SL_GPIO_MODE_WIRED_AND_PULLUP, 1);

  // In some situations, after a reset during an I2C transfer, the slave device may be
  // left in an unknown state. Send 9 clock pulses to set slave in a defined state.
  for (clkPulses = 0; clkPulses < SL_I2CSPM_RECOVER_NUM_CLOCKS; clkPulses++) {
    sl_gpio_clear_pin(&scl_gpio);
    sl_udelay_wait(SL_I2CSPM_SCL_HOLD_TIME_US);
    sl_gpio_set_pin(&scl_gpio);
    sl_udelay_wait(SL_I2CSPM_SCL_HOLD_TIME_US);
  }

  // Enable pins and set location.
  GPIO->I2CROUTE[i2cInstance].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;

  GPIO->I2CROUTE[i2cInstance].SCLROUTE = (uint32_t)((init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                                    | (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT));

  GPIO->I2CROUTE[i2cInstance].SDAROUTE = (uint32_t)((init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                                    | (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT));

#if defined(_SILICON_LABS_32B_SERIES_3)
  // Initialize I2C module in Leader Mode.
  sl_hal_i2c_init(init->port, SL_I2CSPM_LEADER_MODE);

  // Set the I2C Bus Frequency.
  sl_hal_i2c_set_clock_frequency(init->port, init->i2cRefFreq, init->i2cMaxFreq, init->i2cClhr);

  // Enable i2c.
  sl_hal_i2c_enable(init->port);
#else
  /* Set emlib init parameters */
  i2cInit.enable = true;
  i2cInit.master = true; /* master mode only */
  i2cInit.freq = init->i2cMaxFreq;
  i2cInit.refFreq = init->i2cRefFreq;
  i2cInit.clhr = init->i2cClhr;

  I2C_Init(init->port, &i2cInit);
#endif //_SILICON_LABS_32B_SERIES_3
}

/*******************************************************************************
 *   Perform I2C transfer
 ******************************************************************************/
I2C_TransferReturn_TypeDef I2CSPM_Transfer(I2C_TypeDef *i2c, I2C_TransferSeq_TypeDef *seq)
{
  I2C_TransferReturn_TypeDef ret;

#if defined(_SILICON_LABS_32B_SERIES_3)
  I2C_Device_Handle_TypeDef *i2cDevice = NULL;
  bool is10BitAddr = 0;

  // Get the I2C device handle of an instance.
  i2cDevice = get_i2c_device(I2C_NUM(i2c));

  // Do not try to read 0 bytes. It is not possible according to the I2C spec,
  // since the slave will always start sending the first byte ACK on an address.
  // The read operation can only be stopped by NACKing a received byte, i.e., minimum 1 byte.
  if (((seq->flags & I2C_FLAG_READ) && !(seq->buf[0].len))
      || ((seq->flags & I2C_FLAG_WRITE_READ) && !(seq->buf[1].len))
      ) {
    return i2cTransferUsageFault;
  }

  // Check whether the follower address is a 10bit address.
  is10BitAddr = (i2cDevice->followerAddress > 0xFF) ? true : false;

  // Assert on Invalid address.
  if (is10BitAddr == true) {
    EFM_ASSERT(i2cDevice->followerAddress <= SL_I2CSPM_INVALID_10BIT_FOLLOWER_ADDRESS);
  } else {
    EFM_ASSERT(i2cDevice->followerAddress <= SL_I2CSPM_INVALID_7BIT_FOLLOWER_ADDRESS);
  }
  // Update I2C device handle of an instance.
  i2cDevice->followerAddress = seq->addr;
  i2cDevice->state = i2cStateInProgress;
  i2cDevice->result = i2cTransferInProgress;
  i2cDevice->flags = seq->flags;
  i2cDevice->offset = 0;

  // Ensure buffers are empty.
  sl_hal_i2c_flush_buffers(i2cDevice->port);

  // Clear all pending interrupts prior to starting a transfer.
  sl_hal_i2c_clear_interrupts(i2cDevice->port, _I2C_IF_MASK);

  // Enable the I2C Interrupts.
  sl_hal_i2c_enable_interrupts(i2cDevice->port, (I2C_IEN_NACK | I2C_IEN_ACK | I2C_IEN_MSTOP | SL_I2CSPM_IEN_ERRORS));

  switch (i2cDevice->flags) {
    case I2C_FLAG_WRITE:
    {
      // Update the buffers and data length.
      i2cDevice->txBuffer = seq->buf[0].data;
      i2cDevice->txLen = seq->buf[0].len;
      i2cDevice->rxBuffer = 0;
      i2cDevice->rxLen = 0;

      // I2C Send.
      ret = send(i2cDevice);
      break;
    }
    case I2C_FLAG_READ:
    {
      // Update the buffers and data length.
      i2cDevice->txBuffer = 0;
      i2cDevice->txLen = 0;
      i2cDevice->rxBuffer = seq->buf[0].data;
      i2cDevice->rxLen = seq->buf[0].len;

      // I2C Receive.
      ret = receive(i2cDevice);
      break;
    }
    case I2C_FLAG_WRITE_READ:
    case I2C_FLAG_WRITE_WRITE:
    {
      // Update the buffers and data length.
      i2cDevice->txBuffer = seq->buf[0].data;
      i2cDevice->txLen = seq->buf[0].len;
      i2cDevice->rxBuffer = seq->buf[1].data;
      i2cDevice->rxLen = seq->buf[1].len;

      // I2C Transfer.
      ret = transfer(i2cDevice);
      break;
    }
    default:
    {
      ret = i2cTransferUsageFault;
      break;
    }
  }
#else
  uint32_t timeout = SL_I2CSPM_TRANSFER_TIMEOUT;
  /* Do a polled transfer */
  ret = I2C_TransferInit(i2c, seq);
  while (ret == i2cTransferInProgress && timeout--) {
    ret = I2C_Transfer(i2c);
  }
#endif //_SILICON_LABS_32B_SERIES_3

  return ret;
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
#if defined(_SILICON_LABS_32B_SERIES_3)
/***************************************************************************//**
 * Get the I2C device handle of an instance.
 *
 * @param[in]  instance   Instance of the I2C.
 *
 * @return                Pointer to I2C device handle of an instance.
 ******************************************************************************/
static I2C_Device_Handle_TypeDef* get_i2c_device(int8_t instance)
{
  if (instance < 0 || instance >= I2C_COUNT) {
    // Assert on invalid instance number.
    EFM_ASSERT(false);
  }
  return (&i2cDeviceHandle[instance]);
}

/***************************************************************************//**
 * Sends the follower address.
 *
 * @param[in]  is10BitAddr   Indicates 7bit or 10bit address.
 *
 * @param[in]  rwDirection   Read or Write Direction.
 *
 * @param[in]  i2cDevice     I2C device handle of an instance.
 ******************************************************************************/
static void send_follower_address(bool is10BitAddr, uint8_t rwDirection, I2C_Device_Handle_TypeDef *i2cDevice)
{
  uint16_t followerAddr = 0;

  // Check for 10bit or 7bit Follower Address.
  if (is10BitAddr) {
    // Change the State.
    i2cDevice->state = i2cState10bitAdd;

    // Frame the follower address with read/write direction.
    followerAddr = (((i2cDevice->followerAddress >> 8) & 0x06) | (SL_I2CSPM_FIRST_BYTE_10BIT_ADDR_MASK | rwDirection));
  } else {
    // Change the State.
    i2cDevice->state = i2cState7bitAdd;

    // Frame the follower address with read/write direction.
    followerAddr = ( (i2cDevice->followerAddress & SL_I2CSPM_7BIT_FOLLOWER_ADDRESS_MASK) | rwDirection);
  }

  // Send Start command.
  sl_hal_i2c_start_cmd(i2cDevice->port);

  // Send Follower Address.
  sl_hal_i2c_tx(i2cDevice->port, followerAddr);
}

/***************************************************************************//**
 * This function sends the data to the follower.
 *
 * @param[in]  i2cDevice     I2C device handle of an instance.
 *
 * @return                   Returns status of ongoing transfer.
 ******************************************************************************/
static I2C_TransferReturn_TypeDef send(I2C_Device_Handle_TypeDef *i2cDevice)
{
  uint32_t timeout = SL_I2CSPM_TRANSFER_TIMEOUT;
  bool is10BitAddr = 0;

  // Check whether the follower address is a 10bit address.
  is10BitAddr = (i2cDevice->followerAddress > 0xFF) ? true : false;

  // Send follower address with Write direction.
  send_follower_address(is10BitAddr, SL_I2CSPM_WRITE_DIRECTION, i2cDevice);

  // Run through the state machine until timeout or change of result.
  while (i2cDevice->result == i2cTransferInProgress && timeout--) {
    i2cspm_state_machine(i2cDevice);
  }

  return i2cDevice->result;
}

/***************************************************************************//**
 * This function receives the data from the follower.
 *
 * @param[in]  i2cDevice     I2C device handle of an instance.
 *
 * @return                   Returns status of ongoing transfer.
 ******************************************************************************/
static I2C_TransferReturn_TypeDef receive(I2C_Device_Handle_TypeDef *i2cDevice)
{
  uint32_t timeout = SL_I2CSPM_TRANSFER_TIMEOUT;
  bool is10BitAddr = 0;

  // Check whether the follower address is a 10bit address.
  is10BitAddr = (i2cDevice->followerAddress > 0xFF) ? true : false;

  // Enable I2C Receive Data Valid Interrupt.
  sl_hal_i2c_enable_interrupts(i2cDevice->port, I2C_IEN_RXDATAV);

  // Send follower address.
  if (is10BitAddr) {
    send_follower_address(is10BitAddr, SL_I2CSPM_WRITE_DIRECTION, i2cDevice);
  } else {
    send_follower_address(is10BitAddr, SL_I2CSPM_READ_DIRECTION, i2cDevice);
  }

  // Run through the state machine until timeout or change of result.
  while (i2cDevice->result == i2cTransferInProgress && timeout--) {
    i2cspm_state_machine(i2cDevice);
  }

  return i2cDevice->result;
}

/***************************************************************************//**
 * This function transmits and receives the data from the follower.
 *
 * @param[in]  i2cDevice     I2C device handle of an instance.
 *
 * @return                   Returns status of ongoing transfer.
 ******************************************************************************/
static I2C_TransferReturn_TypeDef transfer(I2C_Device_Handle_TypeDef *i2cDevice)
{
  I2C_TransferReturn_TypeDef returnStatus = i2cTransferInProgress;
  uint32_t timeout = SL_I2CSPM_TRANSFER_TIMEOUT;

  if (i2cDevice->flags == I2C_FLAG_WRITE_READ) {
    // Send the data.
    returnStatus = send(i2cDevice);

    // Check for the return status and perform receive operation.
    if (returnStatus == i2cTransferDone) {
      // Update the flag to read operation.
      i2cDevice->flags = I2C_FLAG_READ;

      // Update the result.
      i2cDevice->result = i2cTransferInProgress;

      // Receive the data.
      returnStatus = receive(i2cDevice);
    }
  } else if (i2cDevice->flags == I2C_FLAG_WRITE_WRITE) {
    // Send the data.
    returnStatus = send(i2cDevice);

    // Check for the return status and perform send operation.
    if (returnStatus == i2cTransferDone) {
      // Update the flag to write operation.
      i2cDevice->flags = I2C_FLAG_WRITE;

      // Update the result.
      i2cDevice->result = i2cTransferInProgress;

      // Update the State.
      i2cDevice->state = i2cState7bitAdd;

      // Update the buffers and data length.
      i2cDevice->txBuffer = i2cDevice->rxBuffer;
      i2cDevice->txLen = i2cDevice->rxLen;
      i2cDevice->rxLen = 0;
      i2cDevice->offset = 0;

      // Transmit data.
      sl_hal_i2c_tx(i2cDevice->port, i2cDevice->txBuffer[i2cDevice->offset++]);

      // Run through the state machine until timeout or change of result.
      while (i2cDevice->result == i2cTransferInProgress && timeout--) {
        i2cspm_state_machine(i2cDevice);
      }

      //Update the return status.
      returnStatus = i2cDevice->result;
    }
  } else {
    // To avoid SonarQube error.
  }

  return returnStatus;
}

/***************************************************************************//**
 * This function handles the data transmission.
 *
 * @param[in]  i2cDevice     I2C device handle of an instance.
 ******************************************************************************/
void transmit_handler(I2C_Device_Handle_TypeDef *i2cDevice)
{
  if (i2cDevice->offset < i2cDevice->txLen) {
    // Transmit data.
    sl_hal_i2c_tx(i2cDevice->port, i2cDevice->txBuffer[i2cDevice->offset]);

    // Increment the offset.
    i2cDevice->offset++;
  } else {
    // Reset the Offset to handle the next transaction.
    i2cDevice->offset = 0;

    // Update the State.
    i2cDevice->state = i2cStateDone;

    if (i2cDevice->flags == I2C_FLAG_WRITE) {
      // Send stop command.
      sl_hal_i2c_stop_cmd(i2cDevice->port);
    } else {
      // Update the result.
      i2cDevice->result = i2cTransferDone;
    }
  }
}

/***************************************************************************//**
 * This function handles the data reception.
 *
 * @param[in]  i2cDevice     I2C device handle of an instance.
 ******************************************************************************/
void receive_handler(I2C_Device_Handle_TypeDef *i2cDevice)
{
  if (i2cDevice->offset < i2cDevice->rxLen) {
    // Read the data.
    i2cDevice->rxBuffer[i2cDevice->offset++] = sl_hal_i2c_rx(i2cDevice->port);

    // Clear Receive Data Valid interrupt.
    sl_hal_i2c_clear_interrupts(i2cDevice->port, I2C_IF_RXDATAV);

    if (i2cDevice->offset == i2cDevice->rxLen) {
      // Send Nack.
      sl_hal_i2c_send_nack(i2cDevice->port);
    } else {
      // Send Ack.
      sl_hal_i2c_send_ack(i2cDevice->port);
    }
  }
}

/***************************************************************************//**
 * This function handles I2C state machine.
 *
 * @param[in]  i2cDevice     I2C device handle of an instance.
 ******************************************************************************/
static void i2cspm_state_machine(I2C_Device_Handle_TypeDef *i2cDevice)
{
  uint32_t pendingIrq = 0;

  // Get the pending interrupt.
  pendingIrq = sl_hal_i2c_get_pending_interrupts(i2cDevice->port);

  // Check for STOP to have been sent.
  if (pendingIrq & I2C_IF_MSTOP) {
    // Clear all interrupts.
    sl_hal_i2c_clear_interrupts(i2cDevice->port, _I2C_IF_MASK);

    // Disable all interrupts.
    sl_hal_i2c_disable_interrupts(i2cDevice->port, _I2C_IEN_MASK);

    // Update the pending irq.
    pendingIrq = 0;

    // Update the result unless a fault has already occurred.
    if (i2cDevice->result == i2cTransferInProgress) {
      i2cDevice->result = i2cTransferDone;
    }
  }

  // Check for Receive Data Valid interrupt.
  if (pendingIrq & I2C_IF_RXDATAV) {
    // Call Receive handler.
    receive_handler(i2cDevice);

    if (i2cDevice->offset == i2cDevice->rxLen) {
      // Reset the Offset to handle the next transaction.
      i2cDevice->offset = 0;

      // Update the State.
      i2cDevice->state = i2cStateDone;

      // Send stop command.
      sl_hal_i2c_stop_cmd(i2cDevice->port);
    }
  }

  // Check for Ack.
  if (pendingIrq & I2C_IF_ACK) {
    // Clear ACK interrupt.
    sl_hal_i2c_clear_interrupts(i2cDevice->port, I2C_IF_ACK);

    // Check for 7bit follower address transfer.
    if (i2cDevice->state == i2cState7bitAdd) {
      if (i2cDevice->flags != I2C_FLAG_READ) {
        // Call Transmit handler.
        transmit_handler(i2cDevice);
      }
    } else if (i2cDevice->state == i2cState10bitAdd) {
      // Check for 10bit follower address transfer.
      if (i2cDevice->flags == I2C_FLAG_WRITE) {
        i2cDevice->state = i2cStateAck;
      } else if (i2cDevice->flags == I2C_FLAG_READ) {
        i2cDevice->state = i2cState10bitAddWithRepStart;
      }

      // Send second byte of 10bit follower address.
      sl_hal_i2c_tx(i2cDevice->port, (i2cDevice->followerAddress & 0xFF));
    } else if (i2cDevice->state == i2cState10bitAddWithRepStart) {
      // Check for 10bit follower address with repeated start.
      i2cDevice->state = i2cStateDataReceive;

      // Send Repeated Start and address with read direction.
      send_follower_address(false, SL_I2CSPM_READ_DIRECTION, i2cDevice);
    } else if (i2cDevice->state == i2cStateAck) {
      // Check for Ack.
      transmit_handler(i2cDevice);
    } else {
      // To avoid SonarQube error.
    }
  }

  // Check for NACK.
  if (pendingIrq & I2C_IF_NACK) {
    i2cDevice->state = i2cStateNack;
    i2cDevice->result = i2cTransferNack;

    // Send stop command.
    sl_hal_i2c_stop_cmd(i2cDevice->port);
  }

  // Check for I2C Errors.
  if (pendingIrq & SL_I2CSPM_IF_ERRORS) {
    // Update the state.
    i2cDevice->state = i2cStateError;

    // Check for Bus error.
    if (pendingIrq & I2C_IF_BUSERR) {
      i2cDevice->result = i2cTransferBusErr;
    }

    // Check for Arbitration lost interrupt.
    if (pendingIrq & I2C_IF_ARBLOST) {
      i2cDevice->result = i2cTransferArbLost;
    }

    // Send stop command.
    sl_hal_i2c_stop_cmd(i2cDevice->port);
  }
}
#endif // _SILICON_LABS_32B_SERIES_3
