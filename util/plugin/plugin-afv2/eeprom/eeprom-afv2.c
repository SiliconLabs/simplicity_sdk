/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 * These callbacks are defined so that other parts of the system can make
 * these calls regardless of whether the EEPROM is actually used.  A
 * user generated callback will be created if they don't use
 * the plugin.
 *
 *****************************************************************************/

#include EMBER_AF_API_AF_HEADER
#include EMBER_AF_API_EEPROM

void sl_util_af_eeprom_init_cb(void)
{
  sl_util_af_eeprom_init();
}

void sl_util_af_eeprom_note_initialized_state_cb(bool state)
{
  sl_util_af_eeprom_note_initialized_state(state);
}

void sl_util_af_eeprom_shutdown_cb(void)
{
  sl_util_af_eeprom_shutdown();
}
