/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave power manager.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_POWER_MANAGER_H_
#define ZPAL_POWER_MANAGER_H_

#include <stdbool.h>
#include <stdint.h>

#include "zpal_power_manager_ids.h"
#include "zpal_status.h"

#ifdef __cplusplus
extern "C" {
#endif

// Maximum number of power locks that can be registered in one domain
#define ZPAL_PM_MAX_DOMAIN_LOCK_COUNT 32

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-power-manager
 * @brief
 * Defines a platform abstraction layer for the Z-Wave power manager.
 *
 * The ZPAL power manager offers an API for registering power locks and for receiving events on power mode
 * transitions. Power lock forces the chip to stay awake in a given power mode for a given time.
 * An event handler is invoked every time the transition between power modes occurs.
 *
 * Infinite lock example:\n
 * @code{.c}
 * zpal_pm_lock(ZPAL_PM_TYPE_USE_RADIO, ZPAL_PM_DOMAIN_APP, 0, id);                          Keep the radio active forever, id is the lock ID find it in zpal_power_manager_ids.h
 *                                                                       Other stuff
 * zpal_pm_lock_cancel(type, id);                                        The chip may go to sleep now
 * @endcode
 *
 * Lock with timeout example:\n
 * @code{.c}
 * zpal_pm_lock(ZPAL_PM_TYPE_USE_RADIO, ZPAL_PM_DOMAIN_APP, 2000, id);                        Keep the radio active for 2000 ms from now
 * @endcode
 *
 * Requirements:
 * - The storage for power locks should be able to hold at least 20 items. Note that this number may change in the future.
 *
 * @{
 */

/** @brief Power manager type in which a lock can request an opearation to prevent the system from entering a sleep mode.
 */
typedef enum {
  /* @note: for now rx and tx are at the same level, but future optimization could modify that */
  ZPAL_PM_TYPE_USE_RADIO,                                // Prevents the system to enter sleep mode where both rx and tx radio operations are unavailable.
  ZPAL_PM_TYPE_USE_RADIO_TX = ZPAL_PM_TYPE_USE_RADIO,    // Prevents the system to enter sleep mode where radio tx operations are unavailable.
  ZPAL_PM_TYPE_USE_RADIO_RX = ZPAL_PM_TYPE_USE_RADIO,    // Prevents the system to enter sleep mode where radio rx operations are unavailable.
  ZPAL_PM_TYPE_DEEP_SLEEP,                               // Prevents the system from going to a sleep mode that requires wake up from reset state.
  ZPAL_PM_TYPE_MAX,
} zpal_pm_type_t;

/** @brief Power manager power levels, each level corresponds to physical power mode implementation.
 */
typedef enum {
  ZPAL_PM_ACTIVE,     ///< Processor is active.
  ZPAL_PM_LIGHT_SLEEP,       ///< Processor is sleeping, but all peripherals are enabled.
  ZPAL_PM_DEEP_SLEEP,  ///< Most peripherals are disabled, but the radio is in FLiRS mode.
  ZPAL_PM_HALTED,     ///< Chip is off and will boot from reset. Hence, no transition from this power mode will ever happen.
} zpal_pm_powerlevel_t;

/**
 * For each lock, three states are defined:
 * - ZPAL_PM_STATE_ACTIVE_PERMANENT: The lock is active and will remain active until it is cancelled.
 * - ZPAL_PM_STATE_ACTIVE_TIMER: The lock is active and will remain active until the timer expires.
 * - ZPAL_PM_STATE_INACTIVE: The lock is inactive.
 */
typedef enum {
  ZPAL_PM_STATE_ACTIVE_PERMANENT,
  ZPAL_PM_STATE_ACTIVE_TIMER,
  ZPAL_PM_STATE_INACTIVE,
  ZPAL_PM_STATE_UNDEFINED,
} zpal_pm_state_t;

/**
 * @brief A domain is a group of locks. They are stored in the same domain structure.
 *
 */
typedef enum {
  ZPAL_PM_DOMAIN_APP,       // publicly available domain
  ZPAL_PM_DOMAIN_PAL,       // PAL domain
  ZPAL_PM_DOMAIN_STACK,     // internally used by the stack
  ZPAL_PM_DOMAIN_UNDEFINED
} zpal_pm_domain_t;

/** @brief Power manager domain status structure.
 * A status is a memory block that holds the state of the locks in a domain.
 * A domain consists of two statuses struct: one for the radio and one for the deep sleep.
 */
typedef struct {
  uint8_t length;          // Number of locks in the domain
  unsigned int permanent_bitfield : ZPAL_PM_MAX_DOMAIN_LOCK_COUNT;   // Bitfield to store active the permanent locks state, each bit corresponds to a lock
  uint32_t * pm_timeouts;  // Timeout value for each lock if the lock operates in timer mode
} pm_domain_status_t;

/**
 * @brief Locks the power mode for a given time period.
 *
 * @param type The power manager type for the lock. If the lock is already active, this function does not overwrite the previous lock state. @ref zpal_pm_relock can be used to overwrite the previous lock.
 * @param domain The domain in which the lock is registered.
 * @param timeout_ms The timeout duration in ms. If set to 0, the lock will be permanent. The maximum timeout value is defined by @ref zpal_get_max_timeout.
 * @param idx The lock index
 * @return true when the lock is successfully acquired
 * @return ZPAL_STATUS_BUSY if the lock is already in active state.
 * @return ZPAL_STATUS_INVALID_ARGUMENT when the type or idx are invalid or timeout is greater than the maximum timeout.
 * @return ZPAL_STATUS_OK if the lock is successfully acquired.
 * @note to acquire by force, use @ref zpal_pm_relock
 */
zpal_status_t zpal_pm_lock(zpal_pm_type_t type, zpal_pm_domain_t domain, uint32_t timeout_ms, uint8_t idx);

/**
 * @brief Locks the power mode for a given time period. If the lock is already active this function overwrites the previous lock. Use with caution.
 * You can get the lock active state for the given id by calling @ref zpal_pm_lock_is_active
 * @param type The power manager type for the lock.
 * @param domain The domain in which the lock is registered.
 * @param timeout_ms The timeout duration in ms. If set to 0, the lock will be permanent. The maximum timeout is return by @ref zpal_get_max_timeout.
 * @param idx The lock index
 * @return true when the lock is successfully acquired, false otherwise (bad parameters).
 * @return ZPAL_STATUS_INVALID_ARGUMENT when the type or idx are invalid or timeout is greater than the maximum timeout.
 * @return ZPAL_STATUS_OK if the lock is successfully acquired.
 */
zpal_status_t zpal_pm_relock(zpal_pm_type_t type, zpal_pm_domain_t domain, uint32_t timeout_ms, uint8_t idx);

/**
 * @brief Gets the maximum timeout value that can be set for a lock.
 * @return The maximum timeout value in ms.
 */
uint32_t zpal_get_max_timeout(void);

/**
 * @brief Return the state of a given lock.
 *
 * @param[in] type The power manager type for the lock.
 * @param[in] domain The domain of the lock. A domain is a group of locks that can be managed together.
 * @param[in] idx The lock index
 * @param[out] state (otional) More detailed state of the lock: INACTIVE, ACTIVE_PERMANENT, ACTIVE_TIMER.
 * @return true is in active state, false if inactive.
 * @note Similar to @ref zpal_pm_lock_type_is_active but for an individual lock.
 */
bool zpal_pm_lock_is_active(zpal_pm_type_t type, zpal_pm_domain_t domain, uint8_t idx, zpal_pm_state_t *state);

/** @brief Return the state of the given type of lock. Resulted from all zwave locks operations on that type.
 * @param[in] type The power manager type to be checked.
 * @param[out] state (optional) More detailed state: INACTIVE, ACTIVE_PERMANENT, ACTIVE_TIMER.
 * @return true if the lock is active, false if inactive.
 * @note: Similar to @ref zpal_pm_lock_is_active but done at the type level which contains any locks operating on that type.
 */
bool zpal_pm_lock_type_is_active(zpal_pm_type_t pm_type, zpal_pm_state_t *state);

/**
 * @brief Registers a domain for power locks.
 * @param domain The domain to be registered.
 * @param[in] radio_domain_status Pointer to the radio domain status. If NULL, the domain will not be registered.
 * @param[in] deep_sleep_domain_status  Pointer to the deep sleep domain status. If NULL, the domain will not be registered.
 * @return ZPAL_STATUS_OK if one or two of the domains status are successfully registered, ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_pm_register_domain(zpal_pm_domain_t domain, pm_domain_status_t *radio_domain_status, pm_domain_status_t *deep_sleep_domain_status);

/**
 * @brief This function cancels an active power lock despite of the ongoing mode (permanent or timer).
 *
 * @note If the specified type is not supported, this function will not perform any action.
 * @param[in] type The lock type to cancel.
 * @param domain The domain in which the lock is associated.
 * @param[in] idx The lock index to cancel.
 * @return ZPAL_STATUS_OK if the lock is successfully cancelled, ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_pm_lock_cancel(zpal_pm_type_t type, zpal_pm_domain_t domain, uint8_t idx);

/**
 * @brief Cancels all active power locks on both radio and deep sleep power locks.
 * @note It cancels all power locks created during runtime, hence it can be used ONLY as a part of power down routine.
 */
void zpal_pm_lock_cancel_all(void);

/**
 * @brief Function to update stack logic (radio operations and configuration) before power mode transitions
 * defined in @ref zpal_pm_powerlevel_t.
 * @param from The power level from which the transition is happening.
 * @param to The power level to which the transition is happening.
 * @note This function is called through app_pm_transition_event and is mapped onto power manager platform specific events.
 */
void zpal_zw_pm_event_handler(zpal_pm_powerlevel_t from, zpal_pm_powerlevel_t to);

/* @brief Function to enter a halt state. Before entering the halt state, please make sure to call @ref zpal_zw_pm_event_handler
 * to properly set stack state before halt.
 */
void zpal_pm_halt(void);

/**
 * @} //zpal-power-manager
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_POWER_MANAGER_H_ */
