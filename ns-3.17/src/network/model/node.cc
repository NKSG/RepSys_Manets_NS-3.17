/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 Georgia Tech Research Corporation, INRIA
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
 * Authors: George F. Riley<riley@ece.gatech.edu>
 *          Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

//added for CS218 project begin /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <sstream>
#include <cstdlib>
#include <string>
//added for CS218 project end ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "node.h"
#include "node-list.h"
#include "net-device.h"
#include "application.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/object-vector.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/global-value.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE ("Node");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Node);

GlobalValue g_checksumEnabled  = GlobalValue ("ChecksumEnabled",
                                              "A global switch to enable all checksums for all protocols",
                                              BooleanValue (false),
                                              MakeBooleanChecker ());

TypeId 
Node::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Node")
    .SetParent<Object> ()
    .AddConstructor<Node> ()
    .AddAttribute ("DeviceList", "The list of devices associated to this Node.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&Node::m_devices),
                   MakeObjectVectorChecker<NetDevice> ())
    .AddAttribute ("ApplicationList", "The list of applications associated to this Node.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&Node::m_applications),
                   MakeObjectVectorChecker<Application> ())
    .AddAttribute ("Id", "The id (unique integer) of this Node.",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0),
                   MakeUintegerAccessor (&Node::m_id),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SystemId", "The systemId of this node: a unique integer used for parallel simulations.",
                   TypeId::ATTR_GET|TypeId::ATTR_SET,
                   UintegerValue (0),
                   MakeUintegerAccessor (&Node::m_sid),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

Node::Node()
  : m_id (0),
    m_sid (0)
{
  NS_LOG_FUNCTION (this);
  Construct ();
}

Node::Node(uint32_t sid)
  : m_id (0),
    m_sid (sid)
{ 
  NS_LOG_FUNCTION (this << sid);
  Construct ();
}

void
Node::Construct (void)
{
  NS_LOG_FUNCTION (this);
  m_id = NodeList::Add (this);
}

Node::~Node ()
{
  NS_LOG_FUNCTION (this);
}

uint32_t
Node::GetId (void) const
{
  NS_LOG_FUNCTION (this);
  return m_id;
}

uint32_t
Node::GetSystemId (void) const
{
  NS_LOG_FUNCTION (this);
  return m_sid;
}

uint32_t
Node::AddDevice (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  uint32_t index = m_devices.size ();
  m_devices.push_back (device);
  device->SetNode (this);
  device->SetIfIndex (index);
  device->SetReceiveCallback (MakeCallback (&Node::NonPromiscReceiveFromDevice, this));
  Simulator::ScheduleWithContext (GetId (), Seconds (0.0), 
                                  &NetDevice::Initialize, device);
  NotifyDeviceAdded (device);
  return index;
}
Ptr<NetDevice>
Node::GetDevice (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  NS_ASSERT_MSG (index < m_devices.size (), "Device index " << index <<
                 " is out of range (only have " << m_devices.size () << " devices).");
  return m_devices[index];
}
uint32_t 
Node::GetNDevices (void) const
{
  NS_LOG_FUNCTION (this);
  return m_devices.size ();
}

uint32_t 
Node::AddApplication (Ptr<Application> application)
{
  NS_LOG_FUNCTION (this << application);
  uint32_t index = m_applications.size ();
  m_applications.push_back (application);
  application->SetNode (this);
  Simulator::ScheduleWithContext (GetId (), Seconds (0.0), 
                                  &Application::Initialize, application);
  return index;
}
Ptr<Application> 
Node::GetApplication (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  NS_ASSERT_MSG (index < m_applications.size (), "Application index " << index <<
                 " is out of range (only have " << m_applications.size () << " applications).");
  return m_applications[index];
}
uint32_t 
Node::GetNApplications (void) const
{
  NS_LOG_FUNCTION (this);
  return m_applications.size ();
}

