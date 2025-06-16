/***************************************************************************//**
 * @file
 * @brief API definitions for memory usage statistics from sl_memory_manager.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

// NOTE for size_t
#include <stddef.h>
#include <stdint.h>
// NOTE for assert
#include <assert.h>
// NOTE for bool
#include <stdbool.h>
#include "sl_memory_manager.h"
#include "sl_zigbee_memory_usage.h"

// init data
static size_t init_used_heap = 0;
static size_t init_heap_high_watermark = 0;

void sli_zigbee_stack_memory_usage_init(void)
{
  init_used_heap = sl_memory_get_used_heap_size();
  init_heap_high_watermark = sl_memory_get_heap_high_watermark();
}

uint32_t sli_zigbee_stack_get_memory_usage_data(sl_zigbee_memory_usage_data_t memory_usage_data_type)
{
  uint32_t memory_usage_value = 0;
  switch (memory_usage_data_type) {
    case TOTAL_HEAP_SIZE:
      memory_usage_value = (uint32_t) sl_memory_get_total_heap_size();
      break;
    case CURRENT_USED_HEAP_SIZE:
      memory_usage_value = (uint32_t) sl_memory_get_used_heap_size();
      break;
    case CURRENT_HEAP_HIGH_WATERMARK:
      memory_usage_value = (uint32_t) sl_memory_get_heap_high_watermark();
      break;
    case INIT_USED_HEAP_SIZE:
      memory_usage_value = (uint32_t) init_used_heap;
      break;
    case INIT_HEAP_HIGH_WATERMARK:
      memory_usage_value = (uint32_t) init_heap_high_watermark;
      break;
    default:
      assert(false);
      break;
  }
  return memory_usage_value;
}
