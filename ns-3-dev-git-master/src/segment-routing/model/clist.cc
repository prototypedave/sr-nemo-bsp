/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/log.h"
#include "ns3/node.h"
#include "clist.h"
#include "sr-l4-protocol.h"
#include "sr-mn.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CList");
NS_OBJECT_ENSURE_REGISTERED (CList);

TypeId CList::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::CList")
    .SetParent<Object> ()
  ;
  return tid;
}


CList::~CList ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Flush ();
}

void CList::DoDispose ()
{
  //Flush ();
  Object::DoDispose ();
}


void CList::Flush ()
{
  delete this;
}


Ptr<Node> CList::GetNode () const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

void CList::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION ( this << node );

  m_node = node;
}

CList::BList (std::list<Ipv6Address> aralist)
  : m_signalstate (STRONG),
  m_tunnelIfIndex (-1),
  //m_hpktbu (0),
  m_Aralist (aralist),
  //m_hretransTimer (Timer::CANCEL_ON_DESTROY),
  //m_hreachableTimer (Timer::CANCEL_ON_DESTROY),
  //m_hrefreshTimer (Timer::CANCEL_ON_DESTROY),
  //m_cnstate (UNREACHABLE),
  //m_cnpktbu (0),
  //m_cnretransTimer (Timer::CANCEL_ON_DESTROY),
  //m_cnreachableTimer (Timer::CANCEL_ON_DESTROY),
  //m_cnrefreshTimer (Timer::CANCEL_ON_DESTROY),
  //m_hotiretransTimer (Timer::CANCEL_ON_DESTROY),
  //m_cotiretransTimer (Timer::CANCEL_ON_DESTROY),
  //m_HomeAddressRegisteredFlag (false),
  //m_ARAddressRegisteredFlag (false),
  //m_FlagR(0)  //NEMO

{
  NS_LOG_FUNCTION_NOARGS ();
}

bool CList::IsConnectionStrong () const
{
  NS_LOG_FUNCTION (this);

  return m_signalstate == STRONG;
}

void MarkSignalSrengthWeak ()
{
    NS_LOG_FUNCTION (this);
    m_signalstate = WEAK;
}

void MarkSignalSrengthPoor ()
{
    NS_LOG_FUNCTION (this);
    m_signalstate = POOR;
}

int16_t GetTunnelIfIndex () const 
{
  NS_LOG_FUNCTION (this);

  return m_tunnelIfIndex;
}

void SetTunnelIfIndex (int16_t tunnelif)
{
  NS_LOG_FUNCTION ( this << tunnelif );

  m_tunnelIfIndex = tunnelif;
}

void SetSID(Segment sid) 
{
  m_sid = sid;
}

Segment GetSID () const
{
  return m_sid;
}



}