void 
Node::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_deviceAdditionListeners.clear ();
  m_handlers.clear ();
  for (std::vector<Ptr<NetDevice> >::iterator i = m_devices.begin ();
       i != m_devices.end (); i++)
    {
      Ptr<NetDevice> device = *i;
      device->Dispose ();
      *i = 0;
    }
  m_devices.clear ();
  for (std::vector<Ptr<Application> >::iterator i = m_applications.begin ();
       i != m_applications.end (); i++)
    {
      Ptr<Application> application = *i;
      application->Dispose ();
      *i = 0;
    }
  m_applications.clear ();
  Object::DoDispose ();
}
void 
Node::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  for (std::vector<Ptr<NetDevice> >::iterator i = m_devices.begin ();
       i != m_devices.end (); i++)
    {
      Ptr<NetDevice> device = *i;
      device->Initialize ();
    }
  for (std::vector<Ptr<Application> >::iterator i = m_applications.begin ();
       i != m_applications.end (); i++)
    {
      Ptr<Application> application = *i;
      application->Initialize ();
    }

  Object::DoInitialize ();
}

void
Node::RegisterProtocolHandler (ProtocolHandler handler, 
                               uint16_t protocolType,
                               Ptr<NetDevice> device,
                               bool promiscuous)
{
  NS_LOG_FUNCTION (this << &handler << protocolType << device << promiscuous);
  struct Node::ProtocolHandlerEntry entry;
  entry.handler = handler;
  entry.protocol = protocolType;
  entry.device = device;
  entry.promiscuous = promiscuous;

  // On demand enable promiscuous mode in netdevices
  if (promiscuous)
    {
      if (device == 0)
        {
          for (std::vector<Ptr<NetDevice> >::iterator i = m_devices.begin ();
               i != m_devices.end (); i++)
            {
              Ptr<NetDevice> dev = *i;
              dev->SetPromiscReceiveCallback (MakeCallback (&Node::PromiscReceiveFromDevice, this));
            }
        }
      else
        {
          device->SetPromiscReceiveCallback (MakeCallback (&Node::PromiscReceiveFromDevice, this));
        }
    }

  m_handlers.push_back (entry);
}

void
Node::UnregisterProtocolHandler (ProtocolHandler handler)
{
  NS_LOG_FUNCTION (this << &handler);
  for (ProtocolHandlerList::iterator i = m_handlers.begin ();
       i != m_handlers.end (); i++)
    {
      if (i->handler.IsEqual (handler))
        {
          m_handlers.erase (i);
          break;
        }
    }
}

bool
Node::ChecksumEnabled (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  BooleanValue val;
  g_checksumEnabled.GetValue (val);
  return val.Get ();
}

bool
Node::PromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
                                const Address &from, const Address &to, NetDevice::PacketType packetType)
{
  NS_LOG_FUNCTION (this << device << packet << protocol << &from << &to << packetType);
  return ReceiveFromDevice (device, packet, protocol, from, to, packetType, true);
}

bool
Node::NonPromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
                                   const Address &from)
{
  NS_LOG_FUNCTION (this << device << packet << protocol << &from);
  return ReceiveFromDevice (device, packet, protocol, from, device->GetAddress (), NetDevice::PacketType (0), false);
}

bool
Node::ReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
                         const Address &from, const Address &to, NetDevice::PacketType packetType, bool promiscuous)
{
  NS_LOG_FUNCTION (this << device << packet << protocol << &from << &to << packetType << promiscuous);
  NS_ASSERT_MSG (Simulator::GetContext () == GetId (), "Received packet with erroneous context ; " <<
                 "make sure the channels in use are correctly updating events context " <<
                 "when transfering events from one node to another.");
  NS_LOG_DEBUG ("Node " << GetId () << " ReceiveFromDevice:  dev "
                        << device->GetIfIndex () << " (type=" << device->GetInstanceTypeId ().GetName ()
                        << ") Packet UID " << packet->GetUid ());
  bool found = false;

  for (ProtocolHandlerList::iterator i = m_handlers.begin ();
       i != m_handlers.end (); i++)
    {
      if (i->device == 0 ||
          (i->device != 0 && i->device == device))
        {
          if (i->protocol == 0 || 
              i->protocol == protocol)
            {
              if (promiscuous == i->promiscuous)
                {
                  i->handler (device, packet, protocol, from, to, packetType);
                  found = true;
                }
            }
        }
    }
  return found;
}
void 
Node::RegisterDeviceAdditionListener (DeviceAdditionListener listener)
{
  NS_LOG_FUNCTION (this << &listener);
  m_deviceAdditionListeners.push_back (listener);
  // and, then, notify the new listener about all existing devices.
  for (std::vector<Ptr<NetDevice> >::const_iterator i = m_devices.begin ();
       i != m_devices.end (); ++i)
    {
      listener (*i);
    }
}
void 
Node::UnregisterDeviceAdditionListener (DeviceAdditionListener listener)
{
  NS_LOG_FUNCTION (this << &listener);
  for (DeviceAdditionListenerList::iterator i = m_deviceAdditionListeners.begin ();
       i != m_deviceAdditionListeners.end (); i++)
    {
      if ((*i).IsEqual (listener))
        {
          m_deviceAdditionListeners.erase (i);
          break;
         }
    }
}
 
