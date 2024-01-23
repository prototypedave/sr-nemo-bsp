#ifndef SEGMENT_H
#define SEGMENT_H

#include "ns3/address.h"
#include "ns3/attribute-helper.h"

#include <ostream>
#include <stdint.h>

namespace ns3
{
class Segment
  {
  public:
    /**
     * \brief Default constructor.
    */
    Segment();
    /*
    */
    Segment(const char* sid);
    /*
    */
    Segment(const Segment& sid);
    /*
    */
    Segment(const Segment* sid);
    /*
    */
    ~Segment();
    /*
    */
    void Set(const char* sid);
    /*
    */
    Segment(uint8_t sid[16]);
    /*
    */
    void Serialize(uint8_t buf[16]) const;
    /*
    */
    static Segment Deserialize(const uint8_t buf[16]);
    private:
      uint8_t m_address[16];
      bool m_initialized;
  private:
    private:
    /**
     * \brief Return the Type of address.
     * \return type of address
     */
    static uint8_t GetType();

  };
}

#endif