/***************************************************************************//**
 * @file
 * @brief CS application logger.
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

#ifndef CS_LOG_H
#define CS_LOG_H

/**************************************************************************//**
 * Provide time stamp in the format YYYY-MM-DDThhmmss.
 * @return Time stamp with the current time is returned upon the first call.
 *         Subsequent calls return the same value.
 *****************************************************************************/
char *cs_log_get_timestamp(void);

/**************************************************************************//**
 * Create CS log file.
 * @param[in] argc Number of input arguments.
 * @param[in] argv Array of input arguments.
 *****************************************************************************/
void cs_log_create(int argc, char *argv[]);

#endif // CS_LOG_H
