/***************************************************************************//**
 * @file
 * @brief PA Tables configuration file.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_RAIL_UTIL_PA_TABLES_CONFIG_H
#define SL_RAIL_UTIL_PA_TABLES_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> PA Table Configuration
#if defined(_SILICON_LABS_EFR32_2G4HZ_HP_PA_PRESENT)
// <o SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ> 2.4 GHz PA Table Selection
// <SL_RAIL_PA_TABLE_SELECTION_DISABLE=> Disable
// <SL_RAIL_PA_TABLE_SELECTION_HP=> High Power PA
// <SL_RAIL_PA_TABLE_SELECTION_MP=> Medium Power PA
// <SL_RAIL_PA_TABLE_SELECTION_LP=> Low Power PA
// <SL_RAIL_PA_TABLE_SELECTION_AUTO_HP=> Automode with HP PA
// <SL_RAIL_PA_TABLE_SELECTION_AUTO_MP=> Automode with MP PA
// <i> Default: SL_RAIL_PA_TABLE_SELECTION_AUTO_HP
#define SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ    SL_RAIL_PA_TABLE_SELECTION_AUTO_HP
#else
// <o SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ> 2.4 GHz PA Table Selection
// <SL_RAIL_PA_TABLE_SELECTION_DISABLE=> Disable
// <SL_RAIL_PA_TABLE_SELECTION_MP=> Medium Power PA
// <SL_RAIL_PA_TABLE_SELECTION_LP=> Low Power PA
// <SL_RAIL_PA_TABLE_SELECTION_AUTO_MP=> Automode with MP PA
// <i> Default: SL_RAIL_PA_TABLE_SELECTION_AUTO_MP
#define SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ    SL_RAIL_PA_TABLE_SELECTION_AUTO_MP
#endif
// <o SL_RAIL_UTIL_PA_TABLE_SELECTION_SUB_GHZ> Sub-1 GHz PA Table Selection
// <SL_RAIL_PA_TABLE_SELECTION_DISABLE=> Disable
// <i> Default: SL_RAIL_PA_TABLE_SELECTION_DISABLE
#define SL_RAIL_UTIL_PA_TABLE_SELECTION_SUB_GHZ    SL_RAIL_PA_TABLE_SELECTION_DISABLE
// </h>

// <<< end of configuration section >>>

#if (SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ == SL_RAIL_PA_TABLE_SELECTION_HP)
#include "sl_rail_util_pa_dbm_powersetting_mapping_table_20dbm.h"
#elif (SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ == SL_RAIL_PA_TABLE_SELECTION_MP)
#include "sl_rail_util_pa_dbm_powersetting_mapping_table_10dbm.h"
#elif (SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ == SL_RAIL_PA_TABLE_SELECTION_LP)
#include "sl_rail_util_pa_dbm_powersetting_mapping_table_0dbm.h"
#elif (SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ == SL_RAIL_PA_TABLE_SELECTION_AUTO_HP)
#include "sl_rail_util_pa_dbm_powersetting_mapping_table_automode_0_10_20dbm.h"
#elif (SL_RAIL_UTIL_PA_TABLE_SELECTION_2P4_GHZ == SL_RAIL_PA_TABLE_SELECTION_AUTO_MP)
#include "sl_rail_util_pa_dbm_powersetting_mapping_table_automode_0_10dbm.h"
#endif

#endif // SL_RAIL_UTIL_PA_TABLES_CONFIG_H
