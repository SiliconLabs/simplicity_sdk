/***************************************************************************//**
 * @file
 * @brief Memory Manager Driver's Memory Pool Lightweight Feature Implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <string.h>

#include "sl_memory_manager.h"
#include "sli_memory_manager.h"
#include "sl_assert.h"
#include "sl_core.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
#include "sli_memory_profiler.h"
#endif

#define SLI_MEM_POOL_OUT_OF_MEMORY     0xFFFFFFFF
#define SLI_MEM_POOL_REQUIRED_PADDING(obj_size) (((sizeof(size_t) - ((obj_size) % sizeof(size_t))) % sizeof(size_t)))

/***************************************************************************//**
 * Creates a memory pool.
 ******************************************************************************/
sl_status_t sl_memory_create_pool(size_t block_size,
                                  uint32_t block_count,
                                  sl_memory_pool_t *pool_handle)
{
  sl_status_t status = SL_STATUS_OK;
  uint8_t *block = NULL;
  size_t block_addr;
  size_t pool_size;
  EFM_ASSERT(block_count != 0);
  EFM_ASSERT(block_size != 0);

  if (pool_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // SLI_MEM_POOL_REQUIRED_PADDING Rounds up to the nearest platform-dependant size. On a 32-bit processor,
  // it will be rounded-up to 4 bytes. E.g. 101 bytes will be rounded up to 104 bytes.
  pool_handle->block_size = block_size + (uint16_t)SLI_MEM_POOL_REQUIRED_PADDING(block_size);
  pool_handle->block_count = block_count;

  // Reserve a block in which the entire pool will reside. Uses a long term allocation to keep
  // behavior similar to dynamic reservation.
  pool_size = pool_handle->block_size * pool_handle->block_count;
  status = sl_memory_alloc(pool_size, BLOCK_TYPE_LONG_TERM, (void **)&block);

  if (status != SL_STATUS_OK) {
    return status;
  }

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  // Create the tracker for the pool with no description. The code that created
  // the pool can add the tracker description if relevant.
  sli_memory_profiler_create_pool_tracker(pool_handle, NULL, block, pool_size);
#endif

  pool_handle->block_address = (void *)block;

  // Returned block pointer not used because its reference is already stored in block_address.
  (void)&block;

  pool_handle->block_free = (uint32_t *)pool_handle->block_address;

  block_addr = (size_t)pool_handle->block_address;

  // Populate the list of free blocks except the last block.
  for (uint16_t i = 0; i < (block_count - 1); i++) {
    *(size_t *)block_addr = block_addr + pool_handle->block_size;
    block_addr += pool_handle->block_size;
  }

  // Last element will indicate out of memory.
  *(size_t *)block_addr = SLI_MEM_POOL_OUT_OF_MEMORY;

  return status;
}

/***************************************************************************//**
 * Deletes a memory pool.
 *
 * @note The pool_handle provided is neither freed or invalidated. It can be
 *       reused in a new call to sl_memory_create_pool() to create another pool.
 ******************************************************************************/
sl_status_t sl_memory_delete_pool(sl_memory_pool_t *pool_handle)
{
  sl_status_t status;

  // Verify that the handle pointer isn't NULL.
  if (pool_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  // Delete the memory tracker
  sli_memory_profiler_delete_tracker(pool_handle);
#endif

  // Free block.
  status = sl_memory_free(pool_handle->block_address);

  return status;
}

/***************************************************************************//**
 * Allocates a block from a memory pool.
 ******************************************************************************/
sl_status_t sl_memory_pool_alloc(sl_memory_pool_t *pool_handle,
                                 void **block)
{
  CORE_DECLARE_IRQ_STATE;

  if ((pool_handle == NULL) || (block == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  // No block allocated yet.
  *block = NULL;

  CORE_ENTER_ATOMIC();

  if ((size_t)pool_handle->block_free == SLI_MEM_POOL_OUT_OF_MEMORY) {
    CORE_EXIT_ATOMIC();
#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
    SLI_MEMORY_PROFILER_TRACK_ALLOC(pool_handle, NULL, pool_handle->block_size);
#endif
    return SL_STATUS_EMPTY;
  }

  // Get the next free block.
  void *block_addr = pool_handle->block_free;

  // Update the next free block using the address saved in that block.
  pool_handle->block_free = (void *)*(size_t *)block_addr;

  CORE_EXIT_ATOMIC();

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  SLI_MEMORY_PROFILER_TRACK_ALLOC(pool_handle, block_addr, pool_handle->block_size);
#endif

  *block = block_addr;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Frees a block from a memory pool.
 ******************************************************************************/
sl_status_t sl_memory_pool_free(sl_memory_pool_t *pool_handle,
                                void *block)
{
  CORE_DECLARE_IRQ_STATE;

  if ((pool_handle == NULL) || (block == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Validate that the provided address is in the pool payload range.
  EFM_ASSERT((block >= pool_handle->block_address) \
             && ((size_t)block <= ((size_t)pool_handle->block_address + (pool_handle->block_size * pool_handle->block_count))));

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  SLI_MEMORY_PROFILER_TRACK_FREE(pool_handle, block);
#endif

  CORE_ENTER_ATOMIC();

  // Save the current free block address in this block.
  *(size_t *)block = (size_t)pool_handle->block_free;
  pool_handle->block_free = block;

  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Dynamically allocates a memory pool handle.
 ******************************************************************************/
sl_status_t sl_memory_pool_handle_alloc(sl_memory_pool_t **pool_handle)
{
  sl_status_t status;

  // Allocate pool_handle as a long-term block.
  status = sl_memory_alloc(sizeof(sl_memory_pool_t), BLOCK_TYPE_LONG_TERM, (void **)pool_handle);

  return status;
}

/***************************************************************************//**
 * Frees a dynamically allocated memory pool handle.
 ******************************************************************************/
sl_status_t sl_memory_pool_handle_free(sl_memory_pool_t *pool_handle)
{
  sl_status_t status;

  // Free memory pool_handle.
  status = sl_memory_free((void *)pool_handle);

  return status;
}

/***************************************************************************//**
 * Gets the size of the memory pool handle structure.
 ******************************************************************************/
uint32_t sl_memory_pool_handle_get_size(void)
{
  return sizeof(sl_memory_pool_t);
}
