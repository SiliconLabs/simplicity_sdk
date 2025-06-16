/***************************************************************************//**
 * @file
 * @brief I2C Driver Private API definition.
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

#ifndef SLI_I2C_H
#define SLI_I2C_H

#include "dmadrv.h"
#include "sl_i2c.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
// Max number of descriptors for dma tx and rx operations.
#define SL_I2C_DMA_MAX_TX_DESCRIPTOR_COUNT 2
#define SL_I2C_DMA_MAX_RX_DESCRIPTOR_COUNT 5

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
/// I2C state enum
SL_ENUM(sli_i2c_transaction_state_t) {
  SLI_I2C_STATE_ERROR = 0,                                      /// Indicates an error occurred.
  SLI_I2C_STATE_SEND_START_AND_ADDR = 1,                        /// Send start and address byte.
  SLI_I2C_STATE_ADDR_WAIT_FOR_ACK_OR_NACK = 2,                  /// Wait for ACK/NACK on address byte.
  SLI_I2C_STATE_SEND_REPEATED_START_AND_ADDR = 3,               /// Send repeated start and address byte.
  SLI_I2C_STATE_REPEATED_ADDR_WAIT_FOR_ACK_OR_NACK = 4,         /// Wait for ACK/NACK for repeated start.
  SLI_I2C_STATE_ADDRESS_MATCH = 5,                              /// Wait for address matching.
  SLI_I2C_STATE_ADDR_2ND_BYTE_10BIT_WAIT_FOR_ACK_OR_NACK = 6,   /// Wait for ACK/NACK on 2nd byte (10-bit).
  SLI_I2C_STATE_10BIT_ADDRESS_MATCH = 7,                        /// Wait for 10-bit address match.
  SLI_I2C_STATE_REP_ADDR_MATCH = 18,                            /// Wait for repeated start and address match.
  SLI_I2C_STATE_SEND_DATA = 8,                                  /// Send data byte.
  SLI_I2C_STATE_WAIT_FOR_ACK_OR_NACK = 9,                       /// Wait for ACK or NACK after sending data.
  SLI_I2C_STATE_RECEIVE_DATA = 11,                              /// Data to be received.
  SLI_I2C_STATE_SEND_STOP = 13,                                 /// Send stop signal.
  SLI_I2C_STATE_WAIT_FOR_STOP = 14,                             /// Wait for stop condition.
  SLI_I2C_STATE_DONE = 16,                                      /// Transfer completed.
  SLI_I2C_STATE_TIMEOUT = 19                                    /// Timeout.
};

/// I2C transfer mode enum
SL_ENUM(sli_i2c_transfer_mode_t) {
  SLI_I2C_BLOCKING_TRANSFER = 0,       /// I2C blocking transfer
  SLI_I2C_NON_BLOCKING_TRANSFER = 1,   /// I2C non blocking transfer
};

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
/**
 * @struct sli_i2c_instance_t
 * @brief This struct represents an I2C instance, holding configuration details
 *        for the peripheral, buffers and transaction state.
 *
 * @note Usage of `follower_address` depends on the operating mode:
 *       Leader Mode   : Used to specify the address of the follower device.
 *       Follower Mode : Used to set the I2C device's own (self) address.
 */
