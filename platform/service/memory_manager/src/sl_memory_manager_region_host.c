/***************************************************************************//**
 * @file
 * @brief Getters for Heap and stack
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <stdint.h>
#include <stdalign.h>
#include "sl_memory_manager.h"

/* Prevent's compilation errors when building in simulation */
#ifndef   __USED
  #define __USED
#endif

// Simulated C heap size.
#define SL_MM_HEAP_SIZE  (4 * 1024)

/*******************************************************************************
 ***************************  GLOBAL VARIABLES   *******************************
 ******************************************************************************/

// Simulated C heap aligned on 8 bytes (ARM data object alignment requirement).
alignas(8) uint8_t sl_heap[SL_MM_HEAP_SIZE];

/*******************************************************************************
 ***************************  UTILITY FUNCTIONS   ******************************
 ******************************************************************************/

/***************************************************************************//**
 * Get size and location of the heap.
 *
 * @note
 *   sl_memory_get_heap_region
 ******************************************************************************/
sl_memory_region_t sl_memory_get_heap_region(void)
{
  sl_memory_region_t region;

  region.addr = &sl_heap[0];
  region.size = SL_MM_HEAP_SIZE;

  return region;
}

/***************************************************************************//**
 * Get size and location of the stack. (Mock)
 *
 * @note
 *   sl_memory_get_heap_region
 ******************************************************************************/
sl_memory_region_t sl_memory_get_stack_region(void)
{
  sl_memory_region_t region;

  region.addr = NULL;
  region.size = 0;

  return region;
}

#if defined(__GNUC__)
__USED void * _sbrk(int incr)
{
  (void)incr;

  /*
   * This means there is an issue with the setup of C standard library.
   */
  while (1) {
    // infinite loop
  }
}
#endif
