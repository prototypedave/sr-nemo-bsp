#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object-vector.h"
#include "sr-option.h"
#include "sr-option-demux.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6OptionDemux);

TypeId Mipv6OptionDemux::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6OptionDemux")
    .SetParent<Object> ()
    .AddAttribute ("MobilityOptions", "The set of IPv6 Mobility options registered with this demux.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&Mipv6OptionDemux::m_options),
                   MakeObjectVectorChecker<Mipv6Option> ())
  ;
  return tid;
}

Mipv6OptionDemux::Mipv6OptionDemux ()
{
}

Mipv6OptionDemux::~Mipv6OptionDemux ()
{
}

void Mipv6OptionDemux::DoDispose ()
{
  for (Ipv6MobilityOptionList_t::iterator it = m_options.begin (); it != m_options.end (); it++)
    {
      (*it)->Dispose ();
      *it = 0;
    }
  m_options.clear ();
  m_node = 0;
  Object::DoDispose ();
}

void Mipv6OptionDemux::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void Mipv6OptionDemux::Insert (Ptr<Mipv6Option> option)
{
  m_options.push_back (option);
}

Ptr<Mipv6Option> Mipv6OptionDemux::GetOption (int optionNumber)
{
  for (Ipv6MobilityOptionList_t::iterator i = m_options.begin (); i != m_options.end (); ++i)
    {
      if ((*i)->GetMobilityOptionNumber () == optionNumber)
        {
          return *i;
        }
    }
  return 0;
}

void Mipv6OptionDemux::Remove (Ptr<Mipv6Option> option)
{
  m_options.remove (option);
}

} /* namespace ns3 */