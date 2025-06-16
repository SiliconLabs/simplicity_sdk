/***************************************************************************//**
 * @file
 * @brief Main file for Main Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "config/btl_config.h"
#include "api/btl_interface.h"

#include "core/btl_core.h"
#include "core/btl_helper.h"
#include "core/btl_reset.h"
#include "core/btl_parse.h"
#include "core/btl_bootload.h"

#include "debug/btl_debug.h"

#ifdef BTL_GPIO_ACTIVATION
#include "gpio/gpio-activation/btl_gpio_activation.h"
#endif

#ifdef BTL_EZSP_GPIO_ACTIVATION
#include "gpio/ezsp-gpio-activation/btl_ezsp_gpio_activation.h"
#endif

#ifdef BOOTLOADER_SUPPORT_STORAGE
#include "storage/btl_storage.h"
#include "storage/bootloadinfo/btl_storage_bootloadinfo.h"
#include "btl_storage_cfg.h"
#endif

#ifdef BOOTLOADER_SUPPORT_COMMUNICATION
#include "communication/btl_communication.h"
#endif

#include "em_device.h"
#include "sl_hal_gpio.h"

#if defined(__GNUC__)
#define ROM_END_SIZE 0
extern const size_t __rom_end__;
#elif defined(__ICCARM__)
#define ROM_END_SIZE 4
const size_t __rom_end__ @ "ROM_SIZE";
#endif

// -----------------------------------------------------------------------------
// Test Functions

// --------------------------------
// Local function declarations

__STATIC_INLINE bool enter_bootloader(void);
SL_NORETURN static void boot_to_app(uint32_t);

void HardFault_Handler(void)
{
  BTL_DEBUG_PRINTLN("Fault          ");
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
}

// Main Bootloader implementation

int main(void)
{
  int32_t ret = BOOTLOADER_ERROR_STORAGE_BOOTLOAD;

  BTL_DEBUG_PRINTLN("BTL entry");

#if defined(EMU_CMD_EM01VSCALE2) && defined(EMU_STATUS_VSCALEBUSY)
  // Device supports voltage scaling, and the bootloader may have been entered
  // with a downscaled voltage. Scale voltage up to allow flash programming.
  if ((EMU->STATUS & EMU_STATUS_VSCALE_VSCALE2) != EMU_STATUS_VSCALE_VSCALE2) {
    EMU->CMD = EMU_CMD_EM01VSCALE2;
    while (EMU->STATUS & EMU_STATUS_VSCALEBUSY) {
      // Do nothing
    }
  }
#endif

  btl_init();

#ifdef BOOTLOADER_SUPPORT_STORAGE
  if (!reset_resetCounterEnabled()) {
    // Storage bootloaders might use part of the reason signature as a counter,
    // so only invalidate the signature when the counter is not in use.
    reset_invalidateResetReason();
  }
#else
  reset_invalidateResetReason();
#endif

#ifdef BOOTLOADER_SUPPORT_STORAGE
  // If the bootloader supports storage, first attempt to apply an existing
  // image from storage.

  ret = storage_main();

  if (ret == BOOTLOADER_OK) {
    // Firmware upgrade from storage successful. Disable the reset counter
    // and return to application
    if (reset_resetCounterEnabled()) {
      reset_disableResetCounter();
    }
    reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
  } else {
    if (!reset_resetCounterEnabled()) {
      // Start counting the number of consecutive resets after the first reset
      reset_enableResetCounter();
    }

    // Stop after three consecutive resets (the first one counts as 0)
    if (reset_getResetCounter() >= 2) {
      // If the system is not able to recover from a fault like BADAPP or
      // BADIMAGE, wait in a busy loop to ease reflashing and debugging.
      BTL_DEBUG_PRINTLN("Reset loop detected. Stopping...");
      reset_invalidateResetReason();
      while (1) {
        // Wait...
      }
    } else {
      reset_incrementResetCounter();
    }

    // Wait a short while (approx. 500 ms) before continuing.
    // This allows other operations to complete before the reset.
    for (volatile int i = 800000; i > 0; i--) {
      // Do nothing
    }
  }
#endif

#ifdef BOOTLOADER_SUPPORT_COMMUNICATION
  communication_init();

  ret = communication_start();
  if (ret != BOOTLOADER_OK) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  }

  ret = communication_main();
  BTL_DEBUG_PRINT("Protocol returned ");
  BTL_DEBUG_PRINT_WORD_HEX(ret);
  BTL_DEBUG_PRINT_LF();

  communication_shutdown();

  if ((ret == BOOTLOADER_OK)
      || (ret == BOOTLOADER_ERROR_COMMUNICATION_DONE)) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
  }
#endif // BOOTLOADER_SUPPORT_COMMUNICATION

  // An error occurred in storage or communication, and a firmware upgrade
  // was not performed
  if (0
#ifdef BOOTLOADER_SUPPORT_COMMUNICATION
      || (ret == BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR)
      || (ret == BOOTLOADER_ERROR_COMMUNICATION_TIMEOUT)
#endif
#ifdef BOOTLOADER_SUPPORT_STORAGE
      || (ret == BOOTLOADER_ERROR_STORAGE_BOOTLOAD)
#endif
      ) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_BADIMAGE);
  } else {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  }

  return 0;
}

#ifdef BOOTLOADER_SUPPORT_STORAGE
extern const BootloaderStorageFunctions_t storageFunctions;
#endif

const MainBootloaderTable_t mainStageTable = {
  {
    .type = BOOTLOADER_MAGIC_MAIN,
    .layout = BOOTLOADER_HEADER_VERSION_MAIN,
    .version = BOOTLOADER_VERSION_MAIN
  },
  // Bootloader size is the relative address of the end variable plus 4 for the
  // CRC
  .size = ((uint32_t)&__rom_end__) - BTL_MAIN_STAGE_BASE + ROM_END_SIZE + 4,
  .startOfAppSpace = (BareBootTable_t *)(BTL_APPLICATION_BASE),
  .endOfAppSpace = 0,
  .capabilities = (0
#if defined(BOOTLOADER_ENFORCE_SIGNED_UPGRADE) && (BOOTLOADER_ENFORCE_SIGNED_UPGRADE == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_SIGNATURE
#endif
#if defined(BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE) && (BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_ENCRYPTION
#endif
#if defined(BOOTLOADER_ENFORCE_SECURE_BOOT) && (BOOTLOADER_ENFORCE_SECURE_BOOT == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_SECURE_BOOT
#endif
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_CERTIFICATE_SECURE_BOOT
#endif
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
                   | BOOTLOADER_CAPABILITY_ROLLBACK_PROTECTION
#endif
                   | BOOTLOADER_CAPABILITY_BOOTLOADER_UPGRADE
                   | BOOTLOADER_CAPABILITY_GBL
                   | BOOTLOADER_CAPABILITY_GBL_SIGNATURE
#if !defined(BTL_PARSER_NO_SUPPORT_ENCRYPTION)
                   | BOOTLOADER_CAPABILITY_GBL_ENCRYPTION
#endif
#if defined(BOOTLOADER_SUPPORT_STORAGE) && (BOOTLOADER_SUPPORT_STORAGE == 1)
                   | BOOTLOADER_CAPABILITY_STORAGE
#endif
#if defined(BOOTLOADER_SUPPORT_COMMUNICATION) && (BOOTLOADER_SUPPORT_COMMUNICATION == 1)
                   | BOOTLOADER_CAPABILITY_COMMUNICATION
#endif
#if defined(BOOTLOADER_REJECT_DIRECT_SIGNED_IMG) && (BOOTLOADER_REJECT_DIRECT_SIGNED_IMG == 1)
                   | BOOTLOADER_CAPABILITY_REJECT_DIRECT_SIGNED_IMG
#endif
                   ),
  .init = &btl_init,
  .deinit = &btl_deinit,
  .verifyApplication = &bootload_verifyApplication,
  .initParser = &core_init_parser,
  .parseBuffer = &core_parse_buffer,
#if defined(BOOTLOADER_SUPPORT_STORAGE) && (BOOTLOADER_SUPPORT_STORAGE == 1)
  .storage = &storageFunctions,
#else
  .storage = NULL,
#endif
  .parseImageInfo = core_parse_image_info,
  .parserContextSize = core_parser_context_size,
  .remainingApplicationUpgrades = NULL,
  .getPeripheralList = NULL,
  .getUpgradeLocation = NULL
};

#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
const ApplicationCertificate_t sl_app_certificate = {
  .structVersion = APPLICATION_CERTIFICATE_VERSION,
  .flags = { 0U },
  .key = { 0U },
  .version = 0,
  .signature = { 0U },
};
#endif

const ApplicationProperties_t sl_app_properties = {
  .magic = APPLICATION_PROPERTIES_MAGIC,
  .structVersion = APPLICATION_PROPERTIES_VERSION,
  .signatureType = APPLICATION_SIGNATURE_NONE,
  .signatureLocation = ((uint32_t)&__rom_end__) - BTL_MAIN_STAGE_BASE + ROM_END_SIZE,
  .app = {
    .type = APPLICATION_TYPE_BOOTLOADER,
    .version = BOOTLOADER_VERSION_MAIN,
    .capabilities = 0UL,
    .productId = { 0U },
  },
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
  // If certificate based boot chain is enabled, the bootloader binary will be provided with
  // a certificate that does not contain any key.
  // A valid certificate needs to be injected to the bootloader images using Simplicity Commander.
  // Simplicity Commander will replace this certificate.
  .cert = (ApplicationCertificate_t *)&sl_app_certificate,
#else
  .cert = NULL,
#endif
  .longTokenSectionAddress = NULL,
  .decryptKey = { 0u },
};

/**
 * This function gets executed before ANYTHING got initialized.
 * So, no using global variables here!
 */