void 
Node::NotifyDeviceAdded (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  for (DeviceAdditionListenerList::iterator i = m_deviceAdditionListeners.begin ();
       i != m_deviceAdditionListeners.end (); i++)
    {
      (*i) (device);
    }  
}
 
//added for CS218 project begin /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
Node::InitMembers (Ipv4Address selfIpv4)
{
  selfAdd = selfIpv4; 
  UpdateRepTableEntry (selfAdd, REP_DEFAULT, KLOSS_DEFAULT, true);
}

void
Node::ResetStats ()
{
  delta = (float)rand()/(float)RAND_MAX;

  dropFactor = ((float)rand()/(float)RAND_MAX);

  if ((rand() % 10) == 0) // randomly set drop factor to 0
    dropFactor = 0;      
  
  for (uint32_t i = 0; i < MAX_NODE_LIFE; fDeltaHistory[i++] = -1);

  pktSent = 0;
  pktReceived = 0;
  pktDropped = 0;
  pktGen = 0;
  repBrdSent = 0;
  repBrdReceived = 0;
}

void
Node::UpdateRepTableEntry (Ipv4Address node, uint32_t rep, float kLoss, bool overWrite)
{ 
  repStruct rs;
  std::map<Ipv4Address, repStruct>::iterator i = Node::repTable.find (node);
  if (i != Node::repTable.end ()) // map entry exists, update
    {
      rs.rep = rep;
      rs.kLoss = (i->second).kLoss;
      if (overWrite) // overwrite existing value
        {
          Node::repTable.erase (i);   
          Node::repTable.insert (std::make_pair (node, rs));
        }
      return;
    }

  rs.rep = rep;
  rs.kLoss = kLoss; 
  Node::repTable.insert (std::make_pair (node, rs));
}

bool
Node::CheckLossTableEntry (Ipv4Address node, uint8_t transmissionSize)
{
  std::map<Ipv4Address, uint8_t>::iterator i = Node::lossTable.find (node);
  if (i != Node::lossTable.end ()) // map entry exists, update
    {
       uint8_t sentCount = i->second;
       Node::lossTable.erase (i);

       if ((sentCount++) + (uint8_t)(dropFactor*transmissionSize) < transmissionSize)
       // Sent packets count less than required for drop factor - forward it
         {
           Node::lossTable.insert (std::make_pair (node, sentCount));
           return true;
         }
       else
       // Drop packet 
         {
           if (sentCount == transmissionSize) // end of transmission size
             sentCount = 0;
           Node::lossTable.insert (std::make_pair (node, sentCount));  
           return false;
         }
    }
     
  Node::lossTable.insert (std::make_pair (node, 1));
  return true;
}

bool 
Node::UpdateBhvTableEntry (Ipv4Address node, uint64_t pktId, uint8_t transmissionSize, uint8_t batchSize)
{
  bhvStruct bs;
  bs.pkt = new uint64_t[UNACK_PKT_TH];
  for (uint32_t t = 0; t < UNACK_PKT_TH; t++)
    bs.pkt[t] = 0;
  bs.sntCnt = 0;
  bs.bchCnt = 0;
  uint8_t j, k;
  std::map<Ipv4Address, bhvStruct>::iterator i = Node::bhvTable.find (node);
  if (i != Node::bhvTable.end ()) // map entry exists for node, add pktID
    {
      for (j = 0; (j < UNACK_PKT_TH) && ((i->second).pkt[j] != 0); j++) // check if pktId already exists
        if ((i->second).pkt[j] == pktId)
          break;

      if ((i->second).pkt[j] == pktId) // duplicate, drop
        {
          delete bs.pkt;
          return false;
        }
      else
        {
          for (k = 0; k < UNACK_PKT_TH && (i->second).pkt[k] != 0; k++) // copy array of packet IDs
            bs.pkt[k] = (i->second).pkt[k];
          if (k == UNACK_PKT_TH) // many packets unacknowledged, drop
            {
              delete bs.pkt;
              return false;
            }
          else 
            {
              bs.pkt[k] = pktId;
              bs.sntCnt = ++((i->second).sntCnt);
              bs.bchCnt = (i->second).bchCnt;
              Node::bhvTable.erase (i);
              Node::bhvTable.insert (std::make_pair (node, bs));
              UpdateRepValues (node, transmissionSize, batchSize);
              return true;
            }
        }
     }
  bs.pkt[0] = pktId;
  bs.sntCnt++;
  Node::bhvTable.insert (std::make_pair (node, bs));
  UpdateRepValues (node, transmissionSize, batchSize);
  return true;
}

