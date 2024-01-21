#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/net-device.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv6-interface.h"
#include "sr-header.h"
#include "sr-agent.h"
#include "sr-l4-protocol.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/traced-value.h"
#include "ns3/uinteger.h"
#include "ns3/ipv6-header.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("Mipv6Agent");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6Agent);

TypeId Mipv6Agent::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6Agent")
    .SetParent<Object> ()
    .AddConstructor<Mipv6Agent> ()
    .AddTraceSource ("AgentTx",
                     "Trace source indicating a transmitted mobility handling packets by this agent",
                     MakeTraceSourceAccessor (&Mipv6Agent::m_agentTxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("AgentPromiscRx",
                     "Trace source indicating a received mobility handling packets by this agent. This is a promiscuous trace",
                     MakeTraceSourceAccessor (&Mipv6Agent::m_agentPromiscRxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("AgentRx",
                     "Trace source indicating a received mobility handling packets by this agent. This is a non-promiscuous trace",
                     MakeTraceSourceAccessor (&Mipv6Agent::m_agentRxTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}

Mipv6Agent::Mipv6Agent ()
  : m_node (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

Mipv6Agent::~Mipv6Agent ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void Mipv6Agent::DoDispose ()
{
  m_node = 0;
  Object::DoDispose ();
}

void Mipv6Agent::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node> Mipv6Agent::GetNode (void)
{
  NS_LOG_FUNCTION (this);
  return m_node;
}
uint8_t Mipv6Agent::Receive (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << packet << src << dst << interface );

  m_agentPromiscRxTrace (packet);

  Ptr<Packet> p = packet->Copy ();

  Mipv6Header mh;

  p->PeekHeader (mh);

  uint8_t mhType = mh.GetMhType ();

  if (mhType == Mipv6Header::IPV6_MOBILITY_BINDING_UPDATE)
    {
      NS_LOG_FUNCTION (this << packet << src << "BU" << "recieve BU");
      m_agentRxTrace (packet);
      HandleBU (packet, src, dst, interface);
    }
  else if (mhType == Mipv6Header::IPV6_MOBILITY_BINDING_ACKNOWLEDGEMENT)
    {
      NS_LOG_FUNCTION (this << packet << src << "receive BACK");
      m_agentRxTrace (packet);
      HandleBA (packet, src, dst, interface);
    }


  else if (mhType == Mipv6Header::HOME_TEST_INIT)
    {
      NS_LOG_FUNCTION (this << packet << src << "HoTI" << "recieve HoTI");
      m_agentRxTrace (packet);
      HandleHoTI (packet, src, dst, interface);
    }
  else if (mhType == Mipv6Header::HOME_TEST)
    {
      NS_LOG_FUNCTION (this << packet << src << "HoT" << "recieve HoT");
      m_agentRxTrace (packet);
      HandleHoT (packet, src, dst, interface);
    }
  else if (mhType == Mipv6Header::CARE_OF_TEST_INIT)
    {
      NS_LOG_FUNCTION (this << packet << src << "CoTI" << "recieve CoTI");
      m_agentRxTrace (packet);
      HandleCoTI (packet, src, dst, interface);
    }
  else if (mhType == Mipv6Header::CARE_OF_TEST)
    {
      NS_LOG_FUNCTION (this << packet << src << "CoT" << "recieve CoT");
      m_agentRxTrace (packet);
      HandleCoT (packet, src, dst, interface);
    }
  else
    {
      NS_LOG_ERROR ("Unknown MHType (" << (uint32_t)mhType << ")");
    }

  return 0;
}

void Mipv6Agent::SendMessage (Ptr<Packet> packet, Ipv6Address dst, uint32_t ttl)
{
  NS_LOG_FUNCTION (this << packet << dst << (uint32_t)ttl << "send");

  Ptr<Ipv6L3Protocol> ipv6 = m_node->GetObject<Ipv6L3Protocol> ();

  NS_ASSERT (!ipv6 && !ipv6->GetRoutingProtocol ());

  Ipv6Header header;
  SocketIpTtlTag tag;
  Socket::SocketErrno err;
  Ptr<Ipv6Route> route;
  Ptr<NetDevice> oif (0); //specify non-zero if bound to a source address

  header.SetDestination (dst);
  route = ipv6->GetRoutingProtocol ()->RouteOutput (packet, header, oif, err);

  if (!route)
    {
      tag.SetTtl (ttl);
      packet->AddPacketTag (tag);
      Ipv6Address src = route->GetSource ();
      NS_LOG_FUNCTION ("Lura1" << src << "    " << dst);

      m_agentTxTrace (packet);
      ipv6->Send (packet, src, dst, 135, route);
      NS_LOG_LOGIC ("route found and send hmipv6 message");
    }
  else
    {
      NS_LOG_LOGIC ("no route.. drop mipv6 message");
    }
}

uint8_t Mipv6Agent::HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );

  NS_LOG_WARN ("No handler for BU message");

  return 0;
}

uint8_t Mipv6Agent::HandleBA (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );

  NS_LOG_WARN ("No handler for BA message");

  return 0;
}



uint8_t Mipv6Agent::HandleHoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );

  NS_LOG_WARN ("No handler for HoTI message");

  return 0;
}

uint8_t Mipv6Agent::HandleCoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );

  NS_LOG_WARN ("No handler for CoTI message");

  return 0;
}
uint8_t Mipv6Agent::HandleHoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );

  NS_LOG_WARN ("No handler for HoT message");

  return 0;
}

uint8_t Mipv6Agent::HandleCoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );

  NS_LOG_WARN ("No handler for CoT message");

  return 0;
}

} /* namespace ns3 */