/***************************************************************************//**
 * @file
 * @brief Configuration header for EZSP Host
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

// *******************************************************************
// Stack Profile Parameters

// *******************************************************************
// Stack Memory Requirements

#define SL_ZIGBEE_CHILD_TABLE_TOKEN_SIZE                32

#ifdef SL_ZIGBEE_USING_TREE_STACK
  #define SL_ZIGBEE_APS_INDIRECT_BINDING_TABLE_TOKEN_SIZE 32
#else
  #define SL_ZIGBEE_BINDING_TABLE_TOKEN_SIZE              32
#endif

#define SL_ZIGBEE_TRANSPORT_CONNECTION_COUNT            0

// *******************************************************************
// Application Handlers

#define EZSP_APPLICATION_HAS_TIMER_HANDLER

// *******************************************************************
// Application Configuration

#define EZSP_TOKEN_SIZE    8
#define EZSP_TOKEN_ENTRIES 8

#define SL_ZIGBEE_ASSERT_SERIAL_PORT 0
