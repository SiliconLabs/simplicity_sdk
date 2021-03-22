/**************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M Peripheral Access Layer for Silicon Laboratories
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * @verbatim
 * Example: Add "-DEFM32G890F128" to your build options, to define part
 *          Add "#include "em_device.h" to your source files

 *
 * @endverbatim
 ******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
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
 *****************************************************************************/

#ifndef EM_DEVICE_H
#define EM_DEVICE_H
#if defined(MGM210L022JIF)
#include "mgm210l022jif.h"

#elif defined(MGM210L022JNF)
#include "mgm210l022jnf.h"

#elif defined(MGM210LA22JIF)
#include "mgm210la22jif.h"

#elif defined(MGM210LA22JNF)
#include "mgm210la22jnf.h"

#elif defined(MGM210P022JIA)
#include "mgm210p022jia.h"

#elif defined(MGM210P032JIA)
#include "mgm210p032jia.h"

#elif defined(MGM210PA22JIA)
#include "mgm210pa22jia.h"

#elif defined(MGM210PA32JIA)
#include "mgm210pa32jia.h"

#elif defined(MGM210PB22JIA)
#include "mgm210pb22jia.h"

#elif defined(MGM210PB32JIA)
#include "mgm210pb32jia.h"

#elif defined(MGM211LA02JNF)
#include "mgm211la02jnf.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif

#if defined(SL_CATALOG_TRUSTZONE_SECURE_CONFIG_PRESENT) && defined(SL_TRUSTZONE_NONSECURE)
#error "Can't define SL_CATALOG_TRUSTZONE_SECURE_CONFIG_PRESENT and SL_TRUSTZONE_NONSECURE MACRO at the same time."
#endif

#if defined(SL_TRUSTZONE_SECURE) && defined(SL_TRUSTZONE_NONSECURE)
#error "Can't define SL_TRUSTZONE_SECURE and SL_TRUSTZONE_NONSECURE MACRO at the same time."
#endif
#endif /* EM_DEVICE_H */
