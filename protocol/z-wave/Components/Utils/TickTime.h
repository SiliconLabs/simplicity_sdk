/**
 * @file
 * @brief TickTime header file
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef _TICKTIME_H_
#define _TICKTIME_H_

#include <FreeRTOS.h>
#include <task.h>

/**
 * Get the value of 32 bit Tick timer at 1ms ticks.
 * @attention It is not allowed to call method from ISR
 * @return tick count
 */
static inline uint32_t getTickTime(void)
{
  return xTaskGetTickCount();
}

/**
 * Calculate the difference between two tick times.
 *
 * @param iStartTickTime Start tick time
 * @param iEndTickTime End tick time
 * @return Difference between end tick time and start tick time
 */
static inline uint32_t getTickTimeDiff(uint32_t iStartTickTime, uint32_t iEndTickTime)
{
  return ((iEndTickTime >= iStartTickTime) ? iEndTickTime - iStartTickTime : ~iStartTickTime + 1 + iEndTickTime);
}

/**
 * Get passed tickTime ticks since iStartTickTime.
 *
 * Only useful if passed tickTime ticks never exceeds 0xFFFFFFFF as
 * passed tickTime ticks are calculated using a 32 bit sized free running
 * 1ms tick counter.
 * @attention It is not allowed to call method from ISR
 * Max passed tickTime ticks ~ 50 days
 *
 * @param iStartTickTime Start tick time
 * @return passed tick time
 */
static inline uint32_t getTickTimePassed(uint32_t iStartTickTime)
{
  return getTickTimeDiff(iStartTickTime, getTickTime());
}

#endif // _TICKTIME_H_
