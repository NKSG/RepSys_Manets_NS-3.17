/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Hemanth Narra
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
 * Author: Hemanth Narra <hemanth@ittc.ku.com>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  http://wiki.ittc.ku.edu/resilinets
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/dsdv-helper.h"
#include <iostream>
#include <cmath>
#include <cstdlib> //added

using namespace ns3;

uint16_t port = 9;

NS_LOG_COMPONENT_DEFINE ("DsdvManetExample");

class DsdvManetExample
{
public:
  DsdvManetExample ();
  void CaseRun (uint32_t nWifis,
                uint32_t nSinks,
                double totalTime,
                std::string rate,
                std::string phyMode,
                uint32_t nodeSpeed,
                uint32_t periodicUpdateInterval,
                uint32_t settlingTime,
                double dataStart,
                bool printRoutes,
                std::string CSVfileName,
                uint8_t m_transmissionSize, //added
                uint8_t m_batchSize, //added
                std::string CSVfileName_RepTable, //added
                std::string CSVfileName_Counter); //added

private:
  uint32_t m_nWifis;
  uint32_t m_nSinks;
  double m_totalTime;
  std::string m_rate;
  std::string m_phyMode;
  uint32_t m_nodeSpeed;
  uint32_t m_periodicUpdateInterval;
  uint32_t m_settlingTime;
  double m_dataStart;
  uint32_t bytesTotal;
  uint32_t packetsReceived;
  bool m_printRoutes;
  std::string m_CSVfileName;
  uint8_t m_transmissionSize; //added
  uint8_t m_batchSize; //added
  std::string m_CSVfileName_RepTable; //added
  std::string m_CSVfileName_Counter; //added

  NodeContainer nodes;
  NetDeviceContainer devices;
  Ipv4InterfaceContainer interfaces;

private:
  void CreateNodes ();
  void CreateDevices (std::string tr_name);
  void InstallInternetStack (std::string tr_name);
  void InstallApplications ();
  void SetupMobility ();
  void ReceivePacket (Ptr <Socket> );
  Ptr <Socket> SetupPacketReceive (Ipv4Address, Ptr <Node> );
  void CheckThroughput ();
  void PeriodicNodeChange(); //added
};

int main (int argc, char **argv)
{
  DsdvManetExample test;
  uint32_t nWifis = 10; //added
  uint32_t nSinks = 10; //added
  double totalTime = 150.0;
  std::string rate ("8kbps");
  std::string phyMode ("DsssRate11Mbps");
  uint32_t nodeSpeed = 10; // in m/s
  std::string appl = "all";
  uint32_t periodicUpdateInterval = 15;
  uint32_t settlingTime = 6;
  double dataStart = 50.0;
  bool printRoutingTable = true;
  std::string CSVfileName = "DsdvManetExample.csv";
  uint8_t transmissionSize = 5; //added
  uint8_t batchSize = 2; //added
  std::string CSVfileName_RepTable = "DsdvManetExample_RepTable.csv"; //added
  std::string CSVfileName_Counter = "DsdvManetExample_Counter.csv"; //added

  CommandLine cmd;
  cmd.AddValue ("nWifis", "Number of wifi nodes[Default:10]", nWifis); //added
  cmd.AddValue ("nSinks", "Number of wifi sink nodes[Default:10]", nSinks); //added
  cmd.AddValue ("totalTime", "Total Simulation time[Default:100]", totalTime);
  cmd.AddValue ("phyMode", "Wifi Phy mode[Default:DsssRate11Mbps]", phyMode);
  cmd.AddValue ("rate", "CBR traffic rate[Default:8kbps]", rate);
  cmd.AddValue ("nodeSpeed", "Node speed in RandomWayPoint model[Default:10]", nodeSpeed);
  cmd.AddValue ("periodicUpdateInterval", "Periodic Interval Time[Default=15]", periodicUpdateInterval);
  cmd.AddValue ("settlingTime", "Settling Time before sending out an update for changed metric[Default=6]", settlingTime);
  cmd.AddValue ("dataStart", "Time at which nodes start to transmit data[Default=50.0]", dataStart);
  cmd.AddValue ("printRoutingTable", "print routing table for nodes[Default:1]", printRoutingTable);
  cmd.AddValue ("CSVfileName", "The name of the CSV output file name[Default:DsdvManetExample.csv]", CSVfileName);
  cmd.AddValue ("transmissionSize", "Number of packets in a transmission[Default:5]", transmissionSize); //added
  cmd.AddValue ("batchSize", "Number of transmissions in a batch[Default:2]", batchSize); //added
  cmd.AddValue ("CSVfileName_RepTable", "The name of the RepTable output file name[Default:DsdvManetExample_RepTable.csv]", CSVfileName); //added
  cmd.AddValue ("CSVfileName_Counter", "The name of the Counter output file name[Default:DsdvManetExample_Counter.csv]", CSVfileName); //added
  cmd.Parse (argc, argv);

  std::ofstream out (CSVfileName.c_str ());
  out << "SimulationSecond," <<
  "ReceiveRate," <<
  "PacketsReceived," <<
  "NumberOfSinks," <<
  std::endl;
  out.close ();

  std::ofstream outrep (CSVfileName_RepTable.c_str (), std::ios::app); //added
  outrep << "Current Node" << "," << "Time" << "," << "Neighbor Node ID" << "," << "Neighbor Node Rep" << std::endl; //added
  outrep.close (); //added

  std::ofstream outcounter (CSVfileName_Counter.c_str (), std::ios::app); //added
  outcounter << "Node" << "," << "Time" << "," << "delta" << "," << "dropFactor" << "," << "pktSent" << "," << "pktReceived" << "," 
             << "pktDropped" << "," << "pktGen" << "," << "repBrdSent" << "," << "repBrdReceived" << std::endl; //added
  outcounter.close (); //added

  SeedManager::SetSeed (12345);

  Config::SetDefault ("ns3::OnOffApplication::PacketSize", StringValue ("1000"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (rate));
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2000"));

  test = DsdvManetExample ();
  test.CaseRun (nWifis, nSinks, totalTime, rate, phyMode, nodeSpeed, periodicUpdateInterval,
                settlingTime, dataStart, printRoutingTable, CSVfileName, 
                transmissionSize, batchSize, CSVfileName_RepTable, CSVfileName_Counter); //added

  NS_LOG_UNCOND ("Simulation for " << totalTime << " s complete!");

  return 0;
}

DsdvManetExample::DsdvManetExample ()
  : bytesTotal (0),
    packetsReceived (0)
{
}

void
DsdvManetExample::ReceivePacket (Ptr <Socket> socket)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << " Received one packet!");
  Ptr <Packet> packet;
  while (packet = socket->Recv ())
    {
      bytesTotal += packet->GetSize ();
      packetsReceived += 1;
    }
}

void
DsdvManetExample::CheckThroughput ()
{
  double kbs = (bytesTotal * 8.0) / 1000;
  bytesTotal = 0;

  std::ofstream out (m_CSVfileName.c_str (), std::ios::app);

  out << (Simulator::Now ()).GetSeconds () << "," << kbs << "," << packetsReceived << "," << m_nSinks << std::endl;

  out.close ();
  packetsReceived = 0;
  Simulator::Schedule (Seconds (1.0), &DsdvManetExample::CheckThroughput, this);
}

Ptr <Socket>
DsdvManetExample::SetupPacketReceive (Ipv4Address addr, Ptr <Node> node)
{

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr <Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, port);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback ( &DsdvManetExample::ReceivePacket, this));

  return sink;
}

void
DsdvManetExample::CaseRun (uint32_t nWifis, uint32_t nSinks, double totalTime, std::string rate,
                           std::string phyMode, uint32_t nodeSpeed, uint32_t periodicUpdateInterval, uint32_t settlingTime,
                           double dataStart, bool printRoutes, std::string CSVfileName, uint8_t transmissionSize, uint8_t batchSize,
                           std::string CSVfileName_RepTable, std::string CSVfileName_Counter) //added
{
  m_nWifis = nWifis;
  m_nSinks = nSinks;
  m_totalTime = totalTime;
  m_rate = rate;
  m_phyMode = phyMode;
  m_nodeSpeed = nodeSpeed;
  m_periodicUpdateInterval = periodicUpdateInterval;
  m_settlingTime = settlingTime;
  m_dataStart = dataStart;
  m_printRoutes = printRoutes;
  m_CSVfileName = CSVfileName;
  m_transmissionSize = transmissionSize; //added
  m_batchSize = batchSize; //added
  m_CSVfileName_RepTable = CSVfileName_RepTable; //added
  m_CSVfileName_Counter = CSVfileName_Counter; //added

  std::stringstream ss;
  ss << m_nWifis;
  std::string t_nodes = ss.str ();

  std::stringstream ss3;
  ss3 << m_totalTime;
  std::string sTotalTime = ss3.str ();

  std::string tr_name = "Dsdv_Manet_" + t_nodes + "Nodes_" + sTotalTime + "SimTime";
//  std::cout << "Trace file generated is " << tr_name << ".tr\n"; //added

  CreateNodes ();
  CreateDevices (tr_name);
  SetupMobility ();
  InstallInternetStack (tr_name);
  InstallApplications ();

  std::cout << "\nStarting simulation for " << m_totalTime << " s ...\n";

  CheckThroughput ();
  PeriodicNodeChange(); //added

  Simulator::Stop (Seconds (m_totalTime));
  Simulator::Run ();
  Simulator::Destroy ();
}

void
DsdvManetExample::CreateNodes ()
{
  std::cout << "Creating " << (unsigned) m_nWifis << " nodes.\n";
  nodes.Create (m_nWifis);
  NS_ASSERT_MSG (m_nWifis >= m_nSinks, "Sinks must be less or equal to the number of nodes in network"); //added
}

void
DsdvManetExample::SetupMobility ()
{
  MobilityHelper mobility;
  ObjectFactory pos;
  pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));

  std::ostringstream speedConstantRandomVariableStream;
  speedConstantRandomVariableStream << "ns3::ConstantRandomVariable[Constant="
                                   << m_nodeSpeed
                                   << "]";

  Ptr <PositionAllocator> taPositionAlloc = pos.Create ()->GetObject <PositionAllocator> ();
  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel", "Speed", StringValue (speedConstantRandomVariableStream.str ()),
                             "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=2.0]"), "PositionAllocator", PointerValue (taPositionAlloc));
  mobility.SetPositionAllocator (taPositionAlloc);
  mobility.Install (nodes);
}

void
DsdvManetExample::CreateDevices (std::string tr_name)
{
  double lossExp = 2.0; //added
  double rssThreshold = -85; //added
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.Set("EnergyDetectionThreshold", DoubleValue(rssThreshold)); //added
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(lossExp)); //added
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (m_phyMode), "ControlMode",
                                StringValue (m_phyMode));
  devices = wifi.Install (wifiPhy, wifiMac, nodes);

  AsciiTraceHelper ascii;
//  wifiPhy.EnableAsciiAll (ascii.CreateFileStream (tr_name + ".tr")); //added
//  wifiPhy.EnablePcapAll (tr_name); //added
}

void
DsdvManetExample::InstallInternetStack (std::string tr_name)
{
  DsdvHelper dsdv;
  dsdv.Set ("PeriodicUpdateInterval", TimeValue (Seconds (m_periodicUpdateInterval)));
  dsdv.Set ("SettlingTime", TimeValue (Seconds (m_settlingTime)));
  dsdv.Set ("TransmissionSize", UintegerValue (10)); //m_transmissionSize)); //added
  dsdv.Set ("BatchSize", UintegerValue (2)); //m_batchSize)); //added
  dsdv.Set ("CSVfileName_RepTable", StringValue (m_CSVfileName_RepTable)); //added
  dsdv.Set ("CSVfileName_Counter", StringValue (m_CSVfileName_Counter)); //added
  InternetStackHelper stack;
  stack.SetRoutingHelper (dsdv); // has effect on the next Install ()
  stack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  interfaces = address.Assign (devices);
  if (m_printRoutes)
    {
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ((tr_name + ".routes"), std::ios::out);
      dsdv.PrintRoutingTableAllAt (Seconds (m_periodicUpdateInterval), routingStream);
    }
}

void
DsdvManetExample::InstallApplications ()
{
  for (uint32_t i = 0; i <= m_nSinks - 1; i++ )
    {
      Ptr<Node> node = NodeList::GetNode (i);
      Ipv4Address nodeAddress = node->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ();
      Ptr<Socket> sink = SetupPacketReceive (nodeAddress, node);
      //sink->SetAllowBroadcast(true); //added
    }

  for (uint32_t clientNode = 0; clientNode <= m_nWifis - 1; clientNode++ )
    {
      for (uint32_t j = 0; j <= m_nSinks - 1; j++ )
        { 
	  OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (interfaces.GetAddress (j), port)));
          //OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address ("255.255.255.255"), port))); //added
          onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
          onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

          if (j != clientNode)
            {
              ApplicationContainer apps1 = onoff1.Install (nodes.Get (clientNode));
              Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
              apps1.Start (Seconds (var->GetValue (m_dataStart, m_dataStart + 1)));
              apps1.Stop (Seconds (m_totalTime));
            }
        }
    }
}

//added
void DsdvManetExample::PeriodicNodeChange ()
{
 /* Ptr<Node> newNode = new Node;

  uint8_t rv = rand () % 2;
  if (rv == 0)
    {
        nodes.Add (newNode);

  CreateDevices (tr_name);
  SetupMobility ();
  InstallInternetStack (tr_name);
  InstallApplications ();
  else 
        //remove node
node->Nodedepart

*/
  Simulator::Schedule (Seconds (5.0), &DsdvManetExample::PeriodicNodeChange, this);

}

