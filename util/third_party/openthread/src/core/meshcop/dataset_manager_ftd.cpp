/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements MeshCoP Datasets manager to process commands.
 *
 */

#include "meshcop/dataset_manager.hpp"

#if OPENTHREAD_FTD

#include <stdio.h>

#include <openthread/platform/radio.h>

#include "coap/coap_message.hpp"
#include "common/as_core_type.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/locator_getters.hpp"
#include "common/log.hpp"
#include "common/random.hpp"
#include "common/timer.hpp"
#include "instance/instance.hpp"
#include "meshcop/dataset.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_leader.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/thread_netif.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/uri_paths.hpp"

namespace ot {
namespace MeshCoP {

RegisterLogModule("DatasetManager");

Error DatasetManager::AppendMleDatasetTlv(Message &aMessage) const
{
    Mle::Tlv::Type mleTlvType = IsActiveDataset() ? Mle::Tlv::kActiveDataset : Mle::Tlv::kPendingDataset;
    Dataset        dataset;

    IgnoreError(Read(dataset));

    // Remove the Active or Pending Timestamp TLV from Dataset before
    // appending to the message. The timestamp is appended as its own
    // MLE TLV to the message.

    dataset.RemoveTlv(IsActiveDataset() ? Tlv::kActiveTimestamp : Tlv::kPendingTimestamp);

    return Tlv::AppendTlv(aMessage, mleTlvType, dataset.GetBytes(), dataset.GetLength());
}

Error DatasetManager::ProcessSetRequest(const Coap::Message &aMessage, SetRequestInfo &aInfo) const
{
    Error              error = kErrorParse;
    Dataset            dataset;
    Timestamp          activeTimestamp;
    ChannelTlvValue    channelValue;
    uint16_t           sessionId;
    Ip6::NetworkPrefix meshLocalPrefix;
    NetworkKey         networkKey;
    uint16_t           panId;
    uint32_t           delayTimer;

    aInfo.Clear();

    SuccessOrExit(dataset.SetFrom(aMessage, aMessage.GetOffset(), aMessage.GetLength() - aMessage.GetOffset()));
    SuccessOrExit(dataset.ValidateTlvs());

    // Verify that the request includes timestamps that are
    // ahead of the locally stored values.

    SuccessOrExit(dataset.Read<ActiveTimestampTlv>(activeTimestamp));

    if (IsPendingDataset())
    {
        Timestamp pendingTimestamp;

        SuccessOrExit(dataset.Read<PendingTimestampTlv>(pendingTimestamp));
        VerifyOrExit(Timestamp::Compare(&pendingTimestamp, mLocal.GetTimestamp()) > 0);
    }
    else
    {
        VerifyOrExit(Timestamp::Compare(&activeTimestamp, mLocal.GetTimestamp()) > 0);
    }

    // Determine whether the new Dataset affects connectivity
    // or network key.

    if ((dataset.Read<ChannelTlv>(channelValue) == kErrorNone) &&
        (channelValue.GetChannel() != Get<Mac::Mac>().GetPanChannel()))
    {
        aInfo.mAffectsConnectivity = true;
    }

    if ((dataset.Read<PanIdTlv>(panId) == kErrorNone) && (panId != Get<Mac::Mac>().GetPanId()))
    {
        aInfo.mAffectsConnectivity = true;
    }

    if ((dataset.Read<MeshLocalPrefixTlv>(meshLocalPrefix) == kErrorNone) &&
        (meshLocalPrefix != Get<Mle::MleRouter>().GetMeshLocalPrefix()))
    {
        aInfo.mAffectsConnectivity = true;
    }

    if (dataset.Read<NetworkKeyTlv>(networkKey) == kErrorNone)
    {
        NetworkKey localNetworkKey;

        Get<KeyManager>().GetNetworkKey(localNetworkKey);

        if (networkKey != localNetworkKey)
        {
            aInfo.mAffectsConnectivity = true;
            aInfo.mAffectsNetworkKey   = true;
        }
    }

    // Check active timestamp rollback. If there is no change to
    // network key, active timestamp must be ahead of local value.

    if (IsPendingDataset() && !aInfo.mAffectsNetworkKey)
    {
        const Timestamp *localActiveTimestamp = Get<ActiveDatasetManager>().GetTimestamp();

        VerifyOrExit(Timestamp::Compare(&activeTimestamp, localActiveTimestamp) > 0);
    }

    // Determine whether the request is from commissioner.

    if (dataset.Read<CommissionerSessionIdTlv>(sessionId) == kErrorNone)
    {
        uint16_t localSessionId;

        aInfo.mIsFromCommissioner = true;

        dataset.RemoveTlv(Tlv::kCommissionerSessionId);

        SuccessOrExit(Get<NetworkData::Leader>().FindCommissioningSessionId(localSessionId));
        VerifyOrExit(localSessionId == sessionId);

        // Verify an MGMT_ACTIVE_SET.req from a Commissioner does not
        // affect connectivity.

        if (IsActiveDataset())
        {
            VerifyOrExit(!aInfo.mAffectsConnectivity);
        }

        // Thread specification allows partial dataset changes for
        // MGMT_ACTIVE_SET.req/MGMT_PENDING_SET.req from Commissioner
        // based on existing active dataset.

        IgnoreError(Get<ActiveDatasetManager>().Read(aInfo.mDataset));
    }

    SuccessOrExit(error = aInfo.mDataset.WriteTlvsFrom(dataset));

    // Check and update the Delay Timer TLV value if present.

    if (aInfo.mDataset.Read<DelayTimerTlv>(delayTimer) == kErrorNone)
    {
        delayTimer = Min(delayTimer, DelayTimerTlv::kMaxDelay);

        if (aInfo.mAffectsNetworkKey && (delayTimer < DelayTimerTlv::kDefaultDelay))
        {
            delayTimer = DelayTimerTlv::kDefaultDelay;
        }
        else
        {
            delayTimer = Max(delayTimer, Get<Leader>().GetDelayTimerMinimal());
        }

        IgnoreError(aInfo.mDataset.Write<DelayTimerTlv>(delayTimer));
    }

exit:
    return error;
}

Error DatasetManager::HandleSet(const Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    StateTlv::State state = StateTlv::kReject;
    SetRequestInfo  info;

    VerifyOrExit(Get<Mle::Mle>().IsLeader());

    SuccessOrExit(ProcessSetRequest(aMessage, info));

    if (IsActiveDataset() && info.mAffectsConnectivity)
    {
        // MGMT_ACTIVE_SET.req which affects connectivity
        // MUST be delayed using pending dataset.

        Get<PendingDatasetManager>().ApplyActiveDataset(info.mDataset);
    }
    else
    {
        SuccessOrExit(Save(info.mDataset));
        Get<NetworkData::Leader>().IncrementVersionAndStableVersion();
    }

    state = StateTlv::kAccept;

    // Notify commissioner if update is from a Thread device.

    if (!info.mIsFromCommissioner)
    {
        uint16_t     localSessionId;
        Ip6::Address destination;

        SuccessOrExit(Get<NetworkData::Leader>().FindCommissioningSessionId(localSessionId));
        SuccessOrExit(Get<Mle::MleRouter>().GetCommissionerAloc(destination, localSessionId));
        Get<Leader>().SendDatasetChanged(destination);
    }

exit:
    SendSetResponse(aMessage, aMessageInfo, state);

    return (state == StateTlv::kAccept) ? kErrorNone : kErrorDrop;
}

void DatasetManager::SendSetResponse(const Coap::Message    &aRequest,
                                     const Ip6::MessageInfo &aMessageInfo,
                                     StateTlv::State         aState)
{
    Error          error = kErrorNone;
    Coap::Message *message;

    message = Get<Tmf::Agent>().NewPriorityResponseMessage(aRequest);
    VerifyOrExit(message != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = Tlv::Append<StateTlv>(*message, aState));

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, aMessageInfo));

