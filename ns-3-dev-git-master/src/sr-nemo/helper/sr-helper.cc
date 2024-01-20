#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/node.h"
#include "ns3/sr-l4-protocol.h"
#include "ns3/sr-header.h"
#include "ns3/sr-mobility.h"
#include "ns3/sr-mn.h"
#include "ns3/ha.h"
#include "ns3/cn.h"
#include "ns3/sr-tun-l4-protocol.h"
#include "sr-helper.h"


NS_LOG_COMPONENT_DEFINE ("MIPv6Helper");

namespace ns3 {

//HA Helper

Mipv6HaHelper::Mipv6HaHelper ()
{
m_haflag=false;
}

Mipv6HaHelper::~Mipv6HaHelper ()
{
}

void Mipv6HaHelper::SetHAAs(bool RorH)   //  NEMO
{
m_haflag=RorH;
}

bool Mipv6HaHelper::GetHAAs( ) const   //  NEMO
{
return m_haflag;
}

void
Mipv6HaHelper::Install (Ptr<Node> node)
{
  m_node = node;
  Ptr<Mipv6L4Protocol> mipv6 = node->GetObject<Mipv6L4Protocol> ();

  if (mipv6 == 0)
    {
      mipv6 = CreateObject<Mipv6L4Protocol> ();
  
      node->AggregateObject (mipv6);
  
      mipv6 = node->GetObject<Mipv6L4Protocol> ();
  
      mipv6->RegisterMobility();

      mipv6->RegisterMobilityOptions();
    }

  Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol> ();
  
  if ( ip6tunnel == 0 )
    {
      ip6tunnel = CreateObject<Ipv6TunnelL4Protocol> ();
	  
      node->AggregateObject (ip6tunnel);
    }

  Ptr<Mipv6Mn> mn = node->GetObject<Mipv6Mn> ();
  if (mn)
    {

      NS_ASSERT_MSG( !mn, "MN stack is installed on HA, not allowed");
  
    }  
  
  Ptr<Mipv6Ha> ha = CreateObject<Mipv6Ha> (m_haflag);  // adding the argument in constructor for NEMO
 	
  node->AggregateObject (ha);
}

std::list<Ipv6Address> Mipv6HaHelper::GetHomeAgentAddressList ()
{
  Ptr<Mipv6Ha> ha = m_node->GetObject<Mipv6Ha> ();
  NS_ASSERT (ha != 0);  // must install mipv6ha before
  return ha->HomeAgentAddressList ();
}

//MN Helper

Mipv6MnHelper::Mipv6MnHelper (std::list<Ipv6Address> haalist, bool rotopt)
 : m_Haalist (haalist),  //home agent list
 m_rotopt (rotopt)     //must be false in this implementation (set as false by default)
{
 m_rotopt = false;
 m_mnflag=false;  //NEMO
 m_mnp="0::0";   //NEMO
}

Mipv6MnHelper::~Mipv6MnHelper ()
{
}

void Mipv6MnHelper::SetMNAs(bool RorH)   //NEMO
{
m_mnflag=RorH;
}

bool Mipv6MnHelper::GetMNAs() const              //NEMO
{
return m_mnflag;
}

void Mipv6MnHelper::SetMobileNetPref(Ipv6Address mnp)    //NEMO
{
m_mnp=mnp;
}

Ipv6Address Mipv6MnHelper::GetMobileNetPref() const   //NEMO
{
return m_mnp;
}

void
Mipv6MnHelper::Install (Ptr<Node> node) const
{
  Ptr<Mipv6L4Protocol> mipv6 = node->GetObject<Mipv6L4Protocol> ();

  if (mipv6 == 0)
    {
      mipv6 = CreateObject<Mipv6L4Protocol> ();
  
      node->AggregateObject (mipv6);
	  
      mipv6 = node->GetObject<Mipv6L4Protocol> ();
  
      mipv6->RegisterMobility ();

      mipv6->RegisterMobilityOptions ();
    }

  Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol> ();
  
  if ( ip6tunnel == 0 )
    {
      ip6tunnel = CreateObject<Ipv6TunnelL4Protocol> ();
	  
      node->AggregateObject (ip6tunnel);
    }

  Ptr<Mipv6Ha> ha = node->GetObject<Mipv6Ha> ();
  if (ha)
    {
 
      NS_ASSERT_MSG ( !ha, "HA stack is installed on MN, not allowed");

    }
  //(adding last2 argument in constructor for NEMO)
  Ptr<Mipv6Mn> mn = CreateObject<Mipv6Mn> (m_Haalist,m_mnflag,m_mnp);  // Pass Home Agent Address List as argument (adding last 2 arguments for NEMO in constructor)

  mn->SetRouteOptimizationReuiredField (m_rotopt);  //Set by default false as the current implementation does
                                                //not support route optimization, otherwise set to m_rotopt
  node->AggregateObject (mn);
}




//CN Helper

Mipv6CnHelper::Mipv6CnHelper ()
{
}

Mipv6CnHelper::~Mipv6CnHelper ()
{
}

void
Mipv6CnHelper::Install (Ptr<Node> node) const
{
  Ptr<Mipv6L4Protocol> mipv6 = node->GetObject<Mipv6L4Protocol> ();

  if (mipv6 == 0)
    {
      mipv6 = CreateObject<Mipv6L4Protocol> ();
  
      node->AggregateObject (mipv6);
  
      mipv6 = node->GetObject<Mipv6L4Protocol> ();
  
      mipv6->RegisterMobility ();

      mipv6->RegisterMobilityOptions ();
    }
	

  
  Ptr<Mipv6CN> cn = CreateObject<Mipv6CN> ();
 	
  node->AggregateObject (cn);
}


} // namespace ns3