#ifndef SR_DEMUX_H
#define SR_DEMUX_H

#include <list>
#include "ns3/object.h"
#include "ns3/ptr.h"

namespace ns3 {

class Mipv6Mobility;
class Node;

/**
 * \class Ipv6MobilityDemux
 * \brief Ipv6 Mobility Demux.
 */
class Mipv6Demux : public Object
{
public:
  /**
   * \brief The interface ID.
   * \return type ID
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor.
   */
  Mipv6Demux ();

  /**
   * \brief Destructor.
   */
  virtual ~Mipv6Demux ();

  /**
   * \brief Set the node.
   * \param node the node to set
   */
  void SetNode (Ptr<Node> node);

  /**
   * \brief Insert a new IPv6 Mobility.
   * \param mobility mobility
   */
  void Insert (Ptr<Mipv6Mobility> mobility);

  /**
   * \brief Get the mobility corresponding to mobilityNumber.
   * \param mobilityNumber the mobility number of the option to retrieve
   * \return a matching IPv6 Mobility mobility
   */
  Ptr<Mipv6Mobility> GetMobility (int mobilityNumber);

  /**
   * \brief Remove an mobility from this demux.
   * \param mobility pointer on the mobility to remove
   */
  void Remove (Ptr<Mipv6Mobility> mobility);

protected:
  /**
   * \brief Dispose this object.
   */
  virtual void DoDispose ();

private:
  /**
   * \brief Signature of the list of all IPv6 Mobility supported.
   */
  typedef std::list<Ptr<Mipv6Mobility> > Ipv6MobilityList_t;

  /**
   * \brief List of IPv6 Mobility supported.
   */
  Ipv6MobilityList_t m_mobilities;

  /**
   * \brief The node.
   */
  Ptr<Node> m_node;
};

} /* namespace ns3 */

#endif /* IPV6_MOBILITY_DEMUX_H */