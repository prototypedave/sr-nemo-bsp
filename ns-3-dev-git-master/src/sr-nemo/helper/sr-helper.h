#ifndef SR_HELPER_H
#define SR_HELPER_H

#include "ns3/ptr.h"
#include "ns3/ipv6-address.h"


namespace ns3 {

class Node;

/**
 * It is a supporting implementation module. It basically works above all the main implemented classes of MIPv6 module.
 * Three helper classes are created for three agent classes:
 * -mipv6MN
 * -mipv6HA and
 * -mipv6CN
 * Users who want to run MIPv6, uses the helper classes to install the MIPv6 functionality to corresponding nodes.
 * These classes implement an important function called Install() . This method takes a node pointer and install
 * over a node. It installs the MIPv6MobL4Protocol and MIPv6TunL4Protocol following the CreateObject() and
 * AggregateObject() functions, defined in the core part of ns-3. After that it registers the corresponding
 * mobility messages using MIPv6MobL4Protocol class
 */

/**
 * \brief HA Helper
 */
class Mipv6HaHelper
{
public:
  Mipv6HaHelper ();
  ~Mipv6HaHelper ();
  /**
   *
   * \param node The node on which to install the stack.
   */
  void Install (Ptr<Node> node);
/**
   *

   * \returns all the link addresses of the home agent.
   */
  std::list<Ipv6Address> GetHomeAgentAddressList ();

   void SetHAAs(bool RorH) ;           // NEMO
   
   bool GetHAAs() const;               // NEMO

protected:
private:
/**
   * \brief node.
   */
  Ptr<Node> m_node;

  bool m_haflag;              //NEMO  
};

/**
 * \brief CN Helper
 */
class Mipv6CnHelper
{
public:
  Mipv6CnHelper ();
  ~Mipv6CnHelper ();

  /**
   *
   * \param node The node on which to install the stack.
   */
  void Install (Ptr<Node> node) const;

protected:
private:
};

/**
 * \brief MN Helper
 */
class Mipv6MnHelper
{
public:
/**
 * \brief constructor
 * \param haalist home agent address list
 * \param rotopt route optimization flag
 */
  Mipv6MnHelper (std::list<Ipv6Address> haalist, bool rotopt);
  ~Mipv6MnHelper ();
  /**
   *
   * \param node The node on which to install the stack.
   */
  void Install (Ptr<Node> node) const;

  void SetMNAs(bool RorH);                    // NEMO
  
  bool GetMNAs() const;                           // NEMO

  void SetMobileNetPref(Ipv6Address mnp);  // NEMo

  Ipv6Address GetMobileNetPref() const;         //  NEMO


protected:
private:
/**
 * \brief home agent address list
 */
  std::list<Ipv6Address> m_Haalist;
/**
 * \brief route optimization flag
 */
  bool m_rotopt; //route optimization flag


  bool m_mnflag; // flag for MN as Router or Host - NEMO
 
  Ipv6Address m_mnp; // mobile network prefix -  NEMO

};

} // namespace ns3

#endif /* MIPv6_HELPER_H */