void SystemInit2(void)
{
  // Initialize debug before first debug print
  BTL_DEBUG_INIT();

  // Assumption: We should enter the app
  volatile bool enterApp = true;
  // Assumption: The app should be verified
  volatile bool verifyApp = true;

  if (enter_bootloader()) {
    // We want to enter the bootloader, app doesn't need to be verified
    enterApp = false;
    verifyApp = false;
  }

#if defined(LOCKBIT_SKIP_BOOT_CHECK)
  bool skipLockBitCheck = true;
#else
  bool skipLockBitCheck = false;
#endif

  sl_se_code_region_config_t region_config = { 0 };
  sl_se_command_context_t cmd_ctx = { 0 };
  sl_se_init_command_context(&cmd_ctx);
  sl_se_code_region_get_config(&cmd_ctx, &region_config, 1, 1);

  uint32_t startOfAppSpace = (uint32_t)mainStageTable.startOfAppSpace;

  if ((region_config.locked == true) || skipLockBitCheck) {
    // Sanity check application program counter
    uint32_t pc = *(uint32_t *)(startOfAppSpace + 4);
    if (pc == 0xFFFFFFFF) {
      // Sanity check failed; enter the bootloader
      reset_setResetReason(BOOTLOADER_RESET_REASON_BADAPP);
      enterApp = false;
      verifyApp = false;
    }
    // App should be verified
    if (verifyApp) {
      // If app verification fails, enter bootloader instead
      enterApp = bootload_verifyApplication(startOfAppSpace);
      if (!enterApp) {
        BTL_DEBUG_PRINTLN("App verify fail");
        reset_setResetReason(BOOTLOADER_RESET_REASON_BADAPP);
      }
    }
  } else {
    enterApp = false;
  }

  if (enterApp) {
    BTL_DEBUG_PRINTLN("Enter app");
    BTL_DEBUG_PRINT_LF();

#if defined(BOOTLOADER_SUPPORT_STORAGE)
    // Disable the reset counter if we're booting (back) into the application
    if (reset_resetCounterEnabled()) {
      reset_disableResetCounter();
    }
#endif

    // Set vector table to application's table
    SCB->VTOR = startOfAppSpace;

    boot_to_app(startOfAppSpace);
  }
  // Enter bootloader
}

/**
 * Jump to app
 */
__attribute__ ((noreturn, naked)) static void boot_to_app(uint32_t startOfAppSpace)
{
  jump_to_application_routine(startOfAppSpace);
  while (1) {
    // Do nothing
  }
}

/**
 * Check whether we should enter the bootloader
 *
 * @return True if the bootloader should be entered
 */
__STATIC_INLINE bool enter_bootloader(void)
{
// *INDENT-OFF*
#if defined(EMU_RSTCAUSE_SYSREQ)
  if (EMU->RSTCAUSE & EMU_RSTCAUSE_SYSREQ) {
#else
  if (RMU->RSTCAUSE & RMU_RSTCAUSE_SYSREQRST) {
#endif
    // Check if we were asked to run the bootloader...
    switch (reset_classifyReset()) {
      case BOOTLOADER_RESET_REASON_BOOTLOAD:
      case BOOTLOADER_RESET_REASON_FORCE:
      case BOOTLOADER_RESET_REASON_UPGRADE:
      case BOOTLOADER_RESET_REASON_BADAPP:
        // Asked to go into bootload mode
        return true;
      default:
        break;
    }
  }
// *INDENT-ON*

#ifdef BTL_GPIO_ACTIVATION
  if (gpio_enterBootloader()) {
    // GPIO pin state signals bootloader entry
    return true;
  }
#endif

#ifdef BTL_EZSP_GPIO_ACTIVATION
  if (ezsp_gpio_enterBootloader()) {
    // GPIO pin state signals bootloader entry
    return true;
  }
#endif

  return false;
}
