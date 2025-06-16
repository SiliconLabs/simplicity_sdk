/*******************************************************************************
 * @file app_pm_transition_event.c
 * @brief This file contains the implementation of the power manager transition
 * event for EM4 deepest sleep.
 *******************************************************************************
 * # License
 * <b> Copyright 2024 Silicon Laboratories Inc. www.silabs.com </b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of the Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 * By installing, copying or otherwise using this software, you agree to the
 * terms of the MSLA.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_power_manager.h"
#include "sl_dcdc.h"
#include "zpal_power_manager.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// Application callback to handle the power manager events/transitions.
static sl_power_manager_em_transition_event_handle_t pm_event_handle = { 0 };
static void ZW_PmTransitionEventCallback(sl_power_manager_em_t from, sl_power_manager_em_t to);
static const sl_power_manager_em_transition_event_info_t pm_event_info =
{
  .event_mask = SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0
                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1
                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2,
  .on_event = ZW_PmTransitionEventCallback
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void ZW_PmTransitionEventInit(void)
{
  // register callback from power manager transitions
  sl_power_manager_subscribe_em_transition_event(&pm_event_handle, &pm_event_info);
}

void ZW_PmTransitionEventCallback(sl_power_manager_em_t from, sl_power_manager_em_t to)
{
  /*
     For never listenning device the maximum available power mode is EM4.

     Condition to go to EM4:
     - The sl_power_manager is transitioning to EM2 or above
     - The device is of type never listenning. (This app is of type never listenning)
     - Z-Wave did not set an EM2 lock <=> ZPAL_PM_DEEP_SLEEP

     note: the sleeptimer component provided by platform add an EM2 requirement.
     Because the sleeptimer is always running the power manager will never transition to EM3.
     In this case we therefore highjack power manager to go to EM4.
   */

  if ((to >= SL_POWER_MANAGER_EM2)
      && (zpal_pm_lock_type_is_active(ZPAL_PM_TYPE_DEEP_SLEEP, NULL) == false)) {
    to = SL_POWER_MANAGER_EM4;
  }

  /* We are moving towards sleep modes, call zwave handler before platform stuff is done */
  if (to > from) {
    zpal_zw_pm_event_handler((zpal_pm_powerlevel_t) from, (zpal_pm_powerlevel_t) to);
  }

  if ((from >= SL_POWER_MANAGER_EM2) && (to < SL_POWER_MANAGER_EM2)) {
    //Wake up from EM2 or EM3 sleep
    sl_dcdc_exit_em2();
  }

  if (to == SL_POWER_MANAGER_EM2) {
    //Go to EM2 sleep
    sl_dcdc_setup_em2();
  } else if (to > SL_POWER_MANAGER_EM2) {
    //Go to EM4
    zpal_pm_halt();
  }

  /* We are moving away from sleep modes, call zwave handler after platform stuff is done */
  if (to < from) {
    zpal_zw_pm_event_handler((zpal_pm_powerlevel_t)from, (zpal_pm_powerlevel_t) to);
  }
}
