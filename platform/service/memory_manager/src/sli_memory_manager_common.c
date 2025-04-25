/***************************************************************************//**
 * @file
 * @brief Memory Manager Driver Implementation.
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

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

// Minimum block alignment in bytes. 8 bytes is the minimum alignment to account for largest CPU data type
// that can be used in some block allocation scenarios. 64-bit data type may be used to manipulate the
// allocated block. The ARM processor ABI defines data types and byte alignment, and 8-byte alignment
// can be seen for the largest data object type.
#define SLI_BLOCK_ALLOC_MIN_ALIGN   SL_MEMORY_BLOCK_ALIGN_8_BYTES

// Minimum block allocation size to avoid creating a block too small while splitting up an allocated block.
// Minimum size is formed from (metadata + payload) size. Size expressed in bytes.
#define SLI_BLOCK_ALLOCATION_MIN_SIZE   (SLI_BLOCK_METADATA_SIZE_BYTE + SL_MEMORY_MANAGER_BLOCK_ALLOCATION_MIN_SIZE)

// 64-bit word size (in octets).
#define SLI_WORD_SIZE_64                8u

// Size of metadata area in different units.
#define SLI_BLOCK_METADATA_SIZE_BYTE    sizeof(sli_block_metadata_t)
#define SLI_BLOCK_METADATA_SIZE_DWORD   SLI_BLOCK_LEN_BYTE_TO_DWORD(SLI_BLOCK_METADATA_SIZE_BYTE)

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

sli_block_metadata_t *sli_free_lt_list_head;
sli_block_metadata_t *sli_free_st_list_head;
uint32_t sli_free_blocks_number;

#ifdef SLI_MEMORY_MANAGER_ENABLE_TEST_UTILITIES
// Dynamic reservation bookkeeping.
sl_memory_reservation_t *sli_reservation_handle_ptr_table[SLI_MAX_RESERVATION_COUNT] = { NULL };
uint32_t sli_reservation_alignment_table[SLI_MAX_RESERVATION_COUNT] = { 0 };

// Reservation no retention bookkeeping.
// Array of structs instead of pointers to avoid dynamic allocation for handle.
sl_memory_reservation_t sli_reservation_no_retention_table[SLI_MAX_RESERVATION_COUNT] = { 0 };
uint32_t sli_reservation_no_retention_alignment_table[SLI_MAX_RESERVATION_COUNT] = { 0 };
#endif

/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

#ifdef SLI_MEMORY_MANAGER_ENABLE_TEST_UTILITIES
/***************************************************************************//**
 * Gets the index in sli_reservation_handle_ptr_table[] by block address.
 *
 * @param[in]  addr  Pointer to block reservation.
 *
 * @return    Corresponding index in sli_reservation_handle_ptr_table.
 ******************************************************************************/
static uint32_t get_reservation_ix_by_addr(void *addr)
{
  for (uint32_t ix = 0; ix < SLI_MAX_RESERVATION_COUNT; ix++) {
    if (sli_reservation_handle_ptr_table[ix] == NULL) {
      continue;
    }
    if (sli_reservation_handle_ptr_table[ix]->block_address == addr) {
      return ix;
    }
  }
  return -1;
}

/***************************************************************************//**
 * Gets the index in sli_reservation_handle_ptr_table[]
 * by reservation handle pointer.
 *
 * @param[in]  reservation_handle_ptr  Pointer to reservation handle.
 *
 * @return    Corresponding index in sli_reservation_handle_ptr_table.
 ******************************************************************************/
static uint32_t get_reservation_ix_by_handle(sl_memory_reservation_t *reservation_handle_ptr)
{
  for (uint32_t ix = 0; ix < SLI_MAX_RESERVATION_COUNT; ix++) {
    if (sli_reservation_handle_ptr_table[ix] == NULL) {
      continue;
    }
    if (sli_reservation_handle_ptr_table[ix] == reservation_handle_ptr) {
      return ix;
    }
  }
  return -1;
}

/***************************************************************************//**
 * Get an index of sli_reservation_handle_ptr_table that is free.
 *
 * @return    Index of an empty entry in sli_reservation_handle_ptr_table.
 ******************************************************************************/
static uint32_t get_available_reservation_handle_ix(void)
{
  for (uint32_t ix = 0; ix < SLI_MAX_RESERVATION_COUNT; ix++) {
    if (sli_reservation_handle_ptr_table[ix] == NULL) {
      return ix;
    }
  }
  return -1;
}

