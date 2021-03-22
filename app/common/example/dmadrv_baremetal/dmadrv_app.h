/***************************************************************************//**
 * @file
 * @brief DMADRV examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef DMADRV_APP_H
#define DMADRV_APP_H

/***************************************************************************//**
 * Initialize example
 ******************************************************************************/
void dmadrv_app_init(void);

/***************************************************************************//**
 * Ticking function
 ******************************************************************************/
void dmadrv_app_process_action(void);

#endif  // DMADRV_APP_H
