/***************************************************************************//**
 * @file
 * @brief Routines for sending data via xmodem
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

#include "app/framework/include/af.h"
#include "ota-bootload-ncp.h"

//------------------------------------------------------------------------------

//#define DEBUG_PRINT_ON
#if defined DEBUG_PRINT_ON
  #define DBGPRINT(...) sl_zigbee_af_ota_bootload_cluster_println(__VA_ARGS__)
#else
  #define DBGPRINT(...)
#endif

#define SOH   (0x01)
#define EOT   (0x04)
#define ACK   (0x06)
#define NAK   (0x15)
#define CAN   (0x18)

#define DATA_SIZE        (128)
#define HEADER_SIZE      (3)
#define FOOTER_SIZE      (2)
#define FULL_BLOCK_SIZE  (HEADER_SIZE + DATA_SIZE + FOOTER_SIZE)

#define CONTROL_OFFSET      (0)
#define BLOCK_NUM_OFFSET    (1)
#define BLOCK_COMP_OFFSET   (2)
#define DATA_OFFSET         (3)
#define CRC_H_OFFSET        (131)
#define CRC_L_OFFSET        (132)

#define UNINITIALIZED       (0)
#define START_TRANSMISSION  (1)
#define SENDING             (2)

static uint8_t state = UNINITIALIZED;
static uint8_t buffFinger;
static uint8_t dataBuff[DATA_SIZE];
static uint8_t blockNum;

//------------------------------------------------------------------------------

static bool sendBlock(uint8_t blockNum, const uint8_t *data)
{
  int i;
  int retry = 5;
  uint16_t crc = 0;
  uint8_t status = NAK;
  uint8_t fullBlock[FULL_BLOCK_SIZE];

  fullBlock[CONTROL_OFFSET] = SOH;
  fullBlock[BLOCK_NUM_OFFSET] = blockNum;
  fullBlock[BLOCK_COMP_OFFSET] = ~blockNum;
  for (i = 0; i < DATA_SIZE; i++) {
    crc = halCommonCrc16(data[i], crc);
    fullBlock[DATA_OFFSET + i] = data[i];
  }
  fullBlock[CRC_H_OFFSET] = HIGH_BYTE(crc);
  fullBlock[CRC_L_OFFSET] = LOW_BYTE(crc);

  while ( (status == NAK) && (retry > 0) ) {
    //print "block %d (" % num,
    //for i in range(0,len(data)):
    //  print "%02x" % ord(data[i]),
    if (!sli_zigbee_af_bootload_send_data(fullBlock, FULL_BLOCK_SIZE)) {
      DBGPRINT("sendBlock() fail 1");
      return false;
    }
    retry--;
    if (!sli_zigbee_af_bootload_wait_char(&status, false, 0)) {
      DBGPRINT("sendBlock() fail 2");
      return false;
    }
    while ( status == 'C' ) {
      // may have leftover C characters from start of transmission
      if (!sli_zigbee_af_bootload_wait_char(&status, false, 0)) {
        DBGPRINT("sendBlock() fail 3");
        return false;
      }
    }
  }

  return (status == ACK);
}

void sli_zigbee_af_init_xmodsli_zigbee_state(bool startImmediately)
{
  if (startImmediately) {
    // skip checking for 'C' characters
    state = SENDING;
  } else {
    state = START_TRANSMISSION;
  }

  buffFinger = 0;
  blockNum = 1;
}

bool sli_zigbee_af_send_xmodem_data(const uint8_t *data, uint16_t length, bool finished)
{
  uint8_t rxData;
  uint16_t i;

  if (state == START_TRANSMISSION) {
    if (sli_zigbee_af_bootload_wait_char(&rxData, true, 'C')) {
      DBGPRINT("sending\n");
      state = SENDING;
    } else {
      DBGPRINT("NoC\n");
      return false;
    }
  }

  if (state == SENDING) {
    for (i = 0; i < length; i++) {
      dataBuff[buffFinger++] = data[i];
      if (buffFinger >= DATA_SIZE) {
        DBGPRINT("block %d\n", blockNum);
        sl_zigbee_af_core_flush();
        if (!sendBlock(blockNum, dataBuff)) {
          DBGPRINT("sendblock err\n");
          sl_zigbee_af_core_flush();
          return false;
        }
        buffFinger = 0;
        blockNum++;
      }
    }
    if ( finished ) {
      if ( buffFinger != 0 ) {
        // pad and send final block
        bool result;
        while (buffFinger < DATA_SIZE) {
          dataBuff[buffFinger++] = 0xFF;
        }
        DBGPRINT("final block %d\n", blockNum);
        result = sendBlock(blockNum, dataBuff);
        if (!result) {
          return false;
        }
      }
      DBGPRINT("EOT\n", blockNum);
      sli_zigbee_af_bootload_send_byte(EOT);
      if (!sli_zigbee_af_bootload_wait_char(&rxData, true, ACK)) {
        DBGPRINT("NoEOTAck\n");
        return false;
      }
    }
  } else {
    DBGPRINT("badstate\n");
    return false;
  }
  return true;
}
