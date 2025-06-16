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

#ifndef SL_I2C_H
#define SL_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_device_peripheral.h"
#include "sl_device_i2c.h"
#include "sl_device_gpio.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

/* *INDENT-OFF* */
// *****************************************************************************
/// @addtogroup i2c I2C - Inter-Integrated Circuit
/// @brief Inter-Integrated Circuit driver
///
/// @li @ref i2c_intro
///
///@n @section i2c_intro Introduction
///  This is an I2C Driver.
///
/// @{
// *****************************************************************************
/* *INDENT-ON* */

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
/// Freq mode enum
SL_ENUM(sl_i2c_freq_mode_t) {
  SL_I2C_FREQ_STANDARD_MODE,     /// Standard mode max frequency assuming using 4:4 ratio for Nlow:Nhigh.
  SL_I2C_FREQ_FAST_MODE,         /// Fast mode max frequency assuming using 6:3 ratio for Nlow:Nhigh.
  SL_I2C_FREQ_FASTPLUS_MODE,     /// Fast mode+ max frequency assuming using 11:6 ratio for Nlow:Nhigh.
};

/// Transaction mode enum
SL_ENUM(sl_i2c_transfer_seq_t) {
  SL_I2C_WRITE = (1 << 0),         /// Sending the data
  SL_I2C_READ = (1 << 1),          /// Reading the data
  SL_I2C_WRITE_READ = (2 << 1),    /// Sending and reading the data
};

/// Transfer event enum
SL_ENUM(sl_i2c_event_t) {
  SL_I2C_EVENT_IN_PROGRESS = 0,         /// Transfer in progress
  SL_I2C_EVENT_COMPLETED = 1,           /// Transfer completed successfully
  SL_I2C_EVENT_NACK_RECEIVED = 2,       /// NACK received
  SL_I2C_EVENT_ARBITRATION_LOST = 3,    /// Arbitration lost
  SL_I2C_EVENT_BUS_ERROR = 4,           /// Bus error occurred
  SL_I2C_EVENT_STOP = 5,                /// Transfer stopped
  SL_I2C_EVENT_SW_FAULT = 6,            /// Transfer fault due to software
  SL_I2C_EVENT_IDLE = 7,                /// Instance is idle
  SL_I2C_EVENT_INVALID_ADDR = 8,        /// Address invalid error
  SL_I2C_EVENT_TIMEOUT = 9,             /// Timeout
};

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
/// FIFO Threshold struct
#if defined(SL_CATALOG_I2C_FIFO_PRESENT)
typedef struct {
  sl_i2c_fifo_threshold_t tx_threshold_val;     /// Transmit FIFO Threshold value
  sl_i2c_fifo_threshold_t rx_threshold_val;     /// Receive FIFO Threshold value
} sl_i2c_fifo_threshold_level_t;
#endif

/// I2C Tx and Rx DMA Channel and trigger source.
typedef struct {
  unsigned int dma_tx_channel;                        /// DMA Channel assigned for Tx operations
  unsigned int dma_rx_channel;                        /// DMA Channel assigned for Rx operations
} sl_i2c_dma_channel_info_t;

/**
 * @struct sl_i2c_init_params_t
 * @brief Initialization parameters for an I2C Driver instance.
 *        This structure is passed to @ref sl_i2c_init() during initialization
 *        of an I2C instance.
 *
 * @note Usage of `follower_address` depends on the operating mode:
 *       Leader Mode   : Used to specify the address of the follower device.
 *       Follower Mode : Used to set the I2C device's own (self) address.
 */
typedef struct {
  I2C_TypeDef *i2c_base_addr;                     ///< I2C Module Instance
  sl_i2c_operating_mode_t operating_mode;         ///< Operating mode: Leader or Follower
  sl_i2c_freq_mode_t freq_mode;                   ///< I2C Speed mode (Standard, Fast, etc.)
  uint16_t follower_address;                      ///< Follower address (see usage note above)
  sl_gpio_t scl_gpio;                             ///< SCL GPIO Port and Pin (Serial Clock Line)
  sl_gpio_t sda_gpio;                             ///< SDA GPIO Port and Pin (Serial Data Line)
#if defined(SL_CATALOG_I2C_FIFO_PRESENT)
  sl_i2c_fifo_threshold_level_t *fifo_threshold;  ///< Transmit and Receive FIFO Threshold levels
#endif
} sl_i2c_init_params_t;

/*******************************************************************************
 *******************************   TYPEDEFS   **********************************
 ******************************************************************************/
typedef void *sl_i2c_handle_t;  /// i2c instance handle

/***************************************************************************//**
 * I2C interrupt callback function pointer.
 *
 * @param[in] transfer_event  Transfer Event.
 * @param[in] context         User-defined context or data.
 *
 * @return  return status.
 ******************************************************************************/
typedef sl_status_t (*sl_i2c_irq_callback_t)(sl_i2c_event_t transfer_event, void *context);

