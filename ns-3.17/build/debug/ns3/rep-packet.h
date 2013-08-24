/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Header file for reputation packet
 *
 * CS218 Fall 2012 project 
 * Instructor - Prof. Mario Gerla
 * Tutor - Chuchu
 * Author - Orlando
 * Team - Deepak, Nagendra
 */

#ifndef REP_PACKET_H
#define REP_PACKET_H

#include <iostream>
#include "header.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"

namespace ns3 {
namespace rep {

/**
 * \ingroup rep
 * \brief Node Reputation Update Packet Format
 * \verbatim
 |      0        |      1        |      2        |       3       |
  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                      Destination Address                      |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                        Reputation Value                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * \endverbatim
 */

class RepHeader : public Header
{
public:
  RepHeader (Ipv4Address dst = Ipv4Address (), uint32_t rep = 0);
  virtual ~RepHeader ();
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  void
  SetNodeId (Ipv4Address nodeId)
  {
    m_nodeId = nodeId;
  }
  Ipv4Address
  GetNodeId () const
  {
    return m_nodeId;
  }
  void
  SetRepValue (uint32_t rep)
  {
    m_rep = rep;
  }
  uint32_t
  GetRepValue () const
  {
    return m_rep;
  }

private:
  Ipv4Address m_nodeId;
  uint32_t m_rep;
};

static inline std::ostream & operator<< (std::ostream& os, const RepHeader & packet)
{
  packet.Print (os);
  return os;
}
}
}

#endif /* REP_PACKET_H */
