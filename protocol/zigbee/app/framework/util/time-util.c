/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "../include/af.h"
#include "af-main.h"
#include "app/framework/util/common.h"
#include "time-util.h"

#if (defined SL_ZIGBEE_AF_PLUGIN_TIME_SERVER) || (defined  SL_CATALOG_ZIGBEE_TIME_SERVER_PRESENT)
#include "app/framework/plugin/time-server/time-server.h"
#endif

#define mYEAR_IS_LEAP_YEAR(year)  ( ((year) % 4 == 0) && (((year) % 100 != 0) || ((year) % 400 == 0)) )

const uint8_t sl_zigbee_af_days_in_month[] =
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint8_t sl_zigbee_af_get_number_days_in_month(sl_zigbee_af_time_struct_t *time)
{
  uint8_t daysInMonth = 0;
  if ( (time->month > 0) && (time->month < 13) ) {
    daysInMonth = sl_zigbee_af_days_in_month[time->month - 1];
    if ( mYEAR_IS_LEAP_YEAR(time->year) && (time->month == 2) ) {
      daysInMonth++;
    }
  }
  return daysInMonth;
}

void sl_zigbee_af_copy_date(uint8_t *data, uint16_t index, sl_zigbee_af_date_t *src)
{
  data[index]   = src->year;
  data[index + 1] = src->month;
  data[index + 2] = src->dayOfMonth;
  data[index + 3] = src->dayOfWeek;
}

int8_t sl_zigbee_af_compare_dates(sl_zigbee_af_date_t* date1, sl_zigbee_af_date_t* date2)
{
  uint32_t val1 = sl_zigbee_af_encode_date(date1);
  uint32_t val2 = sl_zigbee_af_encode_date(date2);
  return (val1 == val2) ? 0 : ((val1 < val2) ? -1 : 1);
}

void sl_zigbee_af_fill_time_struct_from_utc(uint32_t utcTime,
                                            sl_zigbee_af_time_struct_t* returnTime)
{
  bool isLeapYear = true; // 2000 was a leap year
  uint32_t i;
  uint32_t daysSince2000 = utcTime / SECONDS_IN_DAY;
  uint32_t secToday = utcTime - (daysSince2000 * SECONDS_IN_DAY);
  returnTime->hours = (uint8_t)(secToday / SECONDS_IN_HOUR);
  returnTime->minutes = (uint8_t)((secToday
                                   - (returnTime->hours * SECONDS_IN_HOUR)) / 60);
  returnTime->seconds = (uint8_t)(secToday
                                  - ((returnTime->hours * SECONDS_IN_HOUR)
                                     + (returnTime->minutes * 60)));
  returnTime->year = 2000;
  returnTime->month = 1;
  returnTime->day = 1;

  // march through the calendar, counting months, days and years
  // being careful to account for leapyears.
  for (i = 0; i < daysSince2000; i++) {
    uint8_t daysInMonth;
    if (isLeapYear && (returnTime->month == 2)) {
      daysInMonth = 29;
    } else {
      daysInMonth = sl_zigbee_af_days_in_month[returnTime->month - 1];
    }
    if (daysInMonth == returnTime->day) {
      returnTime->month++;
      returnTime->day = 1;
    } else {
      returnTime->day++;
    }
    if (returnTime->month == 13) {
      returnTime->year++;
      returnTime->month = 1;
      if (returnTime->year % 4 == 0
          && (returnTime->year % 100 != 0
              || (returnTime->year % 400 == 0))) {
        isLeapYear = true;
      } else {
        isLeapYear = false;
      }
    }
  }
}

uint32_t sl_zigbee_af_get_utc_from_time_struct(sl_zigbee_af_time_struct_t *time)
{
  // Construct a UTC timestamp given an sl_zigbee_af_time_struct_t structure.
  uint32_t utcTime = 0;
  uint16_t daysThisYear;
  uint32_t i;

  if ( (time == NULL) || (time->year < 2000) || (time->month == 0)
       || (time->month > 12) || (time->day == 0) || (time->day > 31) ) {
    return 0xFFFFFFFFU;    // Invalid parameters
  }

  for ( i = 2000; i < time->year; i++ ) {
    utcTime += (365 * SECONDS_IN_DAY);
    if ( mYEAR_IS_LEAP_YEAR(i) ) {
      utcTime += SECONDS_IN_DAY;
    }
  }
  sl_zigbee_af_app_println("Seconds in %d years=%d", i, utcTime);
  // Utc Time now reflects seconds up to Jan 1 00:00:00 of current year.
  daysThisYear = 0;
  for ( i = 0; i < (uint32_t)(time->month - 1); i++ ) {
    daysThisYear += sl_zigbee_af_days_in_month[i];
    if ( (i == 1) && (mYEAR_IS_LEAP_YEAR(time->year)) ) {
      daysThisYear++;
    }
  }
  daysThisYear += time->day - 1;
  utcTime += SECONDS_IN_DAY * daysThisYear;
  sl_zigbee_af_app_println("daysThisYear=%d, total Sec=%d (0x%4x)", daysThisYear, utcTime, utcTime);

  // Utc Time now reflects seconds up to last completed day of current year.
  for ( i = 0; i < time->hours; i++ ) {
    utcTime += SECONDS_IN_HOUR;
  }
  //for( i=0; i<time->minutes; i++ ){
  //iutcTime += 60;
  //}
  utcTime += (60 * time->minutes);
  utcTime += time->seconds;
  return utcTime;
}

