/***************************************************************************//**
 * @file
 * @brief Communication component implementing BLE Apploader OTA DFU protocol
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Includes

#include "btl_apploader.h"
#include "communication/btl_communication.h"
#include "core/btl_bootload.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_RAIL_UTIL_PTI_PRESENT
#include "sl_rail_util_pti.h"
#endif

// -----------------------------------------------------------------------------
// Functions

void communication_init(void)
{
  bootloader_apploader_communication_init();
#ifdef SL_CATALOG_RAIL_UTIL_PTI_PRESENT
  sl_rail_util_pti_init();
#endif
}

int32_t communication_start(void)
{
  return bootloader_apploader_communication_start();
}

int32_t communication_main(void)
{
  int32_t ret = BOOTLOADER_OK;

  const BootloaderParserCallbacks_t parseCb = {
    .context = NULL,
    .applicationCallback = bootload_applicationCallback,
    .metadataCallback = NULL,
    .bootloaderCallback = bootload_bootloaderCallback
  };

  ImageProperties_t imageProps = {
    .contents = 0U,
    .instructions = 0xFFU,
    .imageCompleted = false,
    .imageVerified = false,
    .bootloaderVersion = 0,
    .application = { 0 },
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
    .seUpgradeVersion = 0
#endif
  };

  ParserContext_t parserContext = { 0 };
  DecryptContext_t decryptContext = { 0 };
  AuthContext_t authContext = { 0 };

  ret = bootloader_apploader_communication_main(&imageProps,
                                                &parserContext,
                                                &decryptContext,
                                                &authContext,
                                                &parseCb);
  return ret;
}

void communication_shutdown(void)
{
  // Do nothing
}