/***************************************************************************//**
 * Gets the index in sli_reservation_no_retention_table[] by block address.
 *
 * @param[in]  addr  Pointer to block reservation.
 *
 * @return    Corresponding index in sli_reservation_no_retention_table[].
 ******************************************************************************/
static uint32_t get_reservation_no_retention_ix(void *addr)
{
  for (uint32_t ix = 0; ix < SLI_MAX_RESERVATION_COUNT; ix++) {
    if (sli_reservation_no_retention_table[ix].block_address == NULL) {
      continue;
    }
    if (sli_reservation_no_retention_table[ix].block_address == addr) {
      return ix;
    }
  }
  return -1;
}

/***************************************************************************//**
 * Gets an index of sli_reservation_no_retention_table[] that is free.
 *
 * @return    Index of an empty entry in sli_reservation_no_retention_table[].
 ******************************************************************************/
static uint32_t get_available_reservation_no_retention_ix(void)
{
  for (uint32_t ix = 0; ix < SLI_MAX_RESERVATION_COUNT; ix++) {
    if (sli_reservation_no_retention_table[ix].block_address == NULL) {
      return ix;
    }
  }
  return -1;
}
#endif

/***************************************************************************//**
 * Initializes a memory block metadata to some reset values.
 ******************************************************************************/
void sli_memory_metadata_init(sli_block_metadata_t *block_metadata)
{
  block_metadata->block_in_use = 0;
  block_metadata->heap_start_align = 0;
  block_metadata->reserved = 0;
  block_metadata->length = 0;
  block_metadata->offset_neighbour_prev = 0;
  block_metadata->offset_neighbour_next = 0;
}

/***************************************************************************//**
 * Gets pointer pointing to the first free block of adequate size.
 *
 * @note (1) For a block reservation, there's no metadata next to the
 *           reserved block. For this reason, when looking for a free block
 *           large enough to fit a new reserved block, the size of the metadata
 *           is counted in the available size of the free blocks.
 *
 * @note (2) For a short-term block, if the required data alignment is greater
 *           than 8 bytes, the found block size must account for the correct
 *           alignment of the block data payload. A series of computations is
 *           done starting from the end of the found block to determine the
 *           best data offset needed to align the data payload. The worst
 *           alignment (size_real + block_align) cannot be taken by default
 *           as it may imply loosing too many bytes in internal fragmentation
 *           due to the alignment requirement.
 ******************************************************************************/
size_t sli_memory_find_free_block(size_t size,
                                  size_t align,
                                  sl_memory_block_type_t type,
                                  bool block_reservation,
                                  sli_block_metadata_t **block)
{
  sli_block_metadata_t *current_block_metadata = NULL;
  void *data_payload = NULL;
  size_t size_adjusted = 0;
  size_t current_block_len;
  size_t block_align = (align == SL_MEMORY_BLOCK_ALIGN_DEFAULT) ? SLI_BLOCK_ALLOC_MIN_ALIGN : align;
  size_t data_payload_offset;
  bool is_aligned = false;

  *block = NULL;

  current_block_metadata = (type == BLOCK_TYPE_LONG_TERM) ? sli_free_lt_list_head : sli_free_st_list_head;
  if (current_block_metadata == NULL) {
    return 0;
  }

  current_block_len = SLI_BLOCK_LEN_DWORD_TO_BYTE(current_block_metadata->length);

  // For a block reservation, add the metadata's size to the free blocks' available memory space. See Note #2.
  current_block_len += block_reservation ? SLI_BLOCK_METADATA_SIZE_BYTE : 0;

  // Try to find a block to allocate (first-fit).
  while (current_block_metadata != NULL) {
    if ((!current_block_metadata->block_in_use) && (current_block_len >= size)) {
      if (type == BLOCK_TYPE_LONG_TERM) {
        // Check alignment requested and ensure size of found block can accommodate worst case alignment.
        // For LT, alignment requirement can be verified here whether the block is split or not.
        data_payload = (void *)((uint8_t *)current_block_metadata + SLI_BLOCK_METADATA_SIZE_BYTE);
        is_aligned = SLI_ADDR_IS_ALIGNED(data_payload, block_align);
        data_payload_offset = (uintptr_t)data_payload % block_align;

        if (is_aligned || (current_block_len >= (size + data_payload_offset))) {
          // Compute remaining block size given an alignment handling or not.
          size_adjusted = is_aligned ? size : (size + data_payload_offset);
          break;
        }
      } else {
        if (block_align == SLI_BLOCK_ALLOC_MIN_ALIGN) {
          // If alignment is 8 bytes (default min alignment), take the requested adjusted size.
          size_adjusted = size;
        } else {
          // If non 8-byte alignment, search the more optimized size accounting for the required alignment. See Note #3.
          uint8_t *block_end = (uint8_t *)((uint64_t *)current_block_metadata + SLI_BLOCK_METADATA_SIZE_DWORD + current_block_metadata->length);

          data_payload = (void *)(block_end - size);
          data_payload = (void *)SLI_ALIGN_ROUND_DOWN(((uintptr_t)data_payload), block_align);
          size_adjusted = (size_t)(block_end - (uint8_t *)data_payload);
        }

        if (current_block_len >= size_adjusted) {
          break;
        }
      }
    }

    // Get next block.
    if (type == BLOCK_TYPE_LONG_TERM) {
      if (current_block_metadata->offset_neighbour_next == 0) {
        return 0;  // End of heap. No block found.
      }
      // Long-term browsing direction goes from start to end of heap.
      current_block_metadata = (sli_block_metadata_t *)((uint64_t *)current_block_metadata + (current_block_metadata->offset_neighbour_next));
    } else {
      if (current_block_metadata->offset_neighbour_prev == 0) {
        return 0;  // Start of heap. No block found.
      }
      // Short-term browsing direction goes from end to start of heap.
      current_block_metadata = (sli_block_metadata_t *)((uint64_t *)current_block_metadata - (current_block_metadata->offset_neighbour_prev));
    }

    current_block_len = SLI_BLOCK_LEN_DWORD_TO_BYTE(current_block_metadata->length);
    current_block_len += block_reservation ? SLI_BLOCK_METADATA_SIZE_BYTE : 0;
  }

  *block = current_block_metadata;
  return size_adjusted;
}

