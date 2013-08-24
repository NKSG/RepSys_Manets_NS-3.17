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
#ifndef NODE_H
#define NODE_H

#include <vector>
//added for CS218 project begin /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <map>
#include <ns3/ipv4.h>
#include <ns3/ipv4-address.h>
//added for CS218 project end ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/net-device.h"

namespace ns3 {

//added for CS218 project begin /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const uint32_t REP_MIN      = 0,
              REP_MAX      = 10,
              REP_DEFAULT  = 10,
              REP_LOW      = 4,
              REP_HIGH     = 8;
const float KLOSS_DEFAULT = 0.1;
const uint32_t UNACK_PKT_TH = 100,
               MAX_NODE_LIFE = 100,
               BHV_TH = 2;

struct repStruct
{
  uint32_t rep;
  float kLoss;
};

struct bhvStruct
{
  uint64_t* pkt;
  uint32_t sntCnt;
  uint32_t bchCnt;
};

enum RepActions
{
  GOOD = 1, // Forward all, be GOOD
  BAD = 2, // Forward partial, be BAD
  EVIL = 3, // Drop all, be EVIL
};
//added for CS218 project end ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Application;
class Packet;
class Address;

/**
 * \ingroup network
 *
 * \brief A network Node.
 *
 * This class holds together:
 *   - a list of NetDevice objects which represent the network interfaces
 *     of this node which are connected to other Node instances through
 *     Channel instances.
 *   - a list of Application objects which represent the userspace
 *     traffic generation applications which interact with the Node
 *     through the Socket API.
 *   - a node Id: a unique per-node identifier.
 *   - a system Id: a unique Id used for parallel simulations.
 *
 * Every Node created is added to the NodeList automatically.
 */

class Node : public Object
{
public:
  static TypeId GetTypeId (void);

  Node();
  /**
   * \param systemId a unique integer used for parallel simulations.
   */
  Node(uint32_t systemId);

  virtual ~Node();

  /**
   * \returns the unique id of this node.
   * 
   * This unique id happens to be also the index of the Node into
   * the NodeList. 
   */
  uint32_t GetId (void) const;

  /**
   * \returns the system id for parallel simulations associated
   *          to this node.
   */
  uint32_t GetSystemId (void) const;

  /**
   * \param device NetDevice to associate to this node.
   * \returns the index of the NetDevice into the Node's list of
   *          NetDevice.
   *
   * Associate this device to this node.
   */
  uint32_t AddDevice (Ptr<NetDevice> device);
  /**
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice associated to this Node.
   *
   * The indexes used by the GetDevice method start at one and
   * end at GetNDevices ()
   */
  Ptr<NetDevice> GetDevice (uint32_t index) const;
  /**
   * \returns the number of NetDevice instances associated
   *          to this Node.
   */
  uint32_t GetNDevices (void) const;

  /**
   * \param application Application to associate to this node.
   * \returns the index of the Application within the Node's list
   *          of Application.
   *
   * Associated this Application to this Node. 
   */
  uint32_t AddApplication (Ptr<Application> application);
  /**
   * \param index
   * \returns the application associated to this requested index
   *          within this Node.
   */
  Ptr<Application> GetApplication (uint32_t index) const;

  /**
   * \returns the number of applications associated to this Node.
   */
  uint32_t GetNApplications (void) const;

  /**
   * A protocol handler
   *
   * \param device a pointer to the net device which received the packet
   * \param packet the packet received
   * \param protocol the 16 bit protocol number associated with this packet.
   *        This protocol number is expected to be the same protocol number
   *        given to the Send method by the user on the sender side.
   * \param sender the address of the sender
   * \param receiver the address of the receiver; Note: this value is
   *                 only valid for promiscuous mode protocol
   *                 handlers.  Note:  If the L2 protocol does not use L2
   *                 addresses, the address reported here is the value of 
   *                 device->GetAddress().
   * \param packetType type of packet received
   *                   (broadcast/multicast/unicast/otherhost); Note:
   *                   this value is only valid for promiscuous mode
   *                   protocol handlers.
   */
  typedef Callback<void,Ptr<NetDevice>, Ptr<const Packet>,uint16_t,const Address &,
                   const Address &, NetDevice::PacketType> ProtocolHandler;
  /**
   * \param handler the handler to register
   * \param protocolType the type of protocol this handler is 
   *        interested in. This protocol type is a so-called
   *        EtherType, as registered here:
   *        http://standards.ieee.org/regauth/ethertype/eth.txt
   *        the value zero is interpreted as matching all
   *        protocols.
   * \param device the device attached to this handler. If the
   *        value is zero, the handler is attached to all
   *        devices on this node.
   * \param promiscuous whether to register a promiscuous mode handler
   */
  void RegisterProtocolHandler (ProtocolHandler handler, 
                                uint16_t protocolType,
                                Ptr<NetDevice> device,
                                bool promiscuous=false);
  /**
   * \param handler the handler to unregister
   *
   * After this call returns, the input handler will never
   * be invoked anymore.
   */
  void UnregisterProtocolHandler (ProtocolHandler handler);

  /**
   * A callback invoked whenever a device is added to a node.
   */
  typedef Callback<void,Ptr<NetDevice> > DeviceAdditionListener;
  /**
   * \param listener the listener to add
   *
   * Add a new listener to the list of listeners for the device-added
   * event. When a new listener is added, it is notified of the existance
   * of all already-added devices to make discovery of devices easier.
   */
  void RegisterDeviceAdditionListener (DeviceAdditionListener listener);
  /**
   * \param listener the listener to remove
   *
   * Remove an existing listener from the list of listeners for the 
   * device-added event.
   */
  void UnregisterDeviceAdditionListener (DeviceAdditionListener listener);

