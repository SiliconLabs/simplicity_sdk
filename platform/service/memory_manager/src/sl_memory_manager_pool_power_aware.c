/***************************************************************************//**
 * @file
 * @brief Memory Manager Driver's Block Reservation Feature Implementation.
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
#include "sl_bit.h"
#include "em_device.h"
#include "sl_core.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
#include "sli_memory_profiler.h"
#endif

#if defined(SLI_MEMORY_MANAGER_ENABLE_SYSTEMVIEW)
#include <stdio.h>
#include "SEGGER_SYSVIEW.h"
uint32_t sl_cmm_pool_id_available = 0;
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Creates a memory pool.
 *
 * @note (1) The list tracking the free blocks is a continuous bitmap. Its
 *           content is dynamically allocated as a long-term block. There is
 *           one bit per block. If '1', the block is free for allocation.
 *           If '0', the block is already allocated.
 ******************************************************************************/
sl_status_t sl_memory_create_pool(size_t block_size,
                                  uint32_t block_count,
                                  sl_memory_pool_t *pool_handle)
{
  sl_status_t status;
  uint8_t *block = NULL;
  size_t size_free_list_bytes = 0u;
  uint32_t full_word_nbr;
  uint32_t partial_word_nbr_bits;

  EFM_ASSERT(block_size != 0u);
  EFM_ASSERT(block_count > 0u);

  // Verify that the handle pointer isn't NULL.
  if ((pool_handle == NULL) || (pool_handle->reservation == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Reserve a block in which the entire pool will reside.
  status = sl_memory_reserve_block((block_size * block_count),
                                   SL_MEMORY_BLOCK_ALIGN_DEFAULT,
                                   pool_handle->reservation,
                                   (void **)&block);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Returned block pointer not used because its reference is already stored in reservation handle.
  (void)&block;

  // Allocate a block containing the free list. See Note #1.
  size_free_list_bytes = SLI_POOL_BITS_TO_BYTE(block_count);
  size_free_list_bytes = SLI_ALIGN_ROUND_UP(size_free_list_bytes, SLI_WORD_SIZE_32);
  status = sl_memory_alloc(size_free_list_bytes, BLOCK_TYPE_LONG_TERM, (void **)&pool_handle->block_free);
  if (status != SL_STATUS_OK) {
    (void)sl_memory_release_block(pool_handle->reservation);
    return status;
  }
  // Initialize the bits of the free list bitmap matching the number of blocks.
  full_word_nbr = block_count / SLI_DEF_INT_32_NBR_BITS;
  if (full_word_nbr != 0u) {
    memset((void *)pool_handle->block_free, 0xFF, (full_word_nbr * SLI_WORD_SIZE_32));
  }
  partial_word_nbr_bits = block_count % SLI_DEF_INT_32_NBR_BITS;
  if (partial_word_nbr_bits != 0u) {
    // Clear the last partial location.
    memset((void *)&pool_handle->block_free[full_word_nbr], 0x00, SLI_WORD_SIZE_32);
    // Set only contiguous bit field of specified bit size (N bits).
    pool_handle->block_free[full_word_nbr] = SL_DEF_BIT(partial_word_nbr_bits) - 1u;
  }

  // Save total block number and block size to speed up alloc() and free().
  pool_handle->block_count = block_count;
  pool_handle->block_size = block_size;

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  // Create the tracker for the pool with no description. The code that created
  // the pool can add the tracker description if relevant.
  sli_memory_profiler_create_pool_tracker(pool_handle,
                                          NULL,
                                          pool_handle->reservation->block_address,
                                          block_count * block_size);
#endif
#if defined(SLI_MEMORY_MANAGER_ENABLE_SYSTEMVIEW)
  char pool_name[16] = "";
  sprintf(pool_name, "Pool %ld", sl_cmm_pool_id_available);
  sl_cmm_pool_id_available++;
  SEGGER_SYSVIEW_HeapDefine(pool_handle,
                            pool_handle->reservation->block_address,
                            block_count * block_size,
                            0);
  SEGGER_SYSVIEW_NameResource((uint32_t) pool_handle, pool_name);
#endif

  return status;
}

/***************************************************************************//**
 * Deletes a memory pool.
 *
 * @note The pool_handle provided is neither freed or invalidated. It can be
 *       reused in a new call to sl_memory_create_pool() to create another pool.
 *
 * @note The pool_handle provided is neither freed or invalidated. It can be
 *       reused in a new call to sl_memory_create_pool() to create another pool.
 ******************************************************************************/
sl_status_t sl_memory_delete_pool(sl_memory_pool_t *pool_handle)
{
  sl_status_t status;
  uint32_t full_word_nbr;
  uint32_t partial_word_nbr_bits;

  // Verify that the handle pointer isn't NULL.
  if ((pool_handle == NULL) || (pool_handle->reservation == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Verify that no blocks are allocated. (Free list is only 0xFF)
  full_word_nbr = pool_handle->block_count / SLI_DEF_INT_32_NBR_BITS;
  for (size_t k = 0u; k < full_word_nbr; k++) {
    if (pool_handle->block_free[k] != 0xFFFFFFFF) {
      return SL_STATUS_INVALID_STATE;
    }
  }

  partial_word_nbr_bits = pool_handle->block_count % SLI_DEF_INT_32_NBR_BITS;
  if ((partial_word_nbr_bits != 0u)
      && ((uint32_t)(SL_DEF_BIT(partial_word_nbr_bits) - 1u) != pool_handle->block_free[full_word_nbr])) {
    return SL_STATUS_INVALID_STATE;
  }

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  // Delete the memory tracker
  sli_memory_profiler_delete_tracker(pool_handle);
#endif

  // Release block.
  status = sl_memory_release_block(pool_handle->reservation);
  if (status != SL_STATUS_OK) {
    return status;
  }

  pool_handle->block_count = 0;
  pool_handle->block_size = 0;

  // Release free list.
  status = sl_memory_free((void *)pool_handle->block_free);

#if defined(SLI_MEMORY_MANAGER_ENABLE_SYSTEMVIEW)
  SEGGER_SYSVIEW_HeapFree(pool_handle, pool_handle->reservation);
#endif

  return status;
}

/***************************************************************************//**
 * Allocates a block from a memory pool.
 *
 * @note (1) Checking if the pool is empty (that is no more free blocks) is
 *           done during the search for a free block in the free list bitmap.
 *           It saves some space in the pool handle.
 *           The other method would be to keep track of the free
 *           blocks in a counter (part of the pool handle) and test it at the
 *           beginning of the function. But adding this counter would cost
 *           precious heap bytes if the pool handle is not aligned with the
 *           default alignment of 8 bytes.
 ******************************************************************************/
sl_status_t sl_memory_pool_alloc(sl_memory_pool_t *pool_handle,
                                 void **block)
{
  uint32_t free_list_size_byte = 0u;
  uint32_t free_list_size_word = 0u;
  uint32_t bitmap_ix = 0u;
  uint32_t block_ix = 0u;

  if ((pool_handle == NULL) || (block == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  *block = NULL; // No block allocated yet.

  free_list_size_byte = SLI_POOL_BITS_TO_BYTE(pool_handle->block_count);
  free_list_size_byte = SLI_ALIGN_ROUND_UP(free_list_size_byte, SLI_WORD_SIZE_32);
  free_list_size_word = free_list_size_byte / SLI_WORD_SIZE_32;

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  // Search the free list bitmap for a free block.
  while ((bitmap_ix < free_list_size_word) && (pool_handle->block_free[bitmap_ix] == 0u)) {
    block_ix += SLI_DEF_INT_32_NBR_BITS;
    bitmap_ix++;
  }

  if (bitmap_ix < free_list_size_word) {
    uint32_t bit_position = __CLZ(__RBIT(pool_handle->block_free[bitmap_ix]));

    // Mark block number as allocated.
    SL_CLEAR_BIT(pool_handle->block_free[bitmap_ix], SL_DEF_BIT(bit_position));
    // Get final block number.
    block_ix += bit_position;
  } else {
    // No more free blocks. See Note #1.
#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
    SLI_MEMORY_PROFILER_TRACK_ALLOC(pool_handle, NULL, pool_handle->block_size);
#endif
    return SL_STATUS_EMPTY;
  }

  // Compute the block start address from the found block number.
  *block = (void *)((uint8_t *)pool_handle->reservation->block_address + (block_ix * pool_handle->block_size));

  CORE_EXIT_ATOMIC();

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  SLI_MEMORY_PROFILER_TRACK_ALLOC(pool_handle, *block, pool_handle->block_size);
#endif
#if defined(SLI_MEMORY_MANAGER_ENABLE_SYSTEMVIEW)
  uint32_t tag = (uint32_t)__builtin_extract_return_addr(__builtin_return_address(0));
  SEGGER_SYSVIEW_HeapAllocEx(pool_handle, *block, pool_handle->block_size, tag);
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Frees a block from a memory pool.
 ******************************************************************************/
sl_status_t sl_memory_pool_free(sl_memory_pool_t *pool_handle,
                                void *block)
{
  uint32_t block_ix = 0u;
  uint32_t bitmap_ix = 0u;
  uint32_t bit_position = 0u;

  if ((pool_handle == NULL) || (block == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  SLI_MEMORY_PROFILER_TRACK_FREE(pool_handle, block);
#endif
#if defined(SLI_MEMORY_MANAGER_ENABLE_SYSTEMVIEW)
  SEGGER_SYSVIEW_HeapFree(pool_handle, block);
#endif

  EFM_ASSERT(((uintptr_t)block >= (uintptr_t)pool_handle->reservation->block_address)
             && ((uintptr_t)block <= ((uintptr_t)pool_handle->reservation->block_address + pool_handle->reservation->block_size)));

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  // Compute the block number from the block start address.
  block_ix = (uint32_t)((uint8_t *)block - (uint8_t *)pool_handle->reservation->block_address) / pool_handle->block_size;

  // Mark block number as free in the free list bitmap.
  bitmap_ix = block_ix / SLI_DEF_INT_32_NBR_BITS;
  bit_position = block_ix & (SLI_DEF_INT_32_NBR_BITS - 1);
  SL_SET_BIT(pool_handle->block_free[bitmap_ix], SL_DEF_BIT(bit_position));

  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Dynamically allocates a memory pool handle.
 ******************************************************************************/
sl_status_t sl_memory_pool_handle_alloc(sl_memory_pool_t **pool_handle)
{
  sl_status_t status;

  // Allocate pool_handle as a long-term block. See Note #1.
  status = sl_memory_alloc(sizeof(sl_memory_pool_t), BLOCK_TYPE_LONG_TERM, (void**)pool_handle);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Allocate reservation handle as a long-term block.
  status = sl_memory_reservation_handle_alloc(&(*pool_handle)->reservation);
  if (status != SL_STATUS_OK) {
    // Free previous successful memory pool handle allocation.
    sl_memory_free((void*)pool_handle);
  }

  return status;
}

/***************************************************************************//**
 * Frees a dynamically allocated memory pool handle.
 ******************************************************************************/
sl_status_t sl_memory_pool_handle_free(sl_memory_pool_t *pool_handle)
{
  sl_status_t status;

  // Free reservation pool_handle.
  status = sl_memory_reservation_handle_free(pool_handle->reservation);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Free memory pool handle.
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
