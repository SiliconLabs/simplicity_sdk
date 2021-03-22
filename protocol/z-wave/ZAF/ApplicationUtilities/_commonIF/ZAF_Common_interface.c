/**
 * @file
 * @copyright 2018 Silicon Laboratories Inc.
 */

#include <assert.h>
#include "ZAF_Common_interface.h"
#include "zaf_protocol_config.h"
//#define DEBUGPRINT
#include "DebugPrint.h"

static SApplicationHandles * m_pAppHandles;
static zpal_pm_handle_t m_PowerLock;
static CP_Handle_t m_CmdPublisherHandle;
static zaf_wake_up_callback_t wake_up_callback;

void ZAF_setAppHandle(SApplicationHandles* pAppHandle)
{
  m_pAppHandles = pAppHandle;
}

void ZAF_setPowerLock(zpal_pm_handle_t powerLock)
{
  m_PowerLock = powerLock;
}

void ZAF_SetCPHandle(CP_Handle_t handle)
{
  m_CmdPublisherHandle = handle;
}

// getters

SApplicationHandles* ZAF_getAppHandle()
{
  assert(m_pAppHandles);
  return m_pAppHandles;
}

const SProtocolInfo* ZAF_getProtocolInfo()
{
  assert(m_pAppHandles);
  return m_pAppHandles->pProtocolInfo;
}

SQueueNotifying* ZAF_getZwTxQueue()
{
  assert(m_pAppHandles);
  return m_pAppHandles->pZwTxQueue;
}

SQueueNotifying* ZAF_getZwCommandQueue()
{
  assert(m_pAppHandles);
  return m_pAppHandles->pZwCommandQueue;
}

const zpal_radio_network_stats_t* ZAF_getNetworkStatistics()
{
  assert(m_pAppHandles);
  return m_pAppHandles->pNetworkStatistics;
}

zpal_pm_handle_t ZAF_getPowerLock()
{
  assert(m_PowerLock);
  return m_PowerLock;
}

uint8_t ZAF_GetSecurityKeys(void)
{
  assert(m_pAppHandles);
  return m_pAppHandles->pNetworkInfo->SecurityKeys;
}

CP_Handle_t ZAF_getCPHandle()
{
  assert(m_CmdPublisherHandle);
  return m_CmdPublisherHandle;
}

bool isFLiRS(__attribute__((unused)) const SAppNodeInfo_t * pAppNodeInfo)
{
  return zaf_get_app_node_info()->DeviceOptionsMask & (APPLICATION_FREQ_LISTENING_MODE_1000ms ^ APPLICATION_FREQ_LISTENING_MODE_250ms);
}

EInclusionState_t ZAF_GetInclusionState(void)
{
  assert(m_pAppHandles);
  return m_pAppHandles->pNetworkInfo->eInclusionState;
}

node_id_t ZAF_GetNodeID(void)
{
  assert(m_pAppHandles);
  return m_pAppHandles->pNetworkInfo->NodeId;
}

uint32_t ZAF_GetHomeID(void)
{
  assert(m_pAppHandles);
  return m_pAppHandles->pNetworkInfo->HomeId;
}

node_id_t ZAF_GetSucNodeId(void)
{
  assert(m_pAppHandles);
  return m_pAppHandles->pNetworkInfo->SucNodeId;
}

EInclusionMode_t ZAF_GetInclusionMode(void)
{
  node_id_t currentNodeID = ZAF_GetNodeID();

  if (currentNodeID == 0)
  {
    return EINCLUSIONMODE_NOT_SET;
  }
  else if (currentNodeID <= ZW_MAX_NODES  // Included as regular Z-Wave
      && currentNodeID > 1)  // Only controller can have node id 1.
  {
    return EINCLUSIONMODE_ZWAVE_CLS;
  }
  else if (currentNodeID >= LOWEST_LONG_RANGE_NODE_ID
      && currentNodeID <= HIGHEST_LONG_RANGE_NODE_ID)  // Included as Z-Wave Long Range
  {
    return EINCLUSIONMODE_ZWAVE_LR;
  }
  DPRINTF("NodeID %d is out of bounds and the InclusionMode cannot be determined", currentNodeID);
  return EINCLUSIONMODE_NOT_SET;
}

bool isRfRegionValid(zpal_radio_region_t region)
{
  /* (REGION_2CH_FIRST <= region) is commented because REGION_2CH_FIRST==0. So the test is always
  true, which create a warning.*/
  if ( (/*(REGION_2CH_FIRST <= region) &&*/(region < REGION_2CH_END) ) ||
       (  (REGION_3CH_FIRST <= region) &&  (region < REGION_3CH_END) ) )
  {
    if ( (REGION_DEPRECATED_4 == region) || (REGION_DEPRECATED_10 == region) )
    {
      return false; //deprecated value are not valid region
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

void zaf_set_stay_awake_callback(zaf_wake_up_callback_t callback) {
  wake_up_callback = callback;
}

zaf_wake_up_callback_t zaf_get_stay_awake_callback(void) {
  return wake_up_callback;
}
