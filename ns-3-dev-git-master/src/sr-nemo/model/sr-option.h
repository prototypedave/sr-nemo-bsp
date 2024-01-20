#ifndef SR_OPTION_H
#define SR_OPTION_H

#include <list>

#include "ns3/object.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/header.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
//#include "ns3/identifier.h"

namespace ns3 {

class Mipv6OptionBundle;

/**
 * \class Ipv6MobilityOption
 * \brief Ipv6 Mobility Option base
 *
 * If you want to implement a new Ipv6 Mobility option, all you have to do is
 * implement a subclass of this class and add it to an Ipv6MobilityOptionDemux.
 */
class Mipv6Option : public Object
{
public:
  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Mipv6Option ();

  /**
   * \brief Set the node.
   * \param node the node to set
   */
  void SetNode (Ptr<Node> node);

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const = 0;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle) = 0;

private:
  /**
   * \brief The node.
   */
  Ptr<Node> m_node;
};

/**
 * \brief Option Bundle
 */
class Mipv6OptionBundle
{
public:

  /**
   * \brief constructor
   */
  Mipv6OptionBundle ();

  /**
   * \brief Get the Home address.
   * \return home address
   */
  Ipv6Address GetHomeAddress () const;
  /**
   * \brief Set the Home address.
   * \param hoa home address
   */
  void SetHomeAddress (Ipv6Address hoa);
  /**
   * \brief Get the CoA.
   * \return CoA
   */
  Ipv6Address GetCareofAddress () const;
  /**
   * \brief Set the CoA.
   * \param coa CoA
   */
  void SetCareofAddress (Ipv6Address coa);

  /**
   * \brief Get the home nonce index.
   * \return home nonce index
   */
  uint16_t GetHomeNonceIndex () const;
  /**
   * \brief Set the home nonce index.
   * \param hi home nonce index
   */
  void SetHomeNonceIndex (uint16_t hi);


  /**
   * \brief Get the care-of nonce index.
   * \return care-of nonce index
   */
  uint16_t GetCareofNonceIndex () const;

  /**
   * \brief Set the care-of nonce index.
   * \param coi care-of nonce index
   */
  void SetCareofNonceIndex (uint16_t coi);


  /**
   * \brief Get the authenticator.
   * \return authenticator
   */
  uint64_t GetAuthenticator () const;

  /**
   * \brief Set the authenticator.
   * \param auth the authenticator
   */
  void SetAuthenticator (uint64_t auth);

  /**
   * \brief Get the refresh interval.
   * \return refresh interval
   */
  uint16_t GetRefreshInterval () const;

  /**
   * \brief Set the refresh interval.
   * \param intvl the refresh interval
   */
  void SetRefreshInterval (uint16_t intvl);

  /**
   * \brief Get the mobile network prefix.
   * \return mobile network prefix
   */
   Ipv6Address GetMobileNetworkPrefix () const;   //NEMO
   /**
   * \brief Set the mobile network prefix.
   * \param mobnetpref mobile network prefix
   */
   void SetMobileNetworkPrefix (Ipv6Address mobnetpref);  //NEMO

protected:
private:
  //for MIPv6
  /**
   * \brief home address.
   */
  Ipv6Address m_hoa;
  /**
   * \brief care-of-address.
   */
  Ipv6Address m_coa;
 /**
   * \brief home init cookie.
   */
  uint16_t m_hi;
 /**
   * \brief care-of init cookie.
   */
  uint16_t m_coi;
 /**
   * \brief authenticator.
   */
  uint64_t m_auth;
 /**
   * \brief refresh interval.
   */
  uint16_t m_interval;

   /**
   * \brief mobile netork prefix.
   */
   Ipv6Address m_mobnetpref;        //NEMO
};

/**
 * \class Ipv6MobilityOptionPad1
 * \brief Ipv6 Mobility Option
 */
class Ipv6MobilityOptionPad1 : public Mipv6Option
{
public:
 /**
   * \brief option no.
   */
  static const uint8_t OPT_NUMBER = 0;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityOptionPad1 ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle);

private:
};

/**
 * \class Ipv6MobilityOptionPadn
 * \brief Ipv6 Mobility Option
 */
class Ipv6MobilityOptionPadn : public Mipv6Option
{
public:
 /**
   * \brief option no.
   */
  static const uint8_t OPT_NUMBER = 1;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityOptionPadn ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle);

private:
};

/**
 * \class Ipv6MobilityOptionMobileNodeIdentifier
 * \brief Ipv6 Mobility Option
 */
class Ipv6MobilityOptionBindingRefreshAdvice : public Mipv6Option
{
public:
  /**
   * \brief option no.
   */
  static const uint8_t OPT_NUMBER = 2;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityOptionBindingRefreshAdvice ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle);

private:
};

/**
 * \class Ipv6MobilityOptionHomeNetworkPrefix
 * \brief Ipv6 Mobility Option
 */
class Ipv6MobilityOptionAlternateCareofAddress : public Mipv6Option
{
public:
 /**
   * \brief option no.
   */
  static const uint8_t OPT_NUMBER = 3;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityOptionAlternateCareofAddress ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle);

private:
};

/**
 * \class Ipv6MobilityOptionHandoffIndicator
 * \brief Ipv6 Mobility Option
 */
class Ipv6MobilityOptionNonceIndices : public Mipv6Option
{
public:
 /**
   * \brief option no.
   */
  static const uint8_t OPT_NUMBER = 4;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityOptionNonceIndices ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle);

private:
};

/**
 * \class Ipv6MobilityOptionAccessTechnologyType
 * \brief Ipv6 Mobility Option
 */
class Ipv6MobilityOptionBindingAuthorizationData : public Mipv6Option
{
public:
 /**
   * \brief option no.
   */
  static const uint8_t OPT_NUMBER = 5;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityOptionBindingAuthorizationData ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle);

private:
};


/**
 * \class Ipv6MobilityOptionMobileNetworkPrefix
 * \brief Ipv6 Mobility Option Mobile Network Prefix
 */
class Ipv6MobilityOptionMobileNetworkPrefix : public Mipv6Option    // NEMO
{
public:
 /**
   * \brief option no.
   */
  static const uint8_t OPT_NUMBER = 6;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityOptionMobileNetworkPrefix ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityOptionNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset offset
   * \param bundle bundle of all option data
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> packet, uint8_t offset, Mipv6OptionBundle& bundle);

private:



};



} /* namespace ns3 */

#endif /* IPV6_MOBILITY_OPTION_H */