void
Node::DeleteRepTableEntry (Ipv4Address node)
{
  std::map<Ipv4Address, repStruct>::iterator i = Node::repTable.find (node);
  Node::repTable.erase (i);
}

void
Node::DeleteLossTableEntry (Ipv4Address node)
{
  std::map<Ipv4Address, uint8_t>::iterator i = Node::lossTable.find (node);
  Node::lossTable.erase (i);
}

void 
Node::DeleteBhvTableEntry (Ipv4Address node, uint64_t pktId)
{
  bhvStruct bs;
  bs.sntCnt = 0;
  bs.bchCnt = 0;
  uint8_t j, k;
  bool dflag = false;
  std::map<Ipv4Address, bhvStruct>::iterator i = Node::bhvTable.find (node);
  if (i != Node::bhvTable.end ()) // map entry exists for node, search for pktId
    {
      bs.pkt = new uint64_t[UNACK_PKT_TH];
      for (uint32_t t = 0; t < UNACK_PKT_TH; t++)
        bs.pkt[t] = 0;
      for (j = 0, k = 0; (i->second).pkt[j] != 0; j++) // check if pktId already exists
        {
          if ((i->second).pkt[j] == pktId)
            {
              dflag = true;
              continue;
            }
          bs.pkt[k++] = (i->second).pkt[j];

        }

      if (dflag) // if anything was deleted, updates required to table
        {
          bs.sntCnt = (i->second).sntCnt;
          bs.bchCnt = (i->second).bchCnt;
          Node::bhvTable.erase (i);
          if (k > 0) // insert again only if other packets exist
            Node::bhvTable.insert (std::make_pair (node, bs));
        }
    }
}  

RepActions
Node::GetRepAction (Ipv4Address src, Ipv4Address dst)
{
  uint32_t srep, drep, rrep;
  srep = GetReputation (src);
  drep = GetReputation (dst);
  rrep = GetReputation (selfAdd);

  if (src == selfAdd)
    return GOOD; // if node is source

  if (funcDelta()) // Probably staying after current batch - follow social norm epsilon
    {
      if ((srep>REP_HIGH) && (drep>REP_HIGH) && (rrep>REP_HIGH)) // Social Norm - Perfect Reliability
        return GOOD;
      else if ((srep<REP_LOW) || (drep<REP_LOW) || (rrep<REP_LOW) ) // Social Norm - No Reliability
        return EVIL;
      else // Social Norm - Partial Reliability
        return BAD;
    }
  else // Probably NOT staying after current batch - drop all packets
    return EVIL;
}

uint32_t
Node::GetReputation (Ipv4Address node)
{
  std::map<Ipv4Address, repStruct>::iterator i = Node::repTable.find (node);
  if (i != Node::repTable.end ()) // map entry exists
    return (i->second).rep;

  // else add default entry
  repStruct rs;
  rs.rep = REP_DEFAULT;
  rs.kLoss = KLOSS_DEFAULT; 
  repTable.insert (std::make_pair (node, rs));
  return REP_DEFAULT; // new entry, return default value 
} 

uint8_t
Node::funcDelta ()
{
  uint32_t i;
  for (i = 0; (i < MAX_NODE_LIFE) && (fDeltaHistory[i] != -1); i++);
  if (i < MAX_NODE_LIFE)
    fDeltaHistory[i] = (((int)delta*100 + rand()) % 2)?1:0;
  return fDeltaHistory[i];
}

void
Node::AttachRepEntries (std::map<Ipv4Address, uint32_t> &allReps)
{
  for (std::map<Ipv4Address, repStruct>::iterator i = Node::repTable.begin (); i != Node::repTable.end (); i++)
        allReps.insert (std::make_pair (i->first, (i->second).rep));
}

