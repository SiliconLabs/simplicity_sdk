/***************************************************************************//**
 * @brief
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

// *** Generated file. Do not edit! ***
// vNCP Version: 1.0

#include "ember-types.h"

void emAfPluginCmsisStackStatusHandler(EmberStatus status);
void emAfPluginCmsisChildJoinHandler(EmberNodeType nodeType,
                                     EmberNodeId nodeId);
void emAfPluginCmsisRadioNeedsCalibratingHandler();
void emAfPluginCmsisStackIsrHandler();
bool emAfPluginCmsisStackIdleHandler(uint32_t *idleTimeMs);
void emAfPluginCmsisMessageSentHandler(EmberStatus status,
                                       EmberOutgoingMessage *message);
void emAfPluginCmsisIncomingMessageHandler(EmberIncomingMessage *message);
void emAfPluginCmsisIncomingMacMessageHandler(EmberIncomingMacMessage *message);
void emAfPluginCmsisMacMessageSentHandler(EmberStatus status,
                                          EmberOutgoingMacMessage *message);
void emAfPluginCmsisIncomingBeaconHandler(EmberPanId panId,
                                          EmberMacAddress *source,
                                          int8_t rssi,
                                          bool permitJoining,
                                          uint8_t beaconFieldsLength,
                                          uint8_t *beaconFields,
                                          uint8_t beaconPayloadLength,
                                          uint8_t *beaconPayload);
void emAfPluginCmsisActiveScanCompleteHandler();
void emAfPluginCmsisEnergyScanCompleteHandler(int8_t mean,
                                              int8_t min,
                                              int8_t max,
                                              uint16_t variance);
void emAfPluginCmsisFrequencyHoppingStartClientCompleteHandler(EmberStatus status);
