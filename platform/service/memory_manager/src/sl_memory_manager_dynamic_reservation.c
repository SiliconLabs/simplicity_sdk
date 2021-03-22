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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdalign.h>

#include "sl_memory_manager_config.h"
#include "sl_memory_manager.h"
#include "sli_memory_manager.h"
#include "sl_assert.h"
#include "sl_bit.h"
#include "sl_common.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
#include "sli_memory_profiler.h"
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Dynamically reserves a block of memory.
 *
 * @note (1) This function assumes the 'handle' is provided by the caller:
 *           - either statically (e.g. as a global variable)
 *           - or dynamically by calling sl_memory_reservation_handle_alloc().
 ******************************************************************************/
sl_status_t sl_memory_reserve_block(size_t size,
                                    size_t align,
                                    sl_memory_reservation_t *handle,
                                    void **block)
{
  // Check proper alignment characteristics.
  EFM_ASSERT((align == SL_MEMORY_BLOCK_ALIGN_DEFAULT)
             || (SL_MATH_IS_PWR2(align)
                 && (align <= SL_MEMORY_BLOCK_ALIGN_512_BYTES)));

  (void) align;
  sli_block_metadata_t *free_block_metadata = NULL;
  void *reserved_blk = NULL;
  size_t current_block_len;
  size_t size_real;
  size_t size_adjusted;
  size_t block_size_remaining;
#if defined(DEBUG_EFM) || defined(DEBUG_EFM_USER)
  reserve_no_retention_first = false;
#endif

  // Verify that the handle pointer isn't NULL. See Note #1.
  if ((handle == NULL) || (block == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Check that the block does not exist yet.
  if ((handle->block_size != 0) || (handle->block_address != NULL)) {
    return SL_STATUS_FAIL;
  }

  *block = NULL; // No block reserved yet.

  if (size == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  size_real = SLI_ALIGN_ROUND_UP(size, SLI_BLOCK_ALLOC_MIN_ALIGN);

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  // For block reservations, the size_adjusted contains the metadata.
  size_adjusted = sli_memory_find_free_block(size_real, align, BLOCK_TYPE_SHORT_TERM, true, &free_block_metadata);

  if ((free_block_metadata == NULL) || (size_adjusted == 0)) {
    CORE_EXIT_ATOMIC();
#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
    SLI_MEMORY_PROFILER_TRACK_ALLOC(sli_mm_heap_name, NULL, size);
#endif
    return SL_STATUS_ALLOCATION_FAILED;
  }

  current_block_len = SLI_BLOCK_LEN_DWORD_TO_BYTE(free_block_metadata->length);
  // SLI_BLOCK_METADATA_SIZE_BYTE is added to the free block length to get the real remaining size as size_adjusted contains the metadata size.
  block_size_remaining = (current_block_len + SLI_BLOCK_METADATA_SIZE_BYTE) - size_adjusted;

  // Create a new block = reserved block returned to requester. This new block is the nearest to the heap end.
  reserved_blk = (sli_block_metadata_t *)((uint8_t *)free_block_metadata + block_size_remaining);

  sli_free_blocks_number--;

  // Split free and reserved blocks if possible.
  if (block_size_remaining >= SLI_BLOCK_RESERVATION_MIN_SIZE_BYTE) {
    // Changes size of free block.
    free_block_metadata->length -= SLI_BLOCK_LEN_BYTE_TO_DWORD(size_real);

    // Account for the split block that is free.
    sli_free_blocks_number++;
  } else {
    sli_block_metadata_t *neighbour_block = NULL;

    // Update next neighbour.
    if (free_block_metadata->offset_neighbour_next != 0) {
      neighbour_block = (sli_block_metadata_t *)((uint64_t *)free_block_metadata + free_block_metadata->offset_neighbour_next);

      if (free_block_metadata->offset_neighbour_prev != 0) {
        neighbour_block->offset_neighbour_prev += free_block_metadata->offset_neighbour_prev;
      } else {
        // Heap start.
        neighbour_block->offset_neighbour_next = 0;
      }
    }

    // Update previous neighbour.
    if (free_block_metadata->offset_neighbour_prev != 0) {
      neighbour_block = (sli_block_metadata_t *)((uint64_t *)free_block_metadata - free_block_metadata->offset_neighbour_prev);

      if (free_block_metadata->offset_neighbour_next != 0) {
        neighbour_block->offset_neighbour_next += free_block_metadata->offset_neighbour_next;
      } else {
        // Heap end.
        neighbour_block->offset_neighbour_next = 0;
      }
    }

    // Update head pointers accordingly.
    if (sli_free_blocks_number != 0) {
      // There is at least one other free block in the heap.
      if (sli_free_lt_list_head == free_block_metadata) {
        sli_free_lt_list_head = sli_memory_find_head_free_block(BLOCK_TYPE_LONG_TERM, NULL);
      }
      if (sli_free_st_list_head == free_block_metadata) {
        sli_free_st_list_head = sli_memory_find_head_free_block(BLOCK_TYPE_SHORT_TERM, neighbour_block);
      }
    } else {
      // No more free blocks.
      sli_free_lt_list_head = NULL;
      sli_free_st_list_head = NULL;
    }
  }

  CORE_EXIT_ATOMIC();

  handle->block_size = size;
  handle->block_address = reserved_blk;
  *block = reserved_blk;

#ifdef SLI_MEMORY_MANAGER_ENABLE_TEST_UTILITIES
  // Save the reservation for heap integrity check purposes.
  sli_memory_save_reservation_handle(handle, align);
#endif

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  SLI_MEMORY_PROFILER_TRACK_ALLOC(sli_mm_heap_name, handle->block_address, size_real);
  SLI_MEMORY_PROFILER_TRACK_ALLOC(sli_mm_heap_reservation_name, handle->block_address, handle->block_size);
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Frees a dynamically reserved block of memory.
 *
 * @note (1) Block reservation are not part of the double linked-list as they
 *           don't have metadata next to them compared to a LT/ST block that
 *           has a metadata with offset. When releasing a reserved block,
 *           the previous and next blocks that are part of the double linked-list
 *           need to be found to insert this new free block. The neighbours are
 *           found by browsing through the linked-list from the heap start until
 *           a block metadata's address is higher than the block being
 *           released's address. The previous and next blocks are then saved to
 *           properly update the double linked-list.
 ******************************************************************************/
sl_status_t sl_memory_release_block(sl_memory_reservation_t *handle)
{
  sl_memory_region_t heap_region = sl_memory_get_heap_region();

  uint16_t new_free_block_length;
  sli_block_metadata_t *new_free_block = NULL;
  sli_block_metadata_t *prev_block = NULL;
  sli_block_metadata_t *next_block = NULL;
  uint16_t reserved_block_offset;

  sli_block_metadata_t *current_metadata = (sli_block_metadata_t *)heap_region.addr;

  // Verify that the handle isn't NULL.
  if (handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

#if defined(SL_CATALOG_MEMORY_PROFILER_PRESENT)
  SLI_MEMORY_PROFILER_TRACK_FREE(sli_mm_heap_name, handle->block_address);
#endif

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  // Find neighbours by searching from the heap start. See Note #1.
  while ((uintptr_t)current_metadata < (uintptr_t)handle->block_address) {
    prev_block = current_metadata;

    if (current_metadata->offset_neighbour_next == 0) {
      break;
    }

    current_metadata = (sli_block_metadata_t *)((uint64_t *)current_metadata + current_metadata->offset_neighbour_next);
  }
  next_block = ((uintptr_t)current_metadata >= (uintptr_t)handle->block_address) ? current_metadata : NULL;

  new_free_block_length = (uint16_t)SLI_BLOCK_LEN_BYTE_TO_DWORD(handle->block_size);

  // Create a new free block while trying to merge it with the previous and next free blocks if possible.
  if (prev_block != NULL) {
    // Calculate offset between the reserved block and the previous block's payload address.
    reserved_block_offset = (uint16_t)((uint64_t *)handle->block_address - (uint64_t *)prev_block - SLI_BLOCK_METADATA_SIZE_DWORD);
    // Then calculate the difference between the above offset and the length of the previous block.
    reserved_block_offset -= prev_block->length;

    // Make sure there's no reserved block between the freed block and the previous block.
    if ((prev_block->block_in_use == 0) && (reserved_block_offset < SLI_BLOCK_RESERVATION_MIN_SIZE_DWORD)) {
      // New freed block's previous block is free, so merge both free blocks.
      new_free_block = prev_block;
      prev_block = (sli_block_metadata_t *)((uint64_t *)prev_block - prev_block->offset_neighbour_prev);
      new_free_block_length += new_free_block->length;
    } else {
      // Create a new free block, because previous block is a dynamic allocation or a reserved block.
      new_free_block = (sli_block_metadata_t *)handle->block_address;
      new_free_block_length -= SLI_BLOCK_METADATA_SIZE_DWORD;
      sli_free_blocks_number++;
    }
  }

  if (next_block != NULL) {
    // Calculate offset between the reserved block and the next block.
    reserved_block_offset = (uint16_t)((uint64_t *)next_block - (uint64_t *)handle->block_address);
    // Then calculate the difference between the above offset and the size of the block being released.
    reserved_block_offset -= SLI_BLOCK_LEN_BYTE_TO_DWORD(handle->block_size);

    // Make sure there's no reserved block between the freed block and the next block.
    if ((next_block->block_in_use == 0) && (reserved_block_offset < SLI_BLOCK_RESERVATION_MIN_SIZE_DWORD)) {
      // New freed block's following block is free, so merge both free blocks.
      new_free_block_length += next_block->length + reserved_block_offset + SLI_BLOCK_METADATA_SIZE_DWORD;
      // Invalidate the next block metadata.
      next_block->length = 0;
      // 2 free blocks have been merged, account for 1 free block only.
      sli_free_blocks_number--;

      if (next_block->offset_neighbour_next != 0) {
        // Get next block following current next block.
        next_block = (sli_block_metadata_t *)((uint64_t *)next_block + next_block->offset_neighbour_next);
      } else {
        next_block = NULL;
      }
    }
  }

  // Update the new free metadata block accordingly.
  sli_memory_metadata_init(new_free_block);
  new_free_block->length = new_free_block_length;

  if (next_block != NULL) {
    new_free_block->offset_neighbour_next = (uint16_t)((uint64_t *)next_block - (uint64_t *)new_free_block);
    if ((uintptr_t)new_free_block == (uintptr_t)handle->block_address) {
      // Case where reserved block not merged with previous neighbor:
      // Update next block's previous neighbor offset accordingly.
      next_block->offset_neighbour_prev = new_free_block->offset_neighbour_next;
    }
    // If the reserved block was merged with the previous neighbor, no need to update next block's
    // previous neighbor offset as this one already points to the correct previous neighbor.
  } else {
    // Heap end.
    new_free_block->offset_neighbour_next = 0;
  }

  if (prev_block != NULL) {
    new_free_block->offset_neighbour_prev = (uint16_t)((uint64_t *)new_free_block - (uint64_t *)prev_block);
    if ((uintptr_t)new_free_block == (uintptr_t)handle->block_address) {
      // Case where reserved block not merged with previous neighbor:
      // Update previous block's next neighbor offset accordingly.
      prev_block->offset_neighbour_next = new_free_block->offset_neighbour_prev;
    }
    // If the reserved block was merged with the previous neighbor, no need to update previous block's
    // next neighbor offset as this one already points to the correct next neighbor.
  } else {
    // Heap start.
    new_free_block->offset_neighbour_prev = 0;
  }

  if (sli_free_lt_list_head == NULL             // LT list is empty. Freed block becomes the new 1st element.
      || sli_free_lt_list_head > new_free_block // LT list not empty. Verify if freed block becomes the head.
      || sli_free_lt_list_head->length == 0) {
    sli_free_lt_list_head = new_free_block;
  }

  if (sli_free_st_list_head == NULL             // ST list is empty. Freed block becomes the new 1st element.
      || sli_free_st_list_head < new_free_block // ST list not empty. Verify if freed block becomes the head.
      || sli_free_st_list_head->length == 0) {
    sli_free_st_list_head = new_free_block;
  }

  // Invalidate handle.
  handle->block_address = NULL;
  handle->block_size = 0;

  CORE_EXIT_ATOMIC();

#ifdef SLI_MEMORY_MANAGER_ENABLE_TEST_UTILITIES
  sli_memory_remove_reservation_handle(handle);
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Dynamically allocates a block reservation handle.
 ******************************************************************************/
sl_status_t sl_memory_reservation_handle_alloc(sl_memory_reservation_t **handle)
{
  sl_status_t status;

  status = sl_memory_alloc(sizeof(sl_memory_reservation_t), BLOCK_TYPE_LONG_TERM, (void**)handle);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Initialize handle data.
  (*handle)->block_address = NULL;
  (*handle)->block_size = 0;

  return status;
}

/***************************************************************************//**
 * Frees a dynamically allocated block reservation handle.
 ******************************************************************************/
sl_status_t sl_memory_reservation_handle_free(sl_memory_reservation_t *handle)
{
  // Check that block has been released before freeing handle.
  if ((handle->block_size != 0) || (handle->block_address != NULL)) {
    return SL_STATUS_FAIL;
  }

  return sl_memory_free((void *)handle);
}

/***************************************************************************//**
 * Gets the size of the memory reservation handle structure.
 ******************************************************************************/
uint32_t sl_memory_reservation_handle_get_size(void)
{
  return sizeof(sl_memory_reservation_t);
}
