/**
 * @file
 * Offers Power Management commands for Silabs targets only.
 * @attention Must be linked for Silabs build targets only.
 * @copyright 2022 Silicon Laboratories Inc.
 */
#include <stdint.h>
#include "cmd_handlers.h"
#include "SerialAPI.h"
#include "app.h"
#include "zw_power_manager_ids.h"
#include "SwTimer.h"
#include "AppTimer.h"

extern SSwTimer mWakeupTimer;

ZW_ADD_CMD(FUNC_ID_PM_STAY_AWAKE)
{
  /* HOST->ZW: PowerLock Type, timeout of stay awake, timeout of wakeup */
  /*           Power locks type 0 for radio and 1 for peripheral*/
  uint32_t timeout = (uint32_t)(frame->payload[1] << 24);
  timeout |= (uint32_t)(frame->payload[2] << 16);
  timeout |= (uint32_t)(frame->payload[3] << 8);
  timeout |= (uint32_t)(frame->payload[4]);

  uint32_t timeoutwakeup = (uint32_t)(frame->payload[5] << 24);
  timeoutwakeup |= (uint32_t)(frame->payload[6] << 16);
  timeoutwakeup |= (uint32_t)(frame->payload[7] << 8);
  timeoutwakeup |= (uint32_t)(frame->payload[8]);

  if (0 == frame->payload[0]) {
    // use relock to force acquisition of the lock
    zw_power_manager_relock(ZPAL_PM_TYPE_USE_RADIO, timeout, ZPAL_PM_APP_RADIO_APPLICATION_ID);
  } else if (1 == frame->payload[0]) {
    // use relock to force acquisition of the lock
    zw_power_manager_relock(ZPAL_PM_TYPE_DEEP_SLEEP, timeout, ZPAL_PM_APP_DEEP_SLEEP_APPLICATION_ID);
  }

  if (timeout && timeoutwakeup) {
    AppTimerDeepSleepPersistentStart(&mWakeupTimer, timeoutwakeup);
  }
  set_state_and_notify(stateIdle);
}

ZW_ADD_CMD(FUNC_ID_PM_CANCEL)
{
  /* HOST->ZW: PowerLock Type*/
  /*Power locks type 0 for radio and 1 for peripheral*/
  if (0 == frame->payload[0]) {
    zw_power_manager_lock_cancel(ZPAL_PM_TYPE_USE_RADIO, ZPAL_PM_APP_RADIO_APPLICATION_ID);
  } else if (1 == frame->payload[0]) {
    zw_power_manager_lock_cancel(ZPAL_PM_TYPE_DEEP_SLEEP, ZPAL_PM_APP_DEEP_SLEEP_APPLICATION_ID);
  }
  set_state_and_notify(stateIdle);
}
