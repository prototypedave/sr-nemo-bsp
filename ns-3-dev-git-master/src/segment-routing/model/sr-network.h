/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/object-factory.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include <fstream>

namespace ns3 {

struct Position {
    double x;
    double y;
}

class LteConfig : public Object {
  public:
      /**
        * \brief Get the type identifier.
        * \return type identifier
      */
      static TypeId GetTypeId ();

      TypeId GetInstanceTypeId (void) const;

      LteConfig (double x, double y);

      ~LteConfig ();

      Ptr<Node> GetNode () const;

      void SetNode (Ptr<Node> node);

      double CalculateRSSI(Position& pos);

      double CalculateDistance(Position& pos1, Position& pos2);

      uint16_t GetLoad () const;

      void SetLoad (uint16_t load);

      double calculatePowerUsage ();

      bool IsCongested () const;

    private:
      double RSRP = -114;   // in dBm
      
      double powerloss = 0.25;  // rate loss

      double power = 6800; // watts

      double p_usage = 0.0068; // approximately 10 watts per device/ connection

      Position m_pos;

      uint16_t m_load;

      bool congested;

      Ptr<Node> m_node;
};

class NrMicro : LteConfig {
  public:
    /**
        * \brief Get the type identifier.
        * \return type identifier
    */
    static TypeId GetTypeId ();

    TypeId GetInstanceTypeId (void) const;

    NrMicro (double x, double y);

    ~NrMicro ();
   private:
    double RSRP = -120   // in dBm
    
    double powerloss = 0.25;  // rate loss

    double power = 6800; // watts

    double p_usage = 0.0068; // approximately 10 watts per device/ connection

    Position m_pos;

    uint16_t load;

    bool congested;

    Ptr<Node> m_node;
};


class NrMacro : NrMicro {
  public:
    /**
     * \brief Get the type identifier.
     * \return type identifier
    */
    static TypeId GetTypeId ();

    TypeId GetInstanceTypeId (void) const;

    NrMacro (double x, double y);

    ~NrMacro ();
   private:
    double RSRP = -120;   // in dBm
    
    double powerloss = 0.25;  // rate loss

    double power = 11500; // watts

    double p_usage = 0.0115; // approximately 10 watts per device/ connection

    Position m_pos;

    uint16_t load;

    Ptr<Node> m_node;
};

class NetworkMonitor : public Object {
  public:
  /**
     * \brief Get the type identifier.
     * \return type identifier
  */
  static TypeId GetTypeId ();

  TypeId GetInstanceTypeId (void) const;
}

}

#endif