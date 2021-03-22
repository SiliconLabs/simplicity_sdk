/**
 * @file
 *
 * ZAF PrintAppInfo module source file
 * @copyright 2023 Silicon Laboratories Inc.
 */

#include "DebugPrint.h"
#include "zpal_misc.h"
#include "ZAF_PrintAppInfo.h"
#include "ZAF_AppName.h"
#include "ZAF_version.h"

void ZAF_PrintAppInfo(void)
{
  char app_name[ZAF_FILE_SIZE_APP_NAME];
  ZAF_AppName_Read(app_name);
  DebugPrint("\n\n----------------------------------\n");
  DebugPrintf("Z-Wave Sample App: %s \n", app_name);
  DebugPrintf("SDK: %d.%d.%d ZAF: %d.%d.%d.%d\n",
              SDK_GetVersionMajor(),
              SDK_GetVersionMinor(),
              SDK_GetVersionPatch(),
              zpal_get_app_version_major(),
              zpal_get_app_version_minor(),
              zpal_get_app_version_patch(),
              ZAF_GetBuildNumber());
}
