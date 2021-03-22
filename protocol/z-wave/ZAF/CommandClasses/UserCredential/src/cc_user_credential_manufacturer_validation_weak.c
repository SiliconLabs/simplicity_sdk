/**
 * @file
 * @brief Weak implementation for Command Class User Credential manufacturer
 * Credential validation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: 2024 Silicon Laboratories Inc.
 *
 * @copyright 2024 Silicon Laboratories Inc.
 */

#include "CC_UserCredential.h"

ZW_WEAK bool CC_UserCredential_manufacturer_validate_credential(
  __attribute__((unused)) u3c_credential * credential)
{
  return true;
}
