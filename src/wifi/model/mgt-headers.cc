/*
 * Copyright (c) 2006 INRIA
 * Copyright (c) 2009 MIRKO BANCHI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Mirko Banchi <mk.banchi@gmail.com>
 */

#include "mgt-headers.h"

#include "ns3/address-utils.h"
#include "ns3/simulator.h"

namespace ns3
{

/***********************************************************
 *          Probe Request
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtProbeRequestHeader);

TypeId
MgtProbeRequestHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtProbeRequestHeader")
                            .SetParent<Header>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtProbeRequestHeader>();
    return tid;
}

TypeId
MgtProbeRequestHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

/***********************************************************
 *          Probe Response
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtProbeResponseHeader);

TypeId
MgtProbeResponseHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtProbeResponseHeader")
                            .SetParent<Header>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtProbeResponseHeader>();
    return tid;
}

TypeId
MgtProbeResponseHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint64_t
MgtProbeResponseHeader::GetBeaconIntervalUs() const
{
    return m_beaconInterval;
}

void
MgtProbeResponseHeader::SetBeaconIntervalUs(uint64_t us)
{
    m_beaconInterval = us;
}

const CapabilityInformation&
MgtProbeResponseHeader::Capabilities() const
{
    return m_capability;
}

CapabilityInformation&
MgtProbeResponseHeader::Capabilities()
{
    return m_capability;
}

uint64_t
MgtProbeResponseHeader::GetTimestamp() const
{
    return m_timestamp;
}

uint32_t
MgtProbeResponseHeader::GetSerializedSizeImpl() const
{
    uint32_t size = 8 /* timestamp */ + 2 /* beacon interval */;
    size += m_capability.GetSerializedSize();
    size += WifiMgtHeader<MgtProbeResponseHeader, ProbeResponseElems>::GetSerializedSizeImpl();
    return size;
}

void
MgtProbeResponseHeader::SerializeImpl(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;
    i.WriteHtolsbU64(Simulator::Now().GetMicroSeconds());
    i.WriteHtolsbU16(static_cast<uint16_t>(m_beaconInterval / 1024));
    i = m_capability.Serialize(i);
    WifiMgtHeader<MgtProbeResponseHeader, ProbeResponseElems>::SerializeImpl(i);
}

uint32_t
MgtProbeResponseHeader::DeserializeImpl(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    m_timestamp = i.ReadLsbtohU64();
    m_beaconInterval = i.ReadLsbtohU16();
    m_beaconInterval *= 1024;
    i = m_capability.Deserialize(i);
    auto distance = i.GetDistanceFrom(start);
    return distance + WifiMgtHeader<MgtProbeResponseHeader, ProbeResponseElems>::DeserializeImpl(i);
}

/***********************************************************
 *          Beacons
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtBeaconHeader);

/* static */
TypeId
MgtBeaconHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtBeaconHeader")
                            .SetParent<MgtProbeResponseHeader>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtBeaconHeader>();
    return tid;
}

