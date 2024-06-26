/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_device.h"
#ifndef _SILICON_LABS_32B_SERIES_2
#include "sl_hal_ldma.h"
#else
#include "em_ldma.h"
#endif

#include "dmadrv.h"
#include "rail.h"
#include "sl_rail_util_dma.h"

void sl_rail_util_dma_init(void)
{
#if SL_RAIL_UTIL_DMA_ENABLE
#if SL_RAIL_UTIL_DMA_DMADRV_ENABLE
  Ecode_t dmaError = DMADRV_Init();
  if ((dmaError == ECODE_EMDRV_DMADRV_ALREADY_INITIALIZED)
      || (dmaError == ECODE_EMDRV_DMADRV_OK)) {
    unsigned int channel;
    dmaError = DMADRV_AllocateChannel(&channel, NULL);
    if (dmaError == ECODE_EMDRV_DMADRV_OK) {
      (void) RAIL_UseDma(channel);
    }
  }
#else // !SL_RAIL_UTIL_DMA_DMADRV_ENABLE
  LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;
  LDMA_Init(&ldmaInit);
  (void) RAIL_UseDma(SL_RAIL_UTIL_DMA_CHANNEL);
#endif // SL_RAIL_UTIL_DMA_DMADRV_ENABLE
#endif // SL_RAIL_UTIL_DMA_ENABLE
}