/***************************************************************************//**
 * Finds the next free block that will become the long-term or short-term head
 * pointer.
 ******************************************************************************/
sli_block_metadata_t *sli_memory_find_head_free_block(sl_memory_block_type_t type,
                                                      sli_block_metadata_t *block_start_from)
{
  sli_block_metadata_t *current_block_metadata = NULL;
  sli_block_metadata_t *free_block_metadata = NULL;
  bool search = true;

  if (sli_free_blocks_number == 0) {
    // No more free blocks.
    return NULL;
  }

  if (block_start_from != NULL) {
    // Start searching from the given block.
    current_block_metadata = block_start_from;
  } else {
    // Start searching from heap start (long-term [LT]) or near heap end (short-term [ST]).
    // For ST, searching cannot start at the absolute heap end. So the ST head pointer is used as it points
    // to the last free block closest to the heap end.
    sl_memory_region_t heap_region = sl_memory_get_heap_region();

    current_block_metadata = (type == BLOCK_TYPE_LONG_TERM) ? (sli_block_metadata_t *)heap_region.addr : sli_free_st_list_head;
  }

  // Long-term block: find the first free block closest to the heap start.
  // Short-term block: find the first free block closest to the heap end.
  do {
    if (current_block_metadata->block_in_use == 0) {
      free_block_metadata = current_block_metadata;
      search = false;
    } else if ((type == BLOCK_TYPE_LONG_TERM) && (current_block_metadata->offset_neighbour_next != 0)) {
      current_block_metadata = (sli_block_metadata_t *)((uint64_t *)current_block_metadata + (current_block_metadata->offset_neighbour_next));
    } else if ((type == BLOCK_TYPE_SHORT_TERM) && (current_block_metadata->offset_neighbour_prev != 0)) {
      current_block_metadata = (sli_block_metadata_t *)((uint64_t *)current_block_metadata - (current_block_metadata->offset_neighbour_prev));
    } else {
      free_block_metadata = NULL;
      break;
    }
  } while (search);

  return free_block_metadata;
}

/***************************************************************************//**
 * Gets long-term head pointer pointing to the first free block.
 ******************************************************************************/
void *sli_memory_get_longterm_head_ptr(void)
{
  return (void *)sli_free_lt_list_head;
}

/***************************************************************************//**
 * Gets short-term head pointer pointing to the first free block.
 ******************************************************************************/
void *sli_memory_get_shortterm_head_ptr(void)
{
  return (void *)sli_free_st_list_head;
}