  /**
   * \returns true if checksums are enabled, false otherwise.
   */
  static bool ChecksumEnabled (void);

//added for CS218 project begin /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /**
   * Initialize member variables
   */
     void InitMembers (Ipv4Address selfIpv4);
  /**
   * Reset Stats
   */
     void ResetStats ();
  /**
   * Update or add entry into reputation table.
   */
  void UpdateRepTableEntry (Ipv4Address node, uint32_t rep, float kLoss, bool overWrite);
  /**
   * Check and Update or add entry into packet counts table (used for packet drop based on social norm).
   */
  bool CheckLossTableEntry (Ipv4Address node, uint8_t trasmissionSize);
  /**
   * Add entry into packet tracking table (used for updating reputation).
   */
  bool UpdateBhvTableEntry (Ipv4Address node, uint64_t pktId, uint8_t transmissionSize, uint8_t batchSize);
  /**
   * Delete entry from reputation table.
   */
  void DeleteRepTableEntry (Ipv4Address node);
  /**
   * Delete entry from counts table.
   */
  void DeleteLossTableEntry (Ipv4Address node);
  /**
   * Delete entry from packets table.
   */
  void DeleteBhvTableEntry (Ipv4Address node, uint64_t pktId);
  /**
   * Return reputation based action - based on sigma, following social norm epsilon
   */
  RepActions GetRepAction (Ipv4Address src, Ipv4Address dst);
  /**
   * Attach reputation entries for packet header
   */
  void AttachRepEntries(std::map<Ipv4Address, uint32_t> &allReps);
  /**
   * Dettach reputation entries from packet header
   */
  void DettachRepEntries(std::map<Ipv4Address, uint32_t> allReps, Ipv4Address fromNode);
  /**
   * Update reputation entries in a table 
   */
  void UpdateRepValues (Ipv4Address node, uint8_t transmissionSize, uint8_t batchSize);
  /**
   * Node depart from network
   */
  bool Nodedepart();
  /**
   * Has bad reputation, change towards GOOD
   */
  void ChangeToGood();
  /**
   * Return Ipv4Address
   */
  Ipv4Address GetIpv4Address ()
  {
    return selfAdd;
  }
  /**
   * Increment counters
   */
  void IncPktSent()
  {
    pktSent++;
  }
  void IncPktReceived()
  {
    pktReceived++;
  }
  void IncPktDropped()
  {
    pktDropped++;
  }
  void IncPktGen()
  {
    pktGen++;
  }
  void IncRepBrdSent()
  {
    repBrdSent++;
  }
  void IncRepBrdReceived()
  {
    repBrdReceived++;
  }
  /**
   * Return counters and other values
   */
  float GetDelta()
  {
    return delta;
  }
  float GetDropFactor()
  {
    return dropFactor;
  }
  uint32_t GetPktSent()
  {
    return pktSent;
  }
  uint32_t GetPktReceived()
  {
    return pktReceived;
  }
  uint32_t GetPktDropped()
  {
    return pktDropped;
  }
  uint32_t GetPktGen()
  {
    return pktGen;
  }
  uint32_t GetRepBrdSent()
  {
    return repBrdSent;
  }
  uint32_t GetRepBrdReceived()
  {
    return repBrdReceived;
  }
//added for CS218 project end ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
  /**
   * The dispose method. Subclasses must override this method
   * and must chain up to it by calling Node::DoDispose at the
   * end of their own DoDispose method.
   */
  virtual void DoDispose (void);
  virtual void DoInitialize (void);
private:
  void NotifyDeviceAdded (Ptr<NetDevice> device);
  bool NonPromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet>, uint16_t protocol, const Address &from);
  bool PromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet>, uint16_t protocol,
                                 const Address &from, const Address &to, NetDevice::PacketType packetType);
  bool ReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet>, uint16_t protocol,
                          const Address &from, const Address &to, NetDevice::PacketType packetType, bool promisc);

  void Construct (void);

//added for CS218 project begin /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /**
   * Return reputation for a node.
   */
  uint32_t GetReputation (Ipv4Address node);
  /**
   * Decide to follow social norm epsilon or not, based on probablibilty delta.
   */
  uint8_t funcDelta ();
//added for CS218 project end ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  struct ProtocolHandlerEntry {
    ProtocolHandler handler;
    Ptr<NetDevice> device;
    uint16_t protocol;
    bool promiscuous;
  };
  typedef std::vector<struct Node::ProtocolHandlerEntry> ProtocolHandlerList;
  typedef std::vector<DeviceAdditionListener> DeviceAdditionListenerList;

  uint32_t    m_id;         // Node id for this node
  uint32_t    m_sid;        // System id for this node
  std::vector<Ptr<NetDevice> > m_devices;
  std::vector<Ptr<Application> > m_applications;
  ProtocolHandlerList m_handlers;
  DeviceAdditionListenerList m_deviceAdditionListeners;

//added for CS218 project begin /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Ipv4Address selfAdd; // Ipv4Address
  std::map<Ipv4Address, repStruct> repTable; // Reputation table
  std::map<Ipv4Address, uint8_t> lossTable; // Counts table used for packet drop, as this implementation is based on packets, not flows
  std::map<Ipv4Address, bhvStruct> bhvTable; // Keep track of packets sent to intermediate nodes, used to update reputation
  RepActions rAction; // Action based on reputation
  float delta; // Probability of staying in next batch (0-1)
  int fDeltaHistory[MAX_NODE_LIFE]; // Historical values for f(delta)
  float dropFactor; // Packet drop factor at each node

  uint32_t pktSent;
  uint32_t pktReceived;
  uint32_t pktDropped;
  uint32_t pktGen;
  uint32_t repBrdSent;
  uint32_t repBrdReceived;
//added for CS218 project end ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

} // namespace ns3

#endif /* NODE_H */
