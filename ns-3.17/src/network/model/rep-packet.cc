/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Main definition file for reputation packet
 *
 * CS218 Fall 2012 project 
 * Instructor - Prof. Mario Gerla
 * Tutor - Chuchu
 * Author - Orlando
 * Team - Deepak, Nagendra
 */

#include "rep-packet.h"
#include "ns3/address-utils.h"
#include "packet.h"

namespace ns3 {
namespace rep {

NS_OBJECT_ENSURE_REGISTERED (RepHeader);
RepHeader::RepHeader (Ipv4Address nodeId, uint32_t rep)
  :m_nodeId (nodeId),
   m_rep (rep)
{
}

RepHeader::~RepHeader ()
{
}

TypeId
RepHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::rep::RepHeader")
    .SetParent<Header> ()
    .AddConstructor<RepHeader> ();
  return tid;
}

TypeId
RepHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RepHeader::GetSerializedSize () const
{
  return 8;
}

void
RepHeader::Serialize (Buffer::Iterator i) const
{
  WriteTo (i, m_nodeId);
  i.WriteHtonU32 (m_rep);
}

uint32_t
RepHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  ReadFrom(i,m_nodeId);
  m_rep = i.ReadNtohU32 ();

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RepHeader::Print (std::ostream &os) const
{
  os<< " NodeId: " << m_nodeId
    << " reputation: " << m_rep;
}
}
}