    LogInfo("sent dataset set response");

exit:
    FreeMessageOnError(message, error);
}

#if OPENTHREAD_CONFIG_OPERATIONAL_DATASET_AUTO_INIT
Error ActiveDatasetManager::GenerateLocal(void)
{
    Error   error = kErrorNone;
    Dataset dataset;

    VerifyOrExit(Get<Mle::MleRouter>().IsAttached(), error = kErrorInvalidState);
    VerifyOrExit(!mLocal.IsTimestampPresent(), error = kErrorAlready);

    IgnoreError(Read(dataset));

    if (!dataset.Contains<ActiveTimestampTlv>())
    {
        Timestamp timestamp;

        timestamp.Clear();
        IgnoreError(dataset.Write<ActiveTimestampTlv>(timestamp));
    }

    if (!dataset.Contains<ChannelTlv>())
    {
        ChannelTlvValue channelValue;

        channelValue.SetChannelAndPage(Get<Mac::Mac>().GetPanChannel());
        IgnoreError(dataset.Write<ChannelTlv>(channelValue));
    }

    if (!dataset.Contains<ChannelMaskTlv>())
    {
        ChannelMaskTlv::Value value;

        ChannelMaskTlv::PrepareValue(value, Get<Mac::Mac>().GetSupportedChannelMask().GetMask());
        IgnoreError(dataset.WriteTlv(Tlv::kChannelMask, value.mData, value.mLength));
    }

    if (!dataset.Contains<ExtendedPanIdTlv>())
    {
        IgnoreError(dataset.Write<ExtendedPanIdTlv>(Get<ExtendedPanIdManager>().GetExtPanId()));
    }

    if (!dataset.Contains<MeshLocalPrefixTlv>())
    {
        IgnoreError(dataset.Write<MeshLocalPrefixTlv>(Get<Mle::MleRouter>().GetMeshLocalPrefix()));
    }

    if (!dataset.Contains<NetworkKeyTlv>())
    {
        NetworkKey networkKey;

        Get<KeyManager>().GetNetworkKey(networkKey);
        IgnoreError(dataset.Write<NetworkKeyTlv>(networkKey));
    }

    if (!dataset.Contains<NetworkNameTlv>())
    {
        NameData nameData = Get<NetworkNameManager>().GetNetworkName().GetAsData();

        IgnoreError(dataset.WriteTlv(Tlv::kNetworkName, nameData.GetBuffer(), nameData.GetLength()));
    }

    if (!dataset.Contains<PanIdTlv>())
    {
        IgnoreError(dataset.Write<PanIdTlv>(Get<Mac::Mac>().GetPanId()));
    }

    if (!dataset.Contains<PskcTlv>())
    {
        Pskc pskc;

        if (Get<KeyManager>().IsPskcSet())
        {
            Get<KeyManager>().GetPskc(pskc);
        }
        else
        {
            SuccessOrExit(error = pskc.GenerateRandom());
        }

        IgnoreError(dataset.Write<PskcTlv>(pskc));
    }

    if (!dataset.Contains<SecurityPolicyTlv>())
    {
        SecurityPolicyTlv tlv;

        tlv.Init();
        tlv.SetSecurityPolicy(Get<KeyManager>().GetSecurityPolicy());
        IgnoreError(dataset.WriteTlv(tlv));
    }

    SuccessOrExit(error = mLocal.Save(dataset));
    IgnoreError(Restore());

    LogInfo("Generated local dataset");

exit:
    return error;
}

