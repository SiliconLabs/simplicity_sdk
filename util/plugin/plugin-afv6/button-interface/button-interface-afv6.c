// *****************************************************************************
// This file exists to forward the halButtonIsr to sl_util_af_hal_button_isr_cb.
// In AFv2, halButtonIsr is blind forwarded to sl_util_af_hal_button_isr_cb. To
// make it so there aren't different implementation.c files for each framework
// for button-interface, it will perform this blind forward as well.
//
// Copyright 2016 Silicon Laboratories, Inc.                               *80*
// *****************************************************************************

#include PLATFORM_HEADER

extern void sl_util_af_hal_button_isr_cb(uint8_t button, uint8_t state);

void halButtonIsr(uint8_t button, uint8_t state)
{
  sl_util_af_hal_button_isr_cb(button, state);
}
