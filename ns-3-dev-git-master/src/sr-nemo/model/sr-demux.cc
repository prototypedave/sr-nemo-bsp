#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object-vector.h"
#include "sr-demux.h"
#include "sr-mobility.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6Demux);

TypeId Mipv6Demux::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6Demux")
    .SetParent<Object> ()
    .AddConstructor<Mipv6Demux> ()
    .AddAttribute ("Mobilities", "The set of IPv6 Mobilities registered with this demux.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&Mipv6Demux::m_mobilities),
                   MakeObjectVectorChecker<Mipv6Mobility> ())
  ;
  return tid;
}

Mipv6Demux::Mipv6Demux ()
{
}

Mipv6Demux::~Mipv6Demux ()
{
}

void Mipv6Demux::DoDispose ()
{
  for (Ipv6MobilityList_t::iterator it = m_mobilities.begin (); it != m_mobilities.end (); it++)
    {
      (*it)->Dispose ();
      *it = 0;
    }
  m_mobilities.clear ();
  m_node = 0;
  Object::DoDispose ();
}

void Mipv6Demux::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void Mipv6Demux::Insert (Ptr<Mipv6Mobility> mobility)
{
  m_mobilities.push_back (mobility);
}

Ptr<Mipv6Mobility> Mipv6Demux::GetMobility (int mobilityNumber)
{
  
  for (Ipv6MobilityList_t::iterator i = m_mobilities.begin (); i != m_mobilities.end (); ++i)
    {
      
      if ((*i)->GetMobilityNumber () == mobilityNumber)
        {
          //std::cout<<(*i)->GetMobilityNumber () << mobilityNumber ;
          return *i;
        }
    }
  
  return 0;
}

void Mipv6Demux::Remove (Ptr<Mipv6Mobility> mobility)
{
  m_mobilities.remove (mobility);
}

} /* namespace ns3 */