void ActiveDatasetManager::StartLeader(void) { IgnoreError(GenerateLocal()); }
#else  // OPENTHREAD_CONFIG_OPERATIONAL_DATASET_AUTO_INIT
void ActiveDatasetManager::StartLeader(void) {}
#endif // OPENTHREAD_CONFIG_OPERATIONAL_DATASET_AUTO_INIT

template <>
void ActiveDatasetManager::HandleTmf<kUriActiveSet>(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    SuccessOrExit(DatasetManager::HandleSet(aMessage, aMessageInfo));
    IgnoreError(ApplyConfiguration());

exit:
    return;
}

void PendingDatasetManager::StartLeader(void) { StartDelayTimer(); }

template <>
void PendingDatasetManager::HandleTmf<kUriPendingSet>(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    SuccessOrExit(DatasetManager::HandleSet(aMessage, aMessageInfo));
    StartDelayTimer();

exit:
    return;
}

void PendingDatasetManager::ApplyActiveDataset(Dataset &aDataset)
{
    // Generates and applies Pending Dataset from an Active Dataset.

    Timestamp activeTimestamp;

    SuccessOrExit(aDataset.Read<ActiveTimestampTlv>(activeTimestamp));
    SuccessOrExit(aDataset.Write<PendingTimestampTlv>(activeTimestamp));
    SuccessOrExit(aDataset.Write<DelayTimerTlv>(Get<Leader>().GetDelayTimerMinimal()));

    IgnoreError(DatasetManager::Save(aDataset));
    StartDelayTimer();

exit:
    return;
}

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_FTD
