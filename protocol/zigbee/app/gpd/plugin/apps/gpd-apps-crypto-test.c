/***************************************************************************//**
 * @file
 * @brief Security Test vectors from Gp Spec for different security.
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
#include "gpd-components-common.h"

#if defined (EMBER_GPD_PACKET_SECURITY_TEST)
uint8_t testOutgoingMpdu[100];
uint8_t testOutgoingMpduLength = 0;
//A.1.5.4 Security test vectors for ApplicationID = 0b000 and a shared key[0]
//A 1.5.4.1 Common Setting
uint8_t testSharedKey[] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                            0xC8, 0xC9, 0xCa, 0xCb, 0xCc, 0xCd, 0xCe, 0xCf };
//
void testSectionA1542(void)
{
  sli_zigbee_gpd.addr.appId = 0;
  sli_zigbee_gpd.addr.id.srcId = 0x87654321;
  sli_zigbee_gpd.rxAfterTx = 0;
  sli_zigbee_gpd.securityFrameCounter = 1;// That gets incremented before send, Test FC = 2
  // copy the test shared key
  (void) memcpy(sli_zigbee_gpd.securityKey, testSharedKey, 16);
  sli_zigbee_gpd.securityKeyType = 3; //Shared Security key type
  sli_zigbee_gpd.securityLevel = 2;

  uint8_t command[] = { 0x20 };

  sl_zigbee_gpd_security_init(&(sli_zigbee_gpd.addr),
                              sli_zigbee_gpd.securityKey,
                              sli_zigbee_gpd.securityFrameCounter);
  sl_zigbee_af_gpdf_send(0, &sli_zigbee_gpd, command, 1, 1);
}

void testSectionA1543(void)
{
  sli_zigbee_gpd.addr.appId = 0;
  sli_zigbee_gpd.addr.id.srcId = 0x87654321;
  sli_zigbee_gpd.rxAfterTx = 0;
  sli_zigbee_gpd.securityFrameCounter = 1;// That gets incremented before send, Test FC = 2
  // copy th test shared key
  (void) memcpy(sli_zigbee_gpd.securityKey, testSharedKey, 16);
  sli_zigbee_gpd.securityKeyType = 3; //Shared Security key type
  sli_zigbee_gpd.securityLevel = 3;

  uint8_t command[] = { 0x20 };
  sl_zigbee_gpd_security_init(&(sli_zigbee_gpd.addr),
                              sli_zigbee_gpd.securityKey,
                              sli_zigbee_gpd.securityFrameCounter);
  sl_zigbee_af_gpdf_send(0, &sli_zigbee_gpd, command, 1, 1);
}

uint8_t receiveMpdu1542[] = {
  0x18,                    //Packet Length
  0x01, 0x08,              // MAC Header
  0x02,                    // Seq No
  0xFF, 0xFF,              // PAN Id
  0xFF, 0xFF,              // Dst Id
  0x8C,                    // Nwc FC
  0x10,                    // Ext Nwk FC
  0x21, 0x43, 0x65, 0x87,  // Src GPD Id
  0x02, 0x00, 0x00, 0x00,  // Frame Counter
  0x20,                    // Gpd Command
  0xCF, 0x78, 0x7E, 0x72   // Nwk MIC
};
uint8_t testSectionA1542ReceiveStatus = 0xFF;
static void testSectionA1542Receive(void)
{
  sli_zigbee_gpd.addr.appId = 0;
  sli_zigbee_gpd.addr.id.srcId = 0x87654321;
  sli_zigbee_gpd.rxAfterTx = 0;
  sli_zigbee_gpd.securityFrameCounter = 2;
  // copy th test shared key
  (void) memcpy(sli_zigbee_gpd.securityKey, testSharedKey, 16);
  sli_zigbee_gpd.securityKeyType = 3; //Shared Security key type
  sli_zigbee_gpd.securityLevel = 3;

  uint8_t command[] = { 0x20 };
  sl_zigbee_gpd_security_init(&(sli_zigbee_gpd.addr),
                              sli_zigbee_gpd.securityKey,
                              sli_zigbee_gpd.securityFrameCounter);
  testSectionA1542ReceiveStatus = sl_zigbee_decrypt_rx_buffer(receiveMpdu1542,
                                                              8,
                                                              0,
                                                              11);
}

typedef void (*fn_t)(void);

fn_t testfn[] = {
  testSectionA1542,
  testSectionA1542Receive,
  testSectionA1543
};
#define TOTAL_TESTS (sizeof(testfn) / sizeof(fn_t))
uint8_t testNumber = 0;
static void securityTests(void)
{
  if (testNumber >= TOTAL_TESTS) {
    testNumber = 0;
  }
  testfn[testNumber]();
  testNumber++;
}
#endif
///////////////////////////////////////////////////////////////////////////////
