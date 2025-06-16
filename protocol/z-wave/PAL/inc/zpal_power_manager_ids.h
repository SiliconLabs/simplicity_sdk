#ifndef _ZPAL_POWER_MANAGER_IDS_H_
#define _ZPAL_POWER_MANAGER_IDS_H_

/* Z-Wave PAL Power Manager IDs */

// @note to prevent undefined behavior `END` of enum needs to be not NULL (used for array size)
typedef enum {
  ZPAL_PM_PAL_DEEP_SLEEP_DUMMY_ID = 0,
  ZPAL_PM_PAL_DEEP_SLEEP_END
} ZPAL_PM_PalDeepSleepID;

typedef enum {
  ZPAL_PM_PAL_RADIO_GECKO_ID,
  ZPAL_PM_PAL_RADIO_END
} ZPAL_PM_PalRadioID;

#endif // _ZPAL_POWER_MANAGER_IDS_H_
