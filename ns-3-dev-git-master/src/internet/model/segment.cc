#include "segment.h"

#include "ns3/mac16-address.h"
#include "ns3/mac48-address.h"
#include "ns3/mac64-address.h"

#include "ns3/log.h"
#include "ns3/assert.h"

#include <iomanip>
#include <memory>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Segment");

Segment::Segment() {
    NS_LOG_FUNCTION(this);
    memset(m_address, 0x00, 16);
    m_initialized = false;
}

Segment::Segment(const Segment& sid) {
    memcpy(m_address, sid.m_address, 16);
    m_initialized = true;
}

Segment::Segment(const Segment* sid) {
    memcpy(m_address, sid->m_address, 16);
    m_initialized = true;
}

Segment::Segment(uint8_t sid[16])
{
    NS_LOG_FUNCTION(this << &sid);
    /* 128 bit => 16 bytes */
    memcpy(m_address, sid, 16);
    m_initialized = true;
}

Segment::Segment(const char* sid) {
    NS_LOG_FUNCTION(this << sid);
    m_initialized = true;
}

Segment::~Segment () {

}

void
Segment::Set (const char* sid) {
    NS_LOG_FUNCTION(this << sid);
    m_initialized = true;
}

void
Segment::Serialize(uint8_t buf[16]) const {
    NS_LOG_FUNCTION(this << &buf);
    memcpy(buf, m_address, 8);
}

Segment 
Segment::Deserialize(const uint8_t buf[16]) {
    NS_LOG_FUNCTION(&buf);
    Segment sid((uint8_t*)buf);
    sid.m_initialized = true;
    return sid;
}

uint8_t
Segment::GetType()
{
    NS_LOG_FUNCTION_NOARGS();
    static uint8_t type = Address::Register();
    return type;
}
}