void
Node::DettachRepEntries (std::map<Ipv4Address, uint32_t> allReps, Ipv4Address fromNode)
{
  for (std::map<Ipv4Address, uint32_t>::iterator i = allReps.begin (); i != allReps.end (); i++)
    {
      uint32_t trep; 
      if (i->first != fromNode)
        if (i->first == selfAdd)
          { 
            trep = GetReputation (selfAdd);
            if (i->second < trep) // if self's reputation is less than known
              ChangeToGood ();
          }

        std::map<Ipv4Address, repStruct>::iterator j = Node::repTable.find (i->first);
        if (j != Node::repTable.end ()) // map entry exists, update
          {
            trep = (i->second + (j->second).rep) / 2;
            UpdateRepTableEntry (i->first, trep, 0, true); // 0 kLoss as it will be taken from existing entry
          }
        else
          UpdateRepTableEntry (i->first, i->second, 0, true); // 0 kLoss as it will be taken from existing entry      
    }
}

void
Node::UpdateRepValues (Ipv4Address node, uint8_t transmissionSize, uint8_t batchSize)
{
  std::map<Ipv4Address, repStruct>::iterator i = Node::repTable.find (node);
  if (i != Node::repTable.end ()) // map entry exists, update
    {
      std::map<Ipv4Address, bhvStruct>::iterator j = Node::bhvTable.find (i->first);
      if (j != Node::bhvTable.end ()) // map entry exists for node
        if ((j->second).sntCnt >= (transmissionSize * batchSize)) // batch is complete
          {
            uint8_t k, batches = 0, temp1, temp2, temp3;
            uint8_t rep = (i->second).rep;
            float kLoss = (i->second).kLoss;

            // update reputation
            batches = (j->second).sntCnt / (transmissionSize * batchSize);
            temp1 = (uint8_t)(batchSize * transmissionSize);
            temp2 = (uint8_t)(temp1 * batches);
            temp3 = (uint8_t)(temp2 * kLoss);

            for (k = 0; ((j->second).pkt[k] != 0) && (k < UNACK_PKT_TH); k++); // check pktId count 

            if (k == 0) // perfect reliability provided, increase reputation
              {
                if (rep < REP_MAX)
                  rep ++; // GOOD
              }
            else if ((k == 1) && (temp3 != 1)) // near perfect reliability provided, no action
              {
                // no action
              }
            else // partial or no reliability, decrease reputation
              if (rep > REP_MIN)
              {
                rep--; // BAD
                if (k > temp3)
                  if (rep > REP_MIN)
                    rep--; // EVIL
              }

            UpdateRepTableEntry (i->first, rep, kLoss, true);

            uint8_t x, y;
            bhvStruct bs;
            bs.pkt = new uint64_t[UNACK_PKT_TH];
            for (uint32_t t = 0; t < UNACK_PKT_TH; t++)
              bs.pkt[t] = 0;
            bs.sntCnt = (j->second).sntCnt - (batches * batchSize * transmissionSize);
            bs.bchCnt = (j->second).bchCnt + batches;

            for (x = temp2, y = 0; ((j->second).pkt[x] != 0) && x < UNACK_PKT_TH; x++) // delete first temp3 pktIDs        
              bs.pkt[y++] = (j->second).pkt[x]; 
            Node::bhvTable.erase (j);
            if (y > 0) // insert again only if other packets exist
              Node::bhvTable.insert (std::make_pair (i->first, bs));
          }
    }
}

bool
Node::Nodedepart ()
{
  uint32_t c01 = 0, c02 = 0, c0 = 0, i = 0;
  for (; (i < MAX_NODE_LIFE/2) && (fDeltaHistory[i] != -1); i++)
    if (fDeltaHistory[i] == 0)
      c01++;

  for (; (i < MAX_NODE_LIFE) && (fDeltaHistory[i] != -1); i++)
    if (fDeltaHistory[i] == 0)
      c02++;

  c0 = c01 + c02;

  if ((i == MAX_NODE_LIFE) && (c0 > MAX_NODE_LIFE/2) && (c02>c01))
    return true;
  return false; 
}

void
Node::ChangeToGood ()
{
std::cout<<"changing \n";
  if (dropFactor > 0.05)
    dropFactor -= 0.05; // reduce drop factor
}
//added for CS218 project end ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace ns3
