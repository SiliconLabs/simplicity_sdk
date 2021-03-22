/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Concentrator plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/**
 * @defgroup concentrator Concentrator Support
 * @ingroup component
 * @brief API and Callbacks for the Concentrator Support Component
 *
 * Code that handles periodically broadcasting a many-to-one-route-request
 * (MTORR) so that the device will act as a concentrator (sink) in the network.
 *  - Note: NCP/SOC source route table size can be now defined as part
 * of the source routing library.
 *
 */

/**
 * @addtogroup concentrator
 * @{
 */

extern sl_zigbee_event_control_t sl_zigbee_af_concentrator_update_event_control;

#define LOW_RAM_CONCENTRATOR  SL_ZIGBEE_LOW_RAM_CONCENTRATOR
#define HIGH_RAM_CONCENTRATOR SL_ZIGBEE_HIGH_RAM_CONCENTRATOR

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define sli_zigbee_af_concentrator_start_discovery sl_zigbee_af_concentrator_queue_discovery
void sli_zigbee_af_concentrator_stop_discovery(void);
#endif

/**
 * @name API
 * @{
 */

/** @brief Concentrator source route discovery
 *
 * @return uint32_t
 *
 */
uint32_t sl_zigbee_af_concentrator_queue_discovery(void);

/** @brief Concentrator stop source route discover
 *
 *
 */
void sl_zigbee_af_concentrator_stop_discovery(void);

/** @} */ // end of API

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// These values are defined by appbuilder.
// TODO: these can be removed (and the enum below should be update accordingly
// once we drop AppBuilder support)
#define NONE (0x00)
#define FULL (0x01)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum sl_zigbee_af_plugin_concentrator_router_behavior_t
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  SL_ZIGBEE_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_NONE = NONE,
  SL_ZIGBEE_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL = FULL,
  SL_ZIGBEE_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_MAX = SL_ZIGBEE_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL,
};
typedef uint8_t sl_zigbee_af_plugin_concentrator_router_behavior_t;

extern sl_zigbee_af_plugin_concentrator_router_behavior_t sli_zigbee_af_concentrator_router_behaviors[];
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define sli_zigbee_af_concentrator_router_behavior sli_zigbee_af_concentrator_router_behaviors[sl_zigbee_get_current_network()]
#endif
#define sl_zigbee_af_concentrator_get_router_behavior() \
  (sli_zigbee_af_concentrator_router_behavior)
#define sl_zigbee_af_concentrator_set_router_behavior(behavior) \
  do { sli_zigbee_af_concentrator_router_behavior = behavior; } while (0);

/** @} */ // end of concentrator
