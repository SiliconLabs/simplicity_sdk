/**
 * @file
 * @brief Database functions for Command Class User Code.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: 2024 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include "CC_UserCode.h"
#include "cc_user_code_io.h"
#include "cc_user_code_config.h"
#include "DebugPrint.h"
#include "assert.h"
#include <string.h>

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

ZW_WEAK uint8_t
CC_UserCode_UsersNumberReport_handler(__attribute__((unused)) uint8_t endpoint)
{
  return CC_USER_CODE_MAX_IDS;
}

ZW_WEAK bool
CC_UserCode_getId_handler(
  uint8_t identifier,
  USER_ID_STATUS* pId,
  __attribute__((unused)) uint8_t endpoint)
{
  __attribute__((unused)) bool status;
  SUserCode userCode = { 0 };

  status = CC_UserCode_Read(identifier, &userCode);
  assert(status);

  *pId = userCode.user_id_status;
  return true;
}

ZW_WEAK bool
CC_UserCode_Report_handler(
  uint8_t identifier,
  uint8_t* pUserCode,
  size_t * pLen,
  __attribute__((unused)) uint8_t endpoint)
{
  __attribute__((unused)) bool status;
  SUserCode userCode = { 0 };

  status = CC_UserCode_Read(identifier, &userCode);
  assert(status);

  *pLen = userCode.userCodeLen;
  if (USERCODE_MAX_LEN >= *pLen) {
    memcpy(pUserCode, userCode.userCode, *pLen);

    DPRINT("hCmdUC_Report = ");
    for (size_t i = 0; i < *pLen; i++) {
      DPRINTF("%d", *(pUserCode + i));
    }
    DPRINT("\r\n");
    return true;
  }
  return false;
}

ZW_WEAK e_cmd_handler_return_code_t
CC_UserCode_Set_handler(
  uint8_t identifier,
  USER_ID_STATUS id,
  uint8_t* pUserCode,
  uint8_t len,
  __attribute__((unused)) uint16_t modifierNodeId)
{
  uint8_t i;
  __attribute__((unused)) bool status;
  SUserCode userCode = { 0 };

  // Make sure identifier is valid
  if (identifier > CC_USER_CODE_MAX_IDS) {
    return E_CMD_HANDLER_RETURN_CODE_HANDLED;
  }

  // it is possible to remove all user codes at once when identifier == 0
  if (identifier == 0) {
    if (id == USER_ID_AVAILABLE) {
      userCode.user_id_status = id;
      memset(userCode.userCode, 0xFF, len);
      userCode.userCodeLen = len;

      for (i = 0; i < CC_USER_CODE_MAX_IDS; i++) {
        status = CC_UserCode_Write(i + 1, &userCode);
        assert(status);
      }
    }
  } else {
    userCode.user_id_status = id;
    memcpy(userCode.userCode, pUserCode, len);
    userCode.userCodeLen = len;

    status = CC_UserCode_Write(identifier, &userCode);
    assert(status);
  }

  for (i = 0; i < len; i++) {
    DPRINTF("%d", *(pUserCode + i));
  }
  DPRINT("\r\n");
  return E_CMD_HANDLER_RETURN_CODE_HANDLED;
}

ZW_WEAK void
CC_UserCode_reset_data(void)
{
  uint8_t defaultUserCode[] = CC_USER_CODE_DEFAULT;
  __attribute__((unused)) bool status;

  SUserCode userCodeDefaultData = {
    .user_id_status = USER_ID_OCCUPIED,
    .userCodeLen = sizeof(defaultUserCode)
  };
  memcpy(userCodeDefaultData.userCode, defaultUserCode, userCodeDefaultData.userCodeLen);

  status = CC_UserCode_Write(1, &userCodeDefaultData);
  assert(status);

  if (CC_USER_CODE_MAX_IDS > 1) {
    for (uint8_t i = 1; i < CC_USER_CODE_MAX_IDS; i++) {
      userCodeDefaultData.user_id_status = USER_ID_AVAILABLE;
      userCodeDefaultData.userCodeLen = sizeof(defaultUserCode);
      memset(userCodeDefaultData.userCode, 0xFF, userCodeDefaultData.userCodeLen);
      status = CC_UserCode_Write(i + 1, &userCodeDefaultData);
      assert(status);
    }
  }
}

ZW_WEAK bool
CC_UserCode_Validate(uint8_t identifier, const uint8_t *pCode, uint8_t len)
{
  __attribute__((unused)) bool status;
  SUserCode userCode = { 0 };

  status = CC_UserCode_Read(identifier, &userCode);
  assert(status);

  if ((len == userCode.userCodeLen)
      && ((USER_ID_OCCUPIED == userCode.user_id_status)
          || (USER_ID_RESERVED == userCode.user_id_status))) {
    if (0 == memcmp(pCode, userCode.userCode, len)) {
      return true;
    }
  }
  return false;
}