/***************************************************************************//**
 * Update free lists heads (short and long terms).
 ******************************************************************************/
void sli_update_free_list_heads(sli_block_metadata_t *free_head,
                                const sli_block_metadata_t *condition_block,
                                bool search)
{
  if (search) {
    if ((sli_free_lt_list_head == condition_block) || (condition_block == NULL)) {
      sli_free_lt_list_head = sli_memory_find_head_free_block(BLOCK_TYPE_LONG_TERM, free_head);
    }
    if ((sli_free_st_list_head == condition_block) || (condition_block == NULL)) {
      sli_free_st_list_head = sli_memory_find_head_free_block(BLOCK_TYPE_SHORT_TERM, free_head);
    }
  } else {
    if (sli_free_lt_list_head == condition_block) {
      sli_free_lt_list_head = free_head;
    } else if (free_head < sli_free_lt_list_head) {
      sli_free_lt_list_head = free_head;
    }
    if (sli_free_st_list_head == condition_block) {
      sli_free_st_list_head = free_head;
    } else if (free_head > sli_free_st_list_head) {
      sli_free_st_list_head = free_head;
    }
  }
}

#ifdef SLI_MEMORY_MANAGER_ENABLE_TEST_UTILITIES
/***************************************************************************//**
 * Gets the pointer to sl_memory_reservation_t{} by block address.
 ******************************************************************************/
sl_memory_reservation_t *sli_memory_get_reservation_handle_by_addr(void *addr)
{
  uint32_t reservation_ix;
  reservation_ix = get_reservation_ix_by_addr(addr);

  if (reservation_ix != (uint32_t)-1) {
    return sli_reservation_handle_ptr_table[reservation_ix];
  }

  return NULL;
}

/***************************************************************************//**
 * Gets the size of a reservation by block address.
 ******************************************************************************/
uint32_t sli_memory_get_reservation_size_by_addr(void *addr)
{
  sl_memory_reservation_t * reservation_handle_ptr;
  reservation_handle_ptr = sli_memory_get_reservation_handle_by_addr(addr);

  if (reservation_handle_ptr != NULL) {
    return reservation_handle_ptr->block_size;
  }
  // Not a reservation, return 0 size.
  return 0;
}

/***************************************************************************//**
 * Gets the alignment of a reservation by block address.
 ******************************************************************************/
uint32_t sli_memory_get_reservation_align_by_addr(void *addr)
{
  uint32_t reservation_ix = -1;
  reservation_ix = get_reservation_ix_by_addr(addr);

  if (reservation_ix != (uint32_t)-1) {
    return sli_reservation_alignment_table[reservation_ix];
  }

  return 0;
}

/***************************************************************************//**
 * Bookkeeps a reservation for profiling purposes.
 ******************************************************************************/
sl_status_t sli_memory_save_reservation_handle(sl_memory_reservation_t *reservation_handle_ptr,
                                               uint32_t align)
{
  uint32_t reservation_ix = -1;
  reservation_ix = get_available_reservation_handle_ix();

  if (reservation_ix != (uint32_t)-1) {
    sli_reservation_handle_ptr_table[reservation_ix] = reservation_handle_ptr;
    sli_reservation_alignment_table[reservation_ix] = align;
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FULL;
  }
}

/***************************************************************************//**
 * Removes a reservation from records.
 ******************************************************************************/
sl_status_t sli_memory_remove_reservation_handle(sl_memory_reservation_t *reservation_handle_ptr)
{
  uint32_t reservation_ix = -1;
  reservation_ix = get_reservation_ix_by_handle(reservation_handle_ptr);

  if (reservation_ix != (uint32_t)-1) {
    sli_reservation_handle_ptr_table[reservation_ix] = NULL;
    sli_reservation_alignment_table[reservation_ix] = 0;
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_NOT_FOUND;
  }
}

/***************************************************************************//**
 * Bookkeeps a reservation (no retention) for profiling purposes.
 ******************************************************************************/
sl_status_t sli_memory_save_reservation_no_retention(void * block_address, uint32_t block_size, uint32_t align)
{
  uint32_t reservation_ix = -1;
  reservation_ix = get_available_reservation_no_retention_ix();

  if (reservation_ix != (uint32_t)-1) {
    sli_reservation_no_retention_table[reservation_ix].block_address = block_address;
    sli_reservation_no_retention_table[reservation_ix].block_size = block_size;
    sli_reservation_no_retention_alignment_table[reservation_ix] = align;

    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FULL;
  }
}

