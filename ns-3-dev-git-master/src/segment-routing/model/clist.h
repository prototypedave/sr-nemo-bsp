/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef C_LIST_H
#define C_LIST_H

#include <list>
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/ipv6-address.h"
#include "ns3/ptr.h"
#include "ns3/timer.h"
#include "ns3/sgi-hashmap.h"
#include "ns3/segment.h"

namespace ns3 {
  class CList : public Object 
  {
   public:
    /**
        * \brief Get the type identifier.
        * \return type identifier
    */
    static TypeId GetTypeId ();
    /**
        * \brief constructor.
        * \param aralist AR router address list
    */
    CList (std::list<Ipv6Address> aralist);
    /**
        * \brief destructor
        */
    ~CList ();

    /**
        * \brief get the node pointer.
        * \returns the node pointer 
    */
    Ptr<Node> GetNode () const;

    /**
        * \brief set the node pointer.
        * \param node the node pointer 
    */
    void SetNode (Ptr<Node> node);

    /**
        * \brief whether AR has a strong connection
        * \returns true or false
    */
    bool IsConnectionStrong() const;

    /**
     * \brief sets signal strength to weak
    */
    void MarkSignalSrengthWeak ();

    /**
    * \brief sets signal strength to poor
    */
    void MarkSignalSrengthPoor ();

    /**
        * \brief get tunnel interface index.
        * \return tunnel interface index
    */
    int16_t GetTunnelIfIndex () const;

    /**
     * \brief set tunnel interface index
    */
    void SetTunnelIfIndex (int16_t tunnelif);

    /**
     * \brief assign SID to this router
    */
    void SetSIDRoute(Segment sid);

    /**
    * \brief return SID
    */
    Segment GetSID () const;

   private:
    enum SignalStrength {
        POOR,
        WEAK,
        STRONG
    }

    SignalStrength m_signalstate;

    /**
    * \brief tunnel interface index
    */
    int16_t m_tunnelIfIndex;

    /**
    * \brief SID
    */
    Segment m_sid;





  }
}