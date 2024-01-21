#include "sr-agent.h"
#include "ns3/object-factory.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include <fstream>

namespace ns3 {
class LteConfig : public Object{
private:
    std::string bs_id;
    uint32_t dfb;   // devices connected in default bearers
    uint32_t ddb;   // devices connected in dedicated bearer
    float bitrate = 100;  // max of 100MBps for 4G
    float downlink = 8.5;
    float uplink = 3.5;
public:
LteConfig ()
{   
}

~LteConfig ()
{   
}

static TypeId
GetTypeId (void)
{
static TypeId tid = TypeId ("ns3::LteConfig")
    .SetParent<Object> ()
    .AddConstructor<LteConfig> ();
    return tid;
}

TypeId
GetInstanceTypeId (void) const
{
    return GetTypeId ();
}

void
SetDefaultBearerDevices(uint32_t BE)
{
    dfb = BE;
}

uint32_t
GetDefaultBearerDevices()
{
    return dfb;
}

void
SetDedicatedBearers(uint32_t OD)
{
    ddb = OD;
}

uint32_t
GetDedicatedBearerDevices()
{
    return ddb;
}

float
CalculateNetworkRate()
{
    // for best effort we assume an uplink
    float be_rate = dfb * uplink;
    float od_rate = ddb * downlink;
    float rate = be_rate + od_rate;
    return rate;
}

bool
CheckSignalStrength(std::string bearer)
{
    uint16_t points = 0;
    bool check = false;
    // check for congestion
    if (ddb > dfb)
    {
        points++;
    }

    // check for possible latency
    float rate = CalculateNetworkRate();
    if (rate >= 0.8 * bitrate)
    {
        points++;
    }

    if (bearer == "od")
    {
        points++;
    }

    if (points >= 2)
    {
        check = true;
    }
    return check;
};

};
}