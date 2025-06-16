#ifndef _ZPAL_POWER_MANAGER_INTERNAL_H_
#define _ZPAL_POWER_MANAGER_INTERNAL_H_

#include "sl_sleeptimer.h"
#include "sl_power_manager.h"
#include "sl_dcdc.h"

#include <em_core.h>
#include "em_emu.h"
#include "em_burtc.h"

/* @brief Function to convert a silabs power manager em type to a zpal power manager type.
 * @param[in] em The silabs power manager em type.
 * @param[out] pm_type The zpal power manager type.
 * @return ZPAL_STATUS_OK if the conversion is successful, ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_em_to_pm_type(sl_power_manager_em_t em, zpal_pm_type_t* pm_type);

/* @brief Function to convert a zpal power manager type to a silabs power manager em type.
 * @param[in] pm_type The zpal power manager type.
 * @param[out] em_type The silabs power manager em type.
 * @return ZPAL_STATUS_OK if the conversion is successful, ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_pm_type_to_em(zpal_pm_type_t pm_type, sl_power_manager_em_t* em_type);

#endif // _ZPAL_POWER_MANAGER_INTERNAL_H_