// Determine which day of the week it is, from a given utc timestamp.
// Return 0=MON, 1=TUES, etc.
uint8_t sl_zigbee_af_get_weekday_from_utc(uint32_t utcTime)
{
  uint8_t dayIndex;
  utcTime %= SECONDS_IN_WEEK;

  for ( dayIndex = 0; dayIndex < 7; dayIndex++ ) {
    if ( utcTime < SECONDS_IN_DAY ) {
      break;
    }
    utcTime -= SECONDS_IN_DAY;
  }
  // Note:  Jan 1, 2000 is a SATURDAY.
  // Do some translation work so 0=MONDAY, 5=SATURDAY, 6=SUNDAY
  if ( dayIndex < 2 ) {
    dayIndex += 5;
  } else {
    dayIndex -= 2;
  }
  return dayIndex;
}

void sl_zigbee_af_decode_date(uint32_t src, sl_zigbee_af_date_t* dest)
{
  dest->year       = (uint8_t)((src & 0xFF000000U) >> 24);
  dest->month      = (uint8_t)((src & 0x00FF0000U) >> 16);
  dest->dayOfMonth = (uint8_t)((src & 0x0000FF00U) >>  8);
  dest->dayOfWeek  = (uint8_t) (src & 0x000000FFU);
}

uint32_t sl_zigbee_af_encode_date(sl_zigbee_af_date_t* date)
{
  uint32_t result = ((((uint32_t)date->year) << 24)
                     + (((uint32_t)date->month) << 16)
                     + (((uint32_t)date->dayOfMonth) << 8)
                     + (((uint32_t)date->dayOfWeek)));
  return result;
}

// sl_zigbee_af_print_time expects to be passed a ZigBee time which is the number
// of seconds since the year 2000, it prints out a human readable time
// from that value.
void sl_zigbee_af_print_time(uint32_t utcTime)
{
#if defined(EMBER_AF_PRINT_ENABLE) || defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT)
  sl_zigbee_af_time_struct_t time;
  sl_zigbee_af_fill_time_struct_from_utc(utcTime, &time);
  sl_zigbee_af_println(sl_zigbee_af_print_active_area,
                       "UTC time: %d/%d/%d %d:%d:%d (%4x)",
                       time.month,
                       time.day,
                       time.year,
                       time.hours,
                       time.minutes,
                       time.seconds,
                       utcTime);
#endif //EMBER_AF_PRINT_ENABLE || SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
}

void sl_zigbee_af_print_time_iso_format(uint32_t utcTime)
{
#if defined(EMBER_AF_PRINT_ENABLE) || defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT)
  sl_zigbee_af_time_struct_t time;
  sl_zigbee_af_fill_time_struct_from_utc(utcTime, &time);
  sl_zigbee_af_print(sl_zigbee_af_print_active_area,
                     "%d-%d-%d %d:%d:%d",
                     time.year,
                     time.month,
                     time.day,
                     time.hours,
                     time.minutes,
                     time.seconds);
#endif //EMBER_AF_PRINT_ENABLE || SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
}

void sl_zigbee_af_print_date(const sl_zigbee_af_date_t * date)
{
#if defined(EMBER_AF_PRINT_ENABLE) || defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT)
  uint32_t zigbeeDate = ((((uint32_t)date->year) << 24)
                         + (((uint32_t)date->month) << 16)
                         + (((uint32_t)date->dayOfMonth) << 8)
                         + (((uint32_t)date->dayOfWeek)));

  sl_zigbee_af_print(sl_zigbee_af_print_active_area,
                     "0x%4X (%d/%p%d/%p%d)",
                     zigbeeDate,
                     date->year + 1900,
                     (date->month < 10 ? "0" : ""),
                     date->month,
                     (date->dayOfMonth < 10 ? "0" : ""),
                     date->dayOfMonth);
#endif //EMBER_AF_PRINT_ENABLE  ||  SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
}

void sl_zigbee_af_print_dateln(const sl_zigbee_af_date_t * date)
{
  sl_zigbee_af_print_date(date);
  sl_zigbee_af_println(sl_zigbee_af_print_active_area, "");
}

// *******************************************************
// sl_zigbee_af_print_time and sl_zigbee_af_set_time are convienience methods for setting
// and displaying human readable times.

// Expects to be passed a ZigBee time which is the number of seconds
// since the year 2000
void sl_zigbee_af_set_time(uint32_t utcTime)
{
#if (defined SL_ZIGBEE_AF_PLUGIN_TIME_SERVER)  || (defined  SL_CATALOG_ZIGBEE_TIME_SERVER_PRESENT)
  sli_zigbee_af_time_cluster_server_set_current_time(utcTime);
#endif //SL_ZIGBEE_AF_PLUGIN_TIME_SERVER
  sl_zigbee_af_set_time_cb(utcTime);
}

uint32_t sl_zigbee_af_get_current_time(void)
{
#if (defined SL_ZIGBEE_AF_PLUGIN_TIME_SERVER)  || (defined  SL_CATALOG_ZIGBEE_TIME_SERVER_PRESENT)
  return sli_zigbee_af_time_cluster_server_get_current_time();
#else
  return sl_zigbee_af_get_current_time_cb();
#endif
}