/***************************************************************************//**
 * Gets the size of a reservation (no retention) by block address.
 ******************************************************************************/
uint32_t sli_memory_get_reservation_no_retention_size(void * addr)
{
  uint32_t reservation_ix = -1;
  reservation_ix = get_reservation_no_retention_ix(addr);

  if (reservation_ix != (uint32_t)-1) {
    return sli_reservation_no_retention_table[reservation_ix].block_size;
  }
  // Not a reservation (no retention), return 0 size.
  return 0;
}

/***************************************************************************//**
 * Gets the alignment of a reservation (no retention) by block address.
 ******************************************************************************/
uint32_t sli_memory_get_reservation_no_retention_align(void * addr)
{
  uint32_t reservation_ix = -1;
  reservation_ix = get_reservation_no_retention_ix(addr);

  if (reservation_ix != (uint32_t)-1) {
    return sli_reservation_no_retention_alignment_table[reservation_ix];
  }

  return 0;
}

/***************************************************************************//**
 * Does a heap integrity check forwards from sli_free_lt_list_head and return
 * the pointer to the corrupted sli_block_metadata_t{} (if applicable).
 * This could go past reservations so there are checks.
 ******************************************************************************/
sli_block_metadata_t *sli_memory_check_heap_integrity_forwards(void)
{
  uint64_t * heap_end_by_metadata = 0;
  uint32_t is_corrupted = 0;
  uint32_t reservation_size;
  uint32_t reservation_size_real;
  uint32_t alignment;
  sli_block_metadata_t* current = sli_free_lt_list_head;
  sl_memory_region_t heap_region;
  heap_region = sl_memory_get_heap_region();

  while (current != NULL) {
    // Reached last block in heap.
    if (current->offset_neighbour_next == 0) {
      heap_end_by_metadata = ((uint64_t *)current + (current->length + SLI_BLOCK_METADATA_SIZE_DWORD));

      // Check if reservation (one or more).
      alignment  = sli_memory_get_reservation_align_by_addr((void *)heap_end_by_metadata);
      reservation_size = sli_memory_get_reservation_size_by_addr((void *)heap_end_by_metadata);
      if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
        reservation_size_real = reservation_size;
      } else {
        reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
      }

      while (reservation_size != 0) {
        heap_end_by_metadata = (uint64_t *)((uint8_t*)heap_end_by_metadata + reservation_size_real);
        alignment  = sli_memory_get_reservation_align_by_addr((void *)heap_end_by_metadata);
        reservation_size = sli_memory_get_reservation_size_by_addr((void *)heap_end_by_metadata);
        if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
          reservation_size_real = reservation_size;
        } else {
          reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
        }
      }

      // Check if reservation no retention (one or more).
      // Only needed for forwards.
      alignment  = sli_memory_get_reservation_no_retention_align((void *)heap_end_by_metadata);
      reservation_size = sli_memory_get_reservation_no_retention_size((void *)heap_end_by_metadata);
      if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
        reservation_size_real = reservation_size;
      } else {
        reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
      }

      while (reservation_size != 0) {
        heap_end_by_metadata = (uint64_t *)((uint8_t*)heap_end_by_metadata + reservation_size_real);
        alignment  = sli_memory_get_reservation_no_retention_align((void *)heap_end_by_metadata);
        reservation_size = sli_memory_get_reservation_no_retention_size((void *)heap_end_by_metadata);
        if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
          reservation_size_real = reservation_size;
        } else {
          reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
        }
      }

      if (heap_end_by_metadata != (void *)((uintptr_t)heap_region.addr + heap_region.size)) {
        is_corrupted = 1;
      }
      break;
    }

    // Calculate the address of the next block using offset and length.
    sli_block_metadata_t *next_blk_by_offset = (sli_block_metadata_t *)((uint64_t *)current + (current->offset_neighbour_next));
    sli_block_metadata_t *next_blk_by_len = (sli_block_metadata_t *)((uint64_t *)current + (current->length + SLI_BLOCK_METADATA_SIZE_DWORD));

    // Check if reservation (one or more).
    alignment  = sli_memory_get_reservation_align_by_addr((void *)next_blk_by_len);
    reservation_size = sli_memory_get_reservation_size_by_addr((void *)next_blk_by_len);
    if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
      reservation_size_real = reservation_size;
    } else {
      reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
    }

    while (reservation_size != 0) {
      next_blk_by_len = (sli_block_metadata_t *)((uint8_t*)next_blk_by_len + reservation_size_real);
      alignment  = sli_memory_get_reservation_align_by_addr((void *)next_blk_by_len);
      reservation_size = sli_memory_get_reservation_size_by_addr((void *)next_blk_by_len);
      if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
        reservation_size_real = reservation_size;
      } else {
        reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
      }
    }

    // Check if reservation no retention (one or more).
    // Only needed for forwards.
    alignment  = sli_memory_get_reservation_no_retention_align((void *)next_blk_by_len);
    reservation_size = sli_memory_get_reservation_no_retention_size((void *)next_blk_by_len);
    if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
      reservation_size_real = reservation_size;
    } else {
      reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
    }

    while (reservation_size != 0) {
      next_blk_by_len = (sli_block_metadata_t *)((uint8_t*)next_blk_by_len + reservation_size_real);
      alignment  = sli_memory_get_reservation_no_retention_align((void *)next_blk_by_len);
      reservation_size = sli_memory_get_reservation_no_retention_size((void *)next_blk_by_len);
      if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
        reservation_size_real = reservation_size;
      } else {
        reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
      }
    }

    if (next_blk_by_offset != next_blk_by_len) {
      is_corrupted = 1;
      break;
    } else {
      current = next_blk_by_offset;
    }
  }

  if (is_corrupted) {
    return (sli_block_metadata_t *)current;
  }

  return NULL;
}

/***************************************************************************//**
 * Does a heap integrity check backwards from sli_free_st_list_head and return
 * the pointer to the corrupted sli_block_metadata_t{} (if applicable).
 * This should not go past any reservations, hence there are no checks.
 ******************************************************************************/
sli_block_metadata_t *sli_memory_check_heap_integrity_backwards(void)
{
  uint64_t * heap_base_by_metadata = 0;
  uint32_t is_corrupted = 0;
  uint32_t reservation_size;
  uint32_t reservation_size_real;
  uint32_t alignment;
  sli_block_metadata_t* current = sli_free_st_list_head;
  sl_memory_region_t heap_region;
  heap_region = sl_memory_get_heap_region();

  while (current != NULL) {
    // Reached first block in heap.
    if (current->offset_neighbour_prev == 0) {
      heap_base_by_metadata = ((uint64_t *)current);
      if (heap_base_by_metadata != (void *)heap_region.addr) {
        is_corrupted = 1;
      }
      break;
    }

    // Calculate the address of the current block using offset and length of the previous block.
    sli_block_metadata_t *prev_blk_by_offset = (sli_block_metadata_t *)((uint64_t *)current - (current->offset_neighbour_prev));
    sli_block_metadata_t *current_by_prev_offset = (sli_block_metadata_t *)((uint64_t *)prev_blk_by_offset + (prev_blk_by_offset->offset_neighbour_next));
    sli_block_metadata_t *current_by_prev_len = (sli_block_metadata_t *)((uint64_t *)prev_blk_by_offset + (prev_blk_by_offset->length + SLI_BLOCK_METADATA_SIZE_DWORD));

    // Check if reservation (one or more).
    // This is required when sli_free_st_list_head has reservations before it.
    alignment  = sli_memory_get_reservation_align_by_addr((void *)current_by_prev_len);
    reservation_size = sli_memory_get_reservation_size_by_addr((void *)current_by_prev_len);
    if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
      reservation_size_real = reservation_size;
    } else {
      reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
    }

    while (reservation_size != 0) {
      current_by_prev_len = (sli_block_metadata_t *)((uint8_t*)current_by_prev_len + reservation_size_real);
      alignment  = sli_memory_get_reservation_align_by_addr((void *)current_by_prev_len);
      reservation_size = sli_memory_get_reservation_size_by_addr((void *)current_by_prev_len);
      if (alignment == SL_MEMORY_BLOCK_ALIGN_DEFAULT) {
        reservation_size_real = reservation_size;
      } else {
        reservation_size_real = SLI_ALIGN_ROUND_UP(reservation_size, alignment);
      }
    }

    if (current_by_prev_len != current_by_prev_offset) {
      is_corrupted = 1;
      break;
    } else {
      current = prev_blk_by_offset;
    }
  }

  if (is_corrupted) {
    return (sli_block_metadata_t *)current;
  }

  return NULL;
}
#endif /* SLI_MEMORY_MANAGER_ENABLE_TEST_UTILITIES */