typedef struct {
  I2C_TypeDef *i2c_base_addr;                        /// I2C base address.
  sl_i2c_operating_mode_t operating_mode;            /// Leader/Follower operating mode.
  uint16_t follower_address;                         /// Follower address.
  bool is_10bit_addr;                                /// 7-bit or 10-bit address selection.
  sl_gpio_t scl_gpio;                                /// SCL GPIO Port and Pin.
  sl_gpio_t sda_gpio;                                /// SDA GPIO Port and Pin.
  const uint8_t *tx_buffer;                          /// Transmit data buffer.
  uint8_t *rx_buffer;                                /// Receive data buffer.
  uint16_t tx_len;                                   /// Transmit buffer length.
  uint16_t rx_len;                                   /// Receive buffer length.
  uint16_t tx_offset;                                /// Transmit offset in buffer.
  uint16_t rx_offset;                                /// Receive offset in buffer.
  sl_i2c_transfer_seq_t transfer_seq;                /// I2C transaction sequence.
  sli_i2c_transaction_state_t state;                 /// I2C transaction state.
  sl_i2c_event_t transfer_event;                     /// I2C transfer event type.
  sli_i2c_transfer_mode_t transfer_mode;             /// I2C transfer mode (blocking/non-blocking).
  sl_i2c_dma_channel_info_t dma_channel;             /// I2C DMA Channel Info.
/// DMA descriptor arrays for TX and RX operations.
#if defined(EMDRV_DMADRV_LDMA)
  LDMA_Descriptor_t tx_desc[SL_I2C_DMA_MAX_TX_DESCRIPTOR_COUNT];
  LDMA_Descriptor_t rx_desc[SL_I2C_DMA_MAX_RX_DESCRIPTOR_COUNT];
  #elif defined(EMDRV_DMADRV_LDMA_S3)
  sl_hal_ldma_descriptor_t tx_desc[SL_I2C_DMA_MAX_TX_DESCRIPTOR_COUNT];
  sl_hal_ldma_descriptor_t rx_desc[SL_I2C_DMA_MAX_RX_DESCRIPTOR_COUNT];
  #endif
  uint8_t addr_buffer[3];                            /// Address buffer.
  sl_i2c_irq_callback_t callback;                    /// I2C Callback.
  void *context;                                     /// User-defined context.
} sli_i2c_instance_t;

/***************************************************************************//**
 * This function is used for the configuring I2C instance.
 *
 * @param[in] init_params    A pointer to Init Params.
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sli_i2c_instance_configuration(const sl_i2c_init_params_t *init_params);

/***************************************************************************//**
 * This function is used for the setting the follower address for follower.
 *
 * @param[in]  i2c_base_addr     Pointer to I2C Base Address.
 * @param[in]  follower_address  Follower address.
 * @param[in]  is_10bit_addr     Indicates whether the follower address is 7-bit or 10-bit.
 *                               True if the address is 10-bit, false if it is 7-bit.
 ******************************************************************************/
void sli_i2c_set_follower_address(I2C_TypeDef *i2c_base_addr,
                                  uint16_t follower_address,
                                  bool is_10bit_addr);

/***************************************************************************//**
 * This function is used for the configuring GPIOs for I2C instance.
 *
 * @param[in] i2c_base_addr     Pointer to I2C Base Address.
 * @param[in] sda_gpio          SDA GPIO Port and Pin.
 * @param[in] scl_gpio          SCL GPIO Port and Pin.
 * @param[in] enable            Enable/Disable the gpio.
 ******************************************************************************/
void sli_i2c_configure_gpio(I2C_TypeDef *i2c_base_addr,
                            sl_gpio_t sda_gpio,
                            sl_gpio_t scl_gpio,
                            bool enable);

/***************************************************************************//**
 * This function is used for the initialization of I2C instance.
 *
 * @param[in] i2c_base_addr     Pointer to I2C Base Address.
 * @param[in] operating_mode    Operating mode of the i2c instance (Leader/Follower).
 * @param[in] transfer_seq      Transfer Sequence.
 ******************************************************************************/
void sli_i2c_transfer_init(I2C_TypeDef *i2c_base_addr,
                           sl_i2c_operating_mode_t operating_mode,
                           sl_i2c_transfer_seq_t transfer_seq);

/***************************************************************************//**
 * This function is used for the initialization of I2C DMA transfer.
 *
 * @param[in] i2c_instance   I2C Instance
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sli_i2c_dma_transfer_init(sli_i2c_instance_t *i2c_instance);

#ifdef __cplusplus
}
#endif

#endif // SLI_I2C_H