/***********************************************************
 *          Assoc Request
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtAssocRequestHeader);

TypeId
MgtAssocRequestHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtAssocRequestHeader")
                            .SetParent<Header>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtAssocRequestHeader>();
    return tid;
}

TypeId
MgtAssocRequestHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint16_t
MgtAssocRequestHeader::GetListenInterval() const
{
    return m_listenInterval;
}

void
MgtAssocRequestHeader::SetListenInterval(uint16_t interval)
{
    m_listenInterval = interval;
}

const CapabilityInformation&
MgtAssocRequestHeader::Capabilities() const
{
    return m_capability;
}

CapabilityInformation&
MgtAssocRequestHeader::Capabilities()
{
    return m_capability;
}

uint32_t
MgtAssocRequestHeader::GetSerializedSizeImpl() const
{
    SetMleContainingFrame();

    uint32_t size = 0;
    size += m_capability.GetSerializedSize();
    size += 2; // listen interval
    size += WifiMgtHeader<MgtAssocRequestHeader, AssocRequestElems>::GetSerializedSizeImpl();
    return size;
}

uint32_t
MgtAssocRequestHeader::GetSerializedSizeInPerStaProfileImpl(
    const MgtAssocRequestHeader& frame) const
{
    uint32_t size = 0;
    size += m_capability.GetSerializedSize();
    size +=
        MgtHeaderInPerStaProfile<MgtAssocRequestHeader,
                                 AssocRequestElems>::GetSerializedSizeInPerStaProfileImpl(frame);
    return size;
}

void
MgtAssocRequestHeader::SerializeImpl(Buffer::Iterator start) const
{
    SetMleContainingFrame();

    Buffer::Iterator i = start;
    i = m_capability.Serialize(i);
    i.WriteHtolsbU16(m_listenInterval);
    WifiMgtHeader<MgtAssocRequestHeader, AssocRequestElems>::SerializeImpl(i);
}

void
MgtAssocRequestHeader::SerializeInPerStaProfileImpl(Buffer::Iterator start,
                                                    const MgtAssocRequestHeader& frame) const
{
    Buffer::Iterator i = start;
    i = m_capability.Serialize(i);
    MgtHeaderInPerStaProfile<MgtAssocRequestHeader,
                             AssocRequestElems>::SerializeInPerStaProfileImpl(i, frame);
}

uint32_t
MgtAssocRequestHeader::DeserializeImpl(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    i = m_capability.Deserialize(i);
    m_listenInterval = i.ReadLsbtohU16();
    auto distance = i.GetDistanceFrom(start) +
                    WifiMgtHeader<MgtAssocRequestHeader, AssocRequestElems>::DeserializeImpl(i);
    if (auto& mle = Get<MultiLinkElement>())
    {
        for (std::size_t id = 0; id < mle->GetNPerStaProfileSubelements(); id++)
        {
            auto& perStaProfile = mle->GetPerStaProfile(id);
            if (perStaProfile.HasAssocRequest())
            {
                auto& frameInPerStaProfile =
                    std::get<std::reference_wrapper<MgtAssocRequestHeader>>(
                        perStaProfile.GetAssocRequest())
                        .get();
                frameInPerStaProfile.CopyIesFromContainingFrame(*this);
            }
        }
    }
    return distance;
}

uint32_t
MgtAssocRequestHeader::DeserializeFromPerStaProfileImpl(Buffer::Iterator start,
                                                        uint16_t length,
                                                        const MgtAssocRequestHeader& frame)
{
    Buffer::Iterator i = start;
    i = m_capability.Deserialize(i);
    m_listenInterval = frame.m_listenInterval;
    auto distance = i.GetDistanceFrom(start);
    NS_ASSERT_MSG(distance <= length,
                  "Bytes read (" << distance << ") exceed expected number (" << length << ")");
    return distance + MgtHeaderInPerStaProfile<MgtAssocRequestHeader, AssocRequestElems>::
                          DeserializeFromPerStaProfileImpl(i, length - distance, frame);
}

/***********************************************************
 *          Ressoc Request
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtReassocRequestHeader);

TypeId
MgtReassocRequestHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtReassocRequestHeader")
                            .SetParent<Header>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtReassocRequestHeader>();
    return tid;
}

TypeId
MgtReassocRequestHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint16_t
MgtReassocRequestHeader::GetListenInterval() const
{
    return m_listenInterval;
}

void
MgtReassocRequestHeader::SetListenInterval(uint16_t interval)
{
    m_listenInterval = interval;
}

const CapabilityInformation&
MgtReassocRequestHeader::Capabilities() const
{
    return m_capability;
}

CapabilityInformation&
MgtReassocRequestHeader::Capabilities()
{
    return m_capability;
}

void
MgtReassocRequestHeader::SetCurrentApAddress(Mac48Address currentApAddr)
{
    m_currentApAddr = currentApAddr;
}

uint32_t
MgtReassocRequestHeader::GetSerializedSizeImpl() const
{
    SetMleContainingFrame();

    uint32_t size = 0;
    size += m_capability.GetSerializedSize();
    size += 2; // listen interval
    size += 6; // current AP address
    size += WifiMgtHeader<MgtReassocRequestHeader, AssocRequestElems>::GetSerializedSizeImpl();
    return size;
}

uint32_t
MgtReassocRequestHeader::GetSerializedSizeInPerStaProfileImpl(
    const MgtReassocRequestHeader& frame) const
{
    uint32_t size = 0;
    size += m_capability.GetSerializedSize();
    size +=
        MgtHeaderInPerStaProfile<MgtReassocRequestHeader,
                                 AssocRequestElems>::GetSerializedSizeInPerStaProfileImpl(frame);
    return size;
}

void
MgtReassocRequestHeader::PrintImpl(std::ostream& os) const
{
    os << "current AP address=" << m_currentApAddr << ", ";
    WifiMgtHeader<MgtReassocRequestHeader, AssocRequestElems>::PrintImpl(os);
}

void
MgtReassocRequestHeader::SerializeImpl(Buffer::Iterator start) const
{
    SetMleContainingFrame();

    Buffer::Iterator i = start;
    i = m_capability.Serialize(i);
    i.WriteHtolsbU16(m_listenInterval);
    WriteTo(i, m_currentApAddr);
    WifiMgtHeader<MgtReassocRequestHeader, AssocRequestElems>::SerializeImpl(i);
}

void
MgtReassocRequestHeader::SerializeInPerStaProfileImpl(Buffer::Iterator start,
                                                      const MgtReassocRequestHeader& frame) const
{
    Buffer::Iterator i = start;
    i = m_capability.Serialize(i);
    MgtHeaderInPerStaProfile<MgtReassocRequestHeader,
                             AssocRequestElems>::SerializeInPerStaProfileImpl(i, frame);
}

uint32_t
MgtReassocRequestHeader::DeserializeImpl(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    i = m_capability.Deserialize(i);
    m_listenInterval = i.ReadLsbtohU16();
    ReadFrom(i, m_currentApAddr);
    auto distance = i.GetDistanceFrom(start) +
                    WifiMgtHeader<MgtReassocRequestHeader, AssocRequestElems>::DeserializeImpl(i);
    if (auto& mle = Get<MultiLinkElement>())
    {
        for (std::size_t id = 0; id < mle->GetNPerStaProfileSubelements(); id++)
        {
            auto& perStaProfile = mle->GetPerStaProfile(id);
            if (perStaProfile.HasReassocRequest())
            {
                auto& frameInPerStaProfile =
                    std::get<std::reference_wrapper<MgtReassocRequestHeader>>(
                        perStaProfile.GetAssocRequest())
                        .get();
                frameInPerStaProfile.CopyIesFromContainingFrame(*this);
            }
        }
    }
    return distance;
}

uint32_t
MgtReassocRequestHeader::DeserializeFromPerStaProfileImpl(Buffer::Iterator start,
                                                          uint16_t length,
                                                          const MgtReassocRequestHeader& frame)
{
    Buffer::Iterator i = start;
    i = m_capability.Deserialize(i);
    m_listenInterval = frame.m_listenInterval;
    m_currentApAddr = frame.m_currentApAddr;
    auto distance = i.GetDistanceFrom(start);
    NS_ASSERT_MSG(distance <= length,
                  "Bytes read (" << distance << ") exceed expected number (" << length << ")");
    return distance + MgtHeaderInPerStaProfile<MgtReassocRequestHeader, AssocRequestElems>::
                          DeserializeFromPerStaProfileImpl(i, length - distance, frame);
}

/***********************************************************
 *          Assoc/Reassoc Response
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtAssocResponseHeader);

TypeId
MgtAssocResponseHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtAssocResponseHeader")
                            .SetParent<Header>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtAssocResponseHeader>();
    return tid;
}

TypeId
MgtAssocResponseHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

StatusCode
MgtAssocResponseHeader::GetStatusCode()
{
    return m_code;
}

void
MgtAssocResponseHeader::SetStatusCode(StatusCode code)
{
    m_code = code;
}

const CapabilityInformation&
MgtAssocResponseHeader::Capabilities() const
{
    return m_capability;
}

CapabilityInformation&
MgtAssocResponseHeader::Capabilities()
{
    return m_capability;
}

void
MgtAssocResponseHeader::SetAssociationId(uint16_t aid)
{
    m_aid = aid;
}

uint16_t
MgtAssocResponseHeader::GetAssociationId() const
{
    return m_aid;
}

uint32_t
MgtAssocResponseHeader::GetSerializedSizeImpl() const
{
    SetMleContainingFrame();

    uint32_t size = 0;
    size += m_capability.GetSerializedSize();
    size += m_code.GetSerializedSize();
    size += 2; // aid
    size += WifiMgtHeader<MgtAssocResponseHeader, AssocResponseElems>::GetSerializedSizeImpl();
    return size;
}

uint32_t
MgtAssocResponseHeader::GetSerializedSizeInPerStaProfileImpl(
    const MgtAssocResponseHeader& frame) const
{
    uint32_t size = 0;
    size += m_capability.GetSerializedSize();
    size += m_code.GetSerializedSize();
    size +=
        MgtHeaderInPerStaProfile<MgtAssocResponseHeader,
                                 AssocResponseElems>::GetSerializedSizeInPerStaProfileImpl(frame);
    return size;
}

void
MgtAssocResponseHeader::PrintImpl(std::ostream& os) const
{
    os << "status code=" << m_code << ", "
       << "aid=" << m_aid << ", ";
    WifiMgtHeader<MgtAssocResponseHeader, AssocResponseElems>::PrintImpl(os);
}

void
MgtAssocResponseHeader::SerializeImpl(Buffer::Iterator start) const
{
    SetMleContainingFrame();

    Buffer::Iterator i = start;
    i = m_capability.Serialize(i);
    i = m_code.Serialize(i);
    i.WriteHtolsbU16(m_aid);
    WifiMgtHeader<MgtAssocResponseHeader, AssocResponseElems>::SerializeImpl(i);
}

void
MgtAssocResponseHeader::SerializeInPerStaProfileImpl(Buffer::Iterator start,
                                                     const MgtAssocResponseHeader& frame) const
{
    Buffer::Iterator i = start;
    i = m_capability.Serialize(i);
    i = m_code.Serialize(i);
    MgtHeaderInPerStaProfile<MgtAssocResponseHeader,
                             AssocResponseElems>::SerializeInPerStaProfileImpl(i, frame);
}

uint32_t
MgtAssocResponseHeader::DeserializeImpl(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    i = m_capability.Deserialize(i);
    i = m_code.Deserialize(i);
    m_aid = i.ReadLsbtohU16();
    auto distance = i.GetDistanceFrom(start) +
                    WifiMgtHeader<MgtAssocResponseHeader, AssocResponseElems>::DeserializeImpl(i);
    if (auto& mle = Get<MultiLinkElement>())
    {
        for (std::size_t id = 0; id < mle->GetNPerStaProfileSubelements(); id++)
        {
            auto& perStaProfile = mle->GetPerStaProfile(id);
            if (perStaProfile.HasAssocResponse())
            {
                auto& frameInPerStaProfile = perStaProfile.GetAssocResponse();
                frameInPerStaProfile.CopyIesFromContainingFrame(*this);
            }
        }
    }
    return distance;
}

uint32_t
MgtAssocResponseHeader::DeserializeFromPerStaProfileImpl(Buffer::Iterator start,
                                                         uint16_t length,
                                                         const MgtAssocResponseHeader& frame)
{
    Buffer::Iterator i = start;
    i = m_capability.Deserialize(i);
    i = m_code.Deserialize(i);
    m_aid = frame.m_aid;
    auto distance = i.GetDistanceFrom(start);
    NS_ASSERT_MSG(distance <= length,
                  "Bytes read (" << distance << ") exceed expected number (" << length << ")");
    return distance + MgtHeaderInPerStaProfile<MgtAssocResponseHeader, AssocResponseElems>::
                          DeserializeFromPerStaProfileImpl(i, length - distance, frame);
}

/***********************************************************
 *          Auth Request
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtAuthRequestHeader);

TypeId
MgtAuthRequestHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtAuthRequestHeader")
                            .SetParent<Header>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtAuthRequestHeader>();
    return tid;
}

TypeId
MgtAuthRequestHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
MgtAuthRequestHeader::SetAuthAlgorithm(uint16_t algorithm)
{
    m_authAlgorithm = algorithm;
}

uint16_t
MgtAuthRequestHeader::GetAuthAlgorithm() const
{
    return m_authAlgorithm;
}

void
MgtAuthRequestHeader::SetTransactionSequenceNumber(uint16_t seqNumber)
{
    m_transactionSequenceNumber = seqNumber;
}

uint16_t
MgtAuthRequestHeader::GetTransactionSequenceNumber() const
{
    return m_transactionSequenceNumber;
}

uint32_t
MgtAuthRequestHeader::GetSerializedSizeImpl() const
{
    uint32_t size = 0;
    size += 2; // Algorithm Number (2 bytes)
    size += 2; // Transaction Sequence Number (2 bytes)
    size += WifiMgtHeader<MgtAuthRequestHeader, AuthRequestElems>::GetSerializedSizeImpl();
    return size;
}


void
MgtAuthRequestHeader::SerializeImpl(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;
    i.WriteHtolsbU16(GetAuthAlgorithm());       // Serialize Algorithm Number
    i.WriteHtolsbU16(GetTransactionSequenceNumber());   // Serialize Transaction Sequence Number
    WifiMgtHeader<MgtAuthRequestHeader, AuthRequestElems>::SerializeImpl(i); // Serialize inherited fields
}


uint32_t
MgtAuthRequestHeader::DeserializeImpl(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    SetAuthAlgorithm(i.ReadLsbtohU16());
    SetTransactionSequenceNumber(i.ReadLsbtohU16());
    auto distance = i.GetDistanceFrom(start) +
                    WifiMgtHeader<MgtAuthRequestHeader, AuthRequestElems>::DeserializeImpl(i);
    return distance;
}

/***********************************************************
 *          Auth Response
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED(MgtAuthResponseHeader);

TypeId
MgtAuthResponseHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::MgtAuthResponseHeader")
                            .SetParent<Header>()
                            .SetGroupName("Wifi")
                            .AddConstructor<MgtAuthResponseHeader>();
    return tid;
}

TypeId
MgtAuthResponseHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

StatusCode
MgtAuthResponseHeader::GetStatusCode()
{
    return m_code;
}

void
MgtAuthResponseHeader::SetStatusCode(StatusCode code)
{
    m_code = code;
}


void
MgtAuthResponseHeader::SetAuthenticationAlgorithmNumber(uint16_t algorithm)
{
    m_authAlgorithm = algorithm;
}

uint16_t
MgtAuthResponseHeader::GetAuthenticationAlgorithmNumber() const
{
    return m_authAlgorithm;
}

void
MgtAuthResponseHeader::SetTransactionSequenceNumber(uint16_t seqNumber)
{
    m_transactionSequenceNumber = seqNumber;
}

uint16_t
MgtAuthResponseHeader::GetTransactionSequenceNumber() const
{
    return m_transactionSequenceNumber;
}

uint32_t
MgtAuthResponseHeader::GetSerializedSizeImpl() const
{
    // The size calculation includes the status code, algorithm number, and transaction sequence number
    uint32_t size = 0;
    size += sizeof(StatusCode);               // Status code (2 bytes)
    size += sizeof(uint16_t);                 // Authentication algorithm number (2 bytes)
    size += sizeof(uint16_t);                  // Transaction sequence number (2 byte)
    
    // Include any other elements from the base class (if applicable)
    size += WifiMgtHeader<MgtAuthResponseHeader, AuthResponseElems>::GetSerializedSizeImpl();
    
    return size;
}

void
MgtAuthResponseHeader::SerializeImpl(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;
    
    // Serialize the status code (2 bytes)
    i = m_code.Serialize(i);

    // Serialize the authentication algorithm number (2 bytes)
    i.WriteHtolsbU16(m_authAlgorithm);

    // Serialize the transaction sequence number (2 byte)
    i.WriteHtolsbU16(m_transactionSequenceNumber);

    // Serialize any other elements from the base class (if applicable)
    WifiMgtHeader<MgtAuthResponseHeader, AuthResponseElems>::SerializeImpl(i);
}

uint32_t
MgtAuthResponseHeader::DeserializeImpl(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    // Deserialize the status code (2 bytes)
    i = m_code.Deserialize(i);

    // Deserialize the authentication algorithm number (2 bytes)
    SetAuthenticationAlgorithmNumber(i.ReadLsbtohU16());

    // Deserialize the transaction sequence number (2 byte)
    m_transactionSequenceNumber = i.ReadLsbtohU16();

    // Deserialize any other elements from the base class (if applicable)
    uint32_t distance = WifiMgtHeader<MgtAuthResponseHeader, AuthResponseElems>::DeserializeImpl(i);
    
    return distance;
}

} // namespace ns3