/***************************************************************************//**
 * This function initializes the I2C Module.
 *
 * @param[in]  p_init_params    A pointer to Init Params.
 * @param[out] i2c_handle       I2C instance handle.
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_init(const sl_i2c_init_params_t *init_params,
                        sl_i2c_handle_t *i2c_handle);

/***************************************************************************//**
 * This function Deinitializes and resets the I2C Instance.
 *
 * @param[in] i2c_handle       I2C instance handle.
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_deinit(sl_i2c_handle_t i2c_handle);

/***************************************************************************//**
 * This function sets the frequency of I2C Bus.
 *
 * @param[in] i2c_handle       I2C instance handle.
 * @param[in] freq_mode        Operating Speed.

 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_set_frequency(sl_i2c_handle_t i2c_handle,
                                 sl_i2c_freq_mode_t freq_mode);

/***************************************************************************//**
 * This function gets the current frequency of I2C module.
 *
 * @param[in] i2c_handle       I2C instance handle.
 * @param[out] frequency       Currently configured frequency.

 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_get_frequency(sl_i2c_handle_t i2c_handle,
                                 uint32_t *frequency);

/***************************************************************************//**
 * This API is supported only in Leader mode and it is used to set the follower address.
 * It is used in Multi Follower mode (addressing a new follower which has similar configurations set during Init API).
 *
 * @param[in] i2c_handle        I2C instance handle.
 * @param[in] follower_address  Follower address to which the I2C Leader wants to communicate.
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_set_follower_address(sl_i2c_handle_t i2c_handle,
                                        uint16_t follower_address);

/***************************************************************************//**
 * This API is for configuring the DMA for the I2C instance.
 *
 * @note  This is an optional API that can be used only in the case where the
 *        user wants to use their own DMA channels and overwrite the existing allocated DMA channels.
 *
 * @param[in] i2c_handle        I2C instance handle.
 * @param[in] dma_channel       DMA Channel Info.
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_configure_dma(sl_i2c_handle_t i2c_handle,
                                 sl_i2c_dma_channel_info_t dma_channel);

/***************************************************************************//**
 * Leader Mode : This function is used to send the data to the follower configured during Init API.
 * Follower Mode : This function is used to send the data to the addressed I2C Leader.
 * Returns on transfer complete or on error.
 *
 * @param[in] i2c_handle      I2C Instance handle.
 * @param[in] tx_buffer       A pointer to the transmit data buffer.
 * @param[in] tx_len          Transmit data length.
 * @param[in] timeout         Timeout of the blocking call to exit the loop (in milliseconds).
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_send_blocking(sl_i2c_handle_t i2c_handle,
                                 const uint8_t *tx_buffer,
                                 uint16_t tx_len,
                                 uint32_t timeout);

/***************************************************************************//**
 * Leader Mode : This function is used to receive the data from the follower configured during Init API.
 * Follower Mode : This function is used to receive the data from the addressed I2C Leader.
 * Returns on transfer complete or on error.
 *
 * @param[in] i2c_handle      I2C Instance handle.
 * @param[in] rx_buffer       A pointer to the Receive data buffer.
 * @param[in] rx_len          Receive data length.
 * @param[in] timeout         Timeout of the blocking call to exit the loop (in milliseconds).
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_receive_blocking(sl_i2c_handle_t i2c_handle,
                                    uint8_t *rx_buffer,
                                    uint16_t rx_len,
                                    uint32_t timeout);

/***************************************************************************//**
 * This function is used to send and receive the data from the follower configured during Init API.
 * This API is supported only in Leader Mode and for blocking transfer.
 * Returns on transfer complete or on error.
 *
 * @param[in] i2c_handle    I2C Instance handle.
 * @param[in] tx_buffer     A pointer to transmit data buffer
 * @param[in] tx_len        Transmit data length
 * @param[in] rx_buffer     A pointer to receive data buffer
 * @param[in] rx_len        Receive data length
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_transfer(sl_i2c_handle_t i2c_handle,
                            const uint8_t *tx_buffer,
                            uint16_t tx_len,
                            uint8_t *rx_buffer,
                            uint16_t rx_len);

/***************************************************************************//**
 * Leader Mode : This function uses DMA and Interrupt, to send the data to the follower set up during Init API.
 * Follower Mode : This function uses DMA and Interrupt, to send the data to the addressed I2C Leader.
 * The user is notified through the provided callback function upon completion.
 *
 * @param[in] i2c_handle         I2C Instance handle.
 * @param[in] tx_buffer          A pointer to transmit data buffer
 * @param[in] tx_len             Transmit data length
 * @param[in] i2c_callback       A callback function on completion.
 * @param[in] context            A pointer to user-defined data for callback.
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_send_non_blocking(sl_i2c_handle_t i2c_handle,
                                     const uint8_t *tx_buffer,
                                     uint16_t tx_len,
                                     sl_i2c_irq_callback_t i2c_callback,
                                     void *context);

/***************************************************************************//**
 * Leader Mode : This function uses DMA and Interrupt, to receive the data from the follower set up during Init API.
 * Follower Mode : This function uses DMA and Interrupt, to receive the data from the addressed I2C Leader.
 * The user is notified through the provided callback function upon completion.
 *
 * @param[in] i2c_handle         I2C Instance handle.
 * @param[in] rx_buffer          A pointer to receive data buffer
 * @param[in] rx_len             Receive data length
 * @param[in] i2c_callback       A callback function on completion.
 * @param[in] context            A pointer to user-defined data for callback.
 *
 * @return  return status.
 ******************************************************************************/
sl_status_t sl_i2c_receive_non_blocking(sl_i2c_handle_t i2c_handle,
                                        uint8_t *rx_buffer,
                                        uint16_t rx_len,
                                        sl_i2c_irq_callback_t i2c_callback,
                                        void *context);

/** @} (end addtogroup i2c driver) */
#ifdef __cplusplus
}
#endif

#endif /* SL_I2C_H */
