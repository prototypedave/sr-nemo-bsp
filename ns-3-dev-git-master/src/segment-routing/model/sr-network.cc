#include "sr-network.h"
#include "ns3/position-allocator.h"
#include "ns3/mobility-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteConfig");
NS_OBJECT_ENSURE_REGISTERED (LteConfig);

static TypeId
GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteConfig")
    .SetParent<Object> ()
    .AddConstructor<LteConfig> ();
  return tid;
}

TypeId
GetInstanceTypeId (void) const{
  return GetTypeId ();
}

LteConfig::LteConfig (double x, double y)
{ 
    m_pos.x = x;
    m_pos.y = y;
    congested = false;
}

LteConfig::~LteConfig ()
{

}

Ptr<Node> LteConfig::GetNode () const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

void LteConfig::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION ( this << node );

  m_node = node;
}

double LteConfig::CalculateRSSI(Position& pos)
{
    double p_received = powerloss * CalculateDistance(m_pos, pos);
    return p_received - RSRP;
}

double LteConfig::CalculateDistance(Position& pos1, Position& pos2)
{
    return std::sqrt(std::pow(pos2.x - pos1.x, 2) + std::pow(pos2.y - pos1.y, 2));
}

void LteConfig::SetLoad (uint16_t load) 
{
    m_load = load;
}

uint16_t LteConfig::GetLoad () const 
{
    return m_load;
}

double LteConfig::calculatePowerUsage ()
{
    double usage = GetLoad () * p_usage;
    return usage;
}

bool IsCongested () const
{
    if (calculatePowerUsage () / power > 0.63)
    {
        congested = True;
        return congested;
    }
    return congested;
}

NS_LOG_COMPONENT_DEFINE ("NrMicro");
NS_OBJECT_ENSURE_REGISTERED (NrMicro);

static TypeId
GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrMicro")
    .SetParent<LteConfig> ()
    .AddConstructor<NrMicro> ();
  return tid;
}

TypeId
GetInstanceTypeId (void) const{
  return GetTypeId ();
}

NrMicro::NrMicro (double x, double y)
{ 
    m_pos.x = x;
    m_pos.y = y;  
}

NrMicro::~NrMicro ()
{

}

Ptr<Node> NrMicro::GetNode () const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

void NrMicro::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION ( this << node );

  m_node = node;
}

NS_LOG_COMPONENT_DEFINE ("NrMacro");
NS_OBJECT_ENSURE_REGISTERED (NrMacro);

static TypeId
GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrMacro")
    .SetParent<NrMicro> ()
    .AddConstructor<NrMacro> ();
  return tid;
}

TypeId
GetInstanceTypeId (void) const{
  return GetTypeId ();
}

NrMacro::NrMacro (double x, double y)
{ 
    m_pos.x = x;
    m_pos.y = y;
    congested = false;
}

NrMacro::~NrMacro ()
{

}

NS_LOG_COMPONENT_DEFINE ("NetworkMonitor");
NS_OBJECT_ENSURE_REGISTERED (NetworkMonitor);

static TypeId
GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NetworkMonitor")
    .SetParent<Object> ()
    .AddConstructor<NetworkMonitor> ();
  return tid;
}

TypeId
GetInstanceTypeId (void) const{
  return GetTypeId ();
}

Ptr<Node> NrMacro::GetNode () const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

void NrMacro::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION ( this << node );

  m_node = node;
}


}