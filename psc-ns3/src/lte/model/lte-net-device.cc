/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
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
 * Author: Giuseppe Piro  <g.piro@poliba.it>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#include "ns3/llc-snap-header.h"
#include "ns3/simulator.h"
#include "ns3/callback.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "lte-net-device.h"
#include "ns3/packet-burst.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "ns3/enum.h"
#include "lte-amc.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv6-header.h"
#include <ns3/lte-radio-bearer-tag.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-l3-protocol.h>
#include <ns3/log.h>
#include <cstdlib> 
#include <iostream>
#include <string>
#include <fstream>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteNetDevice");

NS_OBJECT_ENSURE_REGISTERED (LteNetDevice);

////////////////////////////////
// LteNetDevice
////////////////////////////////

TypeId LteNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteNetDevice")
    .SetParent<NetDevice> ()

    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (30000),
                   MakeUintegerAccessor (&LteNetDevice::SetMtu,
                                         &LteNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

LteNetDevice::LteNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}


LteNetDevice::~LteNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}


void
LteNetDevice::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_node = 0;
  NetDevice::DoDispose ();
}


Ptr<Channel>
LteNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  // we can't return a meaningful channel here, because LTE devices using FDD have actually two channels.
  return 0;
}


void
LteNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this << address);
  m_address = Mac64Address::ConvertFrom (address);
}


Address
LteNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);
  return m_address;
}

void
LteNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}


Ptr<Node>
LteNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION (this);
  return m_node;
}


void
LteNetDevice::SetReceiveCallback (ReceiveCallback cb)
{
  //// Debug logs
  std::cout << "lte-net-device.cc in setreceivecallback" << std::endl;
  // std::cout << "checking cb properties: " << cb.IsEqual << "\n";
  NS_LOG_FUNCTION (this);
  m_rxCallback = cb;
}


bool
LteNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_FATAL_ERROR ("SendFrom () not supported");
  return false;
}


bool
LteNetDevice::SupportsSendFrom (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}



bool
LteNetDevice::SetMtu (const uint16_t mtu)
{
  NS_LOG_FUNCTION (this << mtu);
  m_mtu = mtu;
  return true;
}

uint16_t
LteNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mtu;
}


void
LteNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  m_ifIndex = index;
}

uint32_t
LteNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ifIndex;
}


bool
LteNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION (this);
  return m_linkUp;
}


bool
LteNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

Address
LteNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return Mac48Address::GetBroadcast ();
}

bool
LteNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}


bool
LteNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}


bool
LteNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}


bool
LteNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

Address
LteNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_LOG_FUNCTION (this << multicastGroup);

  Mac48Address ad = Mac48Address::GetMulticast (multicastGroup);

  //
  // Implicit conversion (operator Address ()) is defined for Mac48Address, so
  // use it by just returning the EUI-48 address which is automatically converted
  // to an Address.
  //
  NS_LOG_LOGIC ("multicast address is " << ad);

  return ad;
}

Address
LteNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  Mac48Address ad = Mac48Address::GetMulticast (addr);

  NS_LOG_LOGIC ("MAC IPv6 multicast address is " << ad);
  return ad;
}

void
LteNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION (this);
  m_linkChangeCallbacks.ConnectWithoutContext (callback);
}

void
LteNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Promisc mode not supported");
}

void
LteNetDevice::Receive (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);

  // debug logs
  std::cout << "In function LteNetDevice::Receive()\n";
  std::cout << "Printing packet state as of now\n\n";
  // p->Print(std::cout);
  std::string packet_info=p->ToString();
  std::string command1 = "echo \"" + packet_info + "\" >> scratch/data.txt";
  std::string command2 = "python3 scratch/model.py";
  std::string command3 = "cat /dev/null > scratch/data.txt";
  // passing the packet info to a text file
  // const char* command_cstr = command1.c_str();

    // Execute the command using system
  int returnCode1 = system(command1.c_str());
  int returnCode2 = system(command2.c_str());
  int returnCode3 = system(command3.c_str());


  std::cout <<returnCode1<<" ::: "<<returnCode2<<" ::: "<<returnCode3<<"\n";

  /*'BENIGN': 0,
 'LDAP': 1,
 'MSSQL': 2,
 'UDP': 3,
 'NetBIOS': 4,
 'Syn': 5,
 'UDPLag': 6,
 'Portmap': 7*/
  std::ifstream inputFile("scratch/predicted_attack.txt");
  int flag=0;
    if (inputFile.is_open()) {
        std::string read_attack_type;

        // Read a string from the file
        inputFile >> read_attack_type;

        if(read_attack_type=="0"){
          flag=1;
          std::cout<<"Normal Flow"<<std::endl;
        }else if(read_attack_type=="1"){
          std::cout<<"LDAP attack! =========<PACKET DROPPED>========="<<std::endl;
        }else if(read_attack_type=="2"){
          std::cout<<"MSSQL attack! =========<PACKET DROPPED>========="<<std::endl;
        }else if(read_attack_type=="3"){
          std::cout<<"UDP attack! =========<PACKET DROPPED>========="<<std::endl;
        }else if(read_attack_type=="4"){
          std::cout<<"NetBIOS attack! =========<PACKET DROPPED>========="<<std::endl;
        }else if(read_attack_type=="5"){
          std::cout<<"Syn attack! =========<PACKET DROPPED>========="<<std::endl;
        }else if(read_attack_type=="6"){
          std::cout<<"UDPLag attack! =========<PACKET DROPPED>========="<<std::endl;
        }else if(read_attack_type=="7"){
          std::cout<<"Portmap attack! =========<PACKET DROPPED>========="<<std::endl;
        }
        inputFile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
  Ipv4Header ipv4Header;
  Ipv6Header ipv6Header;

  if(flag){
    if (p->PeekHeader (ipv4Header) != 0)
    {
      NS_LOG_LOGIC ("IPv4 stack...");
      m_rxCallback (this, p, Ipv4L3Protocol::PROT_NUMBER, Address ());
    }
  else if  (p->PeekHeader (ipv6Header) != 0)
    {
      NS_LOG_LOGIC ("IPv6 stack...");
      m_rxCallback (this, p, Ipv6L3Protocol::PROT_NUMBER, Address ());
    }
  else
    {
      NS_ABORT_MSG ("LteNetDevice::Receive - Unknown IP type...");
    }
  }
  
}
}
