/***************************************************************************//**
 * @file simple_rail_tx_cli.h
 * @brief Simple RAIL Rx CLI Component
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SIMPLE_RAIL_RX_CLI_H
#define SIMPLE_RAIL_RX_CLI_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * RAIL Rx state machine, used by rail_rx_cli
 *
 * @param None
 * @returns None
 *****************************************************************************/
void sl_simple_rail_rx_cli_tick(void);

/**************************************************************************//**
 * Initializes a static rx_fifo ptr, points to the buffer where the
 * RAIL packet content is copied
 *
 * @param None
 * @returns None
 *****************************************************************************/
void sl_simple_rail_rx_cli_init(void);

#endif // SIMPLE_RAIL_RX_CLI_H
