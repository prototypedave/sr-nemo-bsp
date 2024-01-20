/*
 * Copyright (c) 2022 Chandrakant Jena
 * Copyright (c) 2007-2009 Strasbourg University (original Ping6 code)
 * Copyright (c) 2008 INRIA (original Ping code)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Chandrakant Jena <chandrakant.barcelona@gmail.com>
 *         Tommaso Pecorella <tommaso.pecorella@unifi.it>
 *
 * Derived from original v4Ping application (author: Mathieu Lacage)
 * Derived from original ping6 application (author: Sebastien Vincent)
 */

#include "ping.h"

#include "ns3/abort.h"
#include "ns3/assert.h"
#include "ns3/boolean.h"
#include "ns3/enum.h"
#include "ns3/icmpv4.h"
#include "ns3/icmpv6-header.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-extension-header.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-packet-info-tag.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/socket.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Ping");

NS_OBJECT_ENSURE_REGISTERED(Ping);

/// This value is used to quickly identify ECHO packets generated by this app.
constexpr uint16_t PING_ID{0xbeef};

TypeId
Ping::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::Ping")
            .SetParent<Application>()
            .SetGroupName("Internet-Apps")
            .AddConstructor<Ping>()
            .AddAttribute("Destination",
                          "The unicast IPv4 or IPv6 address of the machine we want to ping",
                          AddressValue(),
                          MakeAddressAccessor(&Ping::m_destination),
                          MakeAddressChecker())
            .AddAttribute("VerboseMode",
                          "Configure verbose, quiet, or silent output",
                          EnumValue(VerboseMode::VERBOSE),
                          MakeEnumAccessor(&Ping::m_verbose),
                          MakeEnumChecker(VerboseMode::VERBOSE,
                                          "Verbose",
                                          VerboseMode::QUIET,
                                          "Quiet",
                                          VerboseMode::SILENT,
                                          "Silent"))
            .AddAttribute("Interval",
                          "Time interval between sending each packet",
                          TimeValue(Seconds(1)),
                          MakeTimeAccessor(&Ping::m_interval),
                          MakeTimeChecker())
            .AddAttribute(
                "Size",
                "The number of data bytes to be sent, before ICMP and IP headers are added",
                UintegerValue(56),
                MakeUintegerAccessor(&Ping::m_size),
                MakeUintegerChecker<uint32_t>(16))
            .AddAttribute(
                "Count",
                "The maximum number of packets the application will send (zero means no limits)",
                UintegerValue(0),
                MakeUintegerAccessor(&Ping::m_count),
                MakeUintegerChecker<uint32_t>())
            .AddAttribute("InterfaceAddress",
                          "Local address of the sender",
                          AddressValue(),
                          MakeAddressAccessor(&Ping::m_interfaceAddress),
                          MakeAddressChecker())
            .AddAttribute("Timeout",
                          "Time to wait for a response if no RTT samples are available",
                          TimeValue(Seconds(1)),
                          MakeTimeAccessor(&Ping::m_timeout),
                          MakeTimeChecker())
            .AddAttribute("Tos",
                          "The Type of Service used to send the ICMP Echo Requests. "
                          "All 8 bits of the TOS byte are set (including ECN bits).",
                          UintegerValue(0),
                          MakeUintegerAccessor(&Ping::m_tos),
                          MakeUintegerChecker<uint8_t>())
            .AddTraceSource("Tx",
                            "The sequence number and ICMP echo response packet.",
                            MakeTraceSourceAccessor(&Ping::m_txTrace),
                            "ns3::Ping::TxTrace")
            .AddTraceSource("Rtt",
                            "The sequence number and RTT sample.",
                            MakeTraceSourceAccessor(&Ping::m_rttTrace),
                            "ns3::Ping::RttTrace")
            .AddTraceSource("Drop",
                            "Drop events due to destination unreachable or other errors.",
                            MakeTraceSourceAccessor(&Ping::m_dropTrace),
                            "ns3::Ping::DropTrace")
            .AddTraceSource("Report",
                            "Summary report at close of application.",
                            MakeTraceSourceAccessor(&Ping::m_reportTrace),
                            "ns3::Ping::ReportTrace");
    return tid;
}

Ping::Ping()
{
    NS_LOG_FUNCTION(this);
}

Ping::~Ping()
{
    NS_LOG_FUNCTION(this);
}

void
Ping::DoDispose()
{
    NS_LOG_FUNCTION(this);
    StopApplication();
    m_socket = nullptr;
    Application::DoDispose();
}

uint64_t
Ping::GetApplicationSignature() const
{
    NS_LOG_FUNCTION(this);

    uint64_t appSignature = GetNode()->GetId();
    appSignature <<= 32;

    Ptr<Node> node = GetNode();
    for (uint32_t i = 0; i < node->GetNApplications(); ++i)
    {
        if (node->GetApplication(i) == this)
        {
            appSignature += i;
            return appSignature;
        }
    }
    NS_ASSERT_MSG(false, "forgot to add application to node");
    return 0; // quiet compiler
}

void
Ping::Receive(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);

    while (m_socket->GetRxAvailable() > 0)
    {
        Address from;
        Ptr<Packet> packet = m_socket->RecvFrom(from);
        uint32_t recvSize = packet->GetSize();
        NS_LOG_DEBUG("recv " << recvSize << " bytes " << *packet);

        if (InetSocketAddress::IsMatchingType(from))
        {
            InetSocketAddress realFrom = InetSocketAddress::ConvertFrom(from);
            Ipv4Header ipv4Hdr;
            packet->RemoveHeader(ipv4Hdr);
            recvSize -= ipv4Hdr.GetSerializedSize();
            Icmpv4Header icmp;
            packet->RemoveHeader(icmp);

            switch (icmp.GetType())
            {
            case Icmpv4Header::ICMPV4_ECHO_REPLY: {
                Icmpv4Echo echo;
                packet->RemoveHeader(echo);

                if (echo.GetIdentifier() != PING_ID)
                {
                    return;
                }

                NS_LOG_INFO("Received Echo Reply size  = "
                            << std::dec << recvSize << " bytes from " << realFrom.GetIpv4()
                            << " id =  " << echo.GetIdentifier()
                            << " seq = " << echo.GetSequenceNumber()
                            << " TTL = " << static_cast<uint16_t>(ipv4Hdr.GetTtl()));

                uint32_t dataSize = echo.GetDataSize();
                if (dataSize < 8)
                {
                    NS_LOG_INFO("Packet too short, discarding");
                    return;
                }
                auto buf = new uint8_t[dataSize];
                echo.GetData(buf);
                uint64_t appSignature = Read64(buf);
                delete[] buf;

                if (appSignature == m_appSignature)
                {
                    Time sendTime = m_sent.at(echo.GetSequenceNumber()).txTime;
                    NS_ASSERT(Simulator::Now() >= sendTime);
                    Time delta = Simulator::Now() - sendTime;

                    bool dupReply = false;
                    if (m_sent.at(echo.GetSequenceNumber()).acked)
                    {
                        m_duplicate++;
                        dupReply = true;
                    }
                    else
                    {
                        m_recv++;
                        m_sent.at(echo.GetSequenceNumber()).acked = true;
                    }

                    m_avgRtt.Update(delta.GetMilliSeconds());
                    m_rttTrace(echo.GetSequenceNumber(), delta);

                    if (m_verbose == VerboseMode::VERBOSE)
                    {
                        std::cout << recvSize << " bytes from " << realFrom.GetIpv4() << ":"
                                  << " icmp_seq=" << echo.GetSequenceNumber()
                                  << " ttl=" << static_cast<uint16_t>(ipv4Hdr.GetTtl())
                                  << " time=" << delta.GetMicroSeconds() / 1000.0 << " ms";
                        if (dupReply && !m_multipleDestinations)
                        {
                            std::cout << " (DUP!)";
                        }
                        std::cout << "\n";
                    }
                }

                break;
            }
            case Icmpv4Header::ICMPV4_DEST_UNREACH: {
                Icmpv4DestinationUnreachable destUnreach;
                packet->RemoveHeader(destUnreach);

                NS_LOG_INFO("Received Destination Unreachable from " << realFrom.GetIpv4());
                break;
            }
            case Icmpv4Header::ICMPV4_TIME_EXCEEDED: {
                Icmpv4TimeExceeded timeExceeded;
                packet->RemoveHeader(timeExceeded);

                NS_LOG_INFO("Received Time Exceeded from " << realFrom.GetIpv4());
                break;
            }
            default:
                break;
            }
        }
        else if (Inet6SocketAddress::IsMatchingType(from))
        {
            Inet6SocketAddress realFrom = Inet6SocketAddress::ConvertFrom(from);
            Ipv6Header ipv6Hdr;

            // We need the IP interface index.
            Ipv6PacketInfoTag infoTag;
            packet->RemovePacketTag(infoTag);
            Ptr<Ipv6L3Protocol> ipv6 = m_node->GetObject<Ipv6L3Protocol>();
            Ipv6Address myAddr = infoTag.GetAddress();
            int32_t ipIfIndex = ipv6->GetInterfaceForAddress(myAddr);

            packet->RemoveHeader(ipv6Hdr);
            recvSize -= ipv6Hdr.GetSerializedSize();
            uint8_t type;
            packet->CopyData(&type, sizeof(type));

            switch (type)
            {
            case Icmpv6Header::ICMPV6_ECHO_REPLY: {
                Icmpv6Echo echo(false);
                packet->RemoveHeader(echo);

                if (echo.GetId() != PING_ID)
                {
                    return;
                }

                NS_LOG_INFO("Received Echo Reply size  = "
                            << std::dec << recvSize << " bytes from " << realFrom.GetIpv6()
                            << " id =  " << echo.GetId() << " seq = " << echo.GetSeq()
                            << " Hop Count = " << static_cast<uint16_t>(ipv6Hdr.GetHopLimit()));

                uint32_t dataSize = packet->GetSize();
                if (dataSize < 8)
                {
                    NS_LOG_INFO("Packet too short, discarding");
                    return;
                }
                auto buf = new uint8_t[dataSize];
                packet->CopyData(buf, dataSize);
                uint64_t appSignature = Read64(buf);
                delete[] buf;

                if (appSignature == m_appSignature)
                {
                    Time sendTime = m_sent.at(echo.GetSeq()).txTime;
                    NS_ASSERT(Simulator::Now() >= sendTime);
                    Time delta = Simulator::Now() - sendTime;

                    bool dupReply = false;
                    if (m_sent.at(echo.GetSeq()).acked)
                    {
                        m_duplicate++;
                        dupReply = true;
                    }
                    else
                    {
                        m_recv++;
                        m_sent.at(echo.GetSeq()).acked = true;
                    }

                    m_avgRtt.Update(delta.GetMilliSeconds());
                    m_rttTrace(echo.GetSeq(), delta);

                    if (m_verbose == VerboseMode::VERBOSE)
                    {
                        std::cout << recvSize << " bytes from (" << realFrom.GetIpv6() << "):"
                                  << " icmp_seq=" << echo.GetSeq()
                                  << " ttl=" << static_cast<uint16_t>(ipv6Hdr.GetHopLimit())
                                  << " time=" << delta.GetMicroSeconds() / 1000.0 << " ms";
                        if (dupReply)
                        {
                            std::cout << " (DUP!)";
                        }
                        std::cout << "\n";
                    }
                }
                ipv6->ReachabilityHint(ipIfIndex, realFrom.GetIpv6());
                break;
            }
            case Icmpv6Header::ICMPV6_ERROR_DESTINATION_UNREACHABLE: {
                Icmpv6DestinationUnreachable destUnreach;
                packet->RemoveHeader(destUnreach);

                NS_LOG_INFO("Received Destination Unreachable from " << realFrom.GetIpv6());
                break;
            }
            case Icmpv6Header::ICMPV6_ERROR_TIME_EXCEEDED: {
                Icmpv6TimeExceeded timeExceeded;
                packet->RemoveHeader(timeExceeded);

                NS_LOG_INFO("Received Time Exceeded from " << realFrom.GetIpv6());
                break;
            }
            default:
                break;
            }
        }
    }

    if (!m_multipleDestinations && m_count > 0 && m_recv == m_count)
    {
        Simulator::ScheduleNow(&Ping::StopApplication, this);
    }
}

// Writes data to buffer in little-endian format; least significant byte
// of data is at lowest buffer address
void
Ping::Write64(uint8_t* buffer, const uint64_t data)
{
    NS_LOG_FUNCTION(this << (void*)buffer << data);
    buffer[0] = (data >> 0) & 0xff;
    buffer[1] = (data >> 8) & 0xff;
    buffer[2] = (data >> 16) & 0xff;
    buffer[3] = (data >> 24) & 0xff;
    buffer[4] = (data >> 32) & 0xff;
    buffer[5] = (data >> 40) & 0xff;
    buffer[6] = (data >> 48) & 0xff;
    buffer[7] = (data >> 56) & 0xff;
}

// Read data from a little-endian formatted buffer to data
uint64_t
Ping::Read64(const uint8_t* buffer)
{
    NS_LOG_FUNCTION(this << (void*)buffer);
    uint64_t data = buffer[7];
    data <<= 8;
    data |= buffer[6];
    data <<= 8;
    data |= buffer[5];
    data <<= 8;
    data |= buffer[4];
    data <<= 8;
    data |= buffer[3];
    data <<= 8;
    data |= buffer[2];
    data <<= 8;
    data |= buffer[1];
    data <<= 8;
    data |= buffer[0];

    return data;
}

void
Ping::Send()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("m_seq=" << m_seq);

    // Prepare the payload.
    // We must write quantities out in some form of network order.  Since there
    // isn't an htonl to work with we just follow the convention in pcap traces
    // (where any difference would show up anyway) and borrow that code.  Don't
    // be too surprised when you see that this is a little endian convention.
    //
    auto data = new uint8_t[m_size];
    memset(data, 0, m_size);
    NS_ASSERT_MSG(m_size >= 16, "ECHO Payload size must be at least 16 bytes");

    Write64(data, m_appSignature);
    Ptr<Packet> dataPacket = Create<Packet>(data, m_size);

    Ptr<Packet> p = Create<Packet>();
    int returnValue = 0;

    if (!m_useIpv6)
    {
        // Using IPv4
        Icmpv4Echo echo;
        echo.SetSequenceNumber(m_seq);
        echo.SetIdentifier(PING_ID);

        // In the Icmpv4Echo the payload is part of the header.
        echo.SetData(dataPacket);

        p->AddHeader(echo);
        Icmpv4Header header;
        header.SetType(Icmpv4Header::ICMPV4_ECHO);
        header.SetCode(0);
        if (Node::ChecksumEnabled())
        {
            header.EnableChecksum();
        }
        p->AddHeader(header);
        auto dest = InetSocketAddress(Ipv4Address::ConvertFrom(m_destination), 0);
        dest.SetTos(m_tos);
        returnValue = m_socket->SendTo(p, 0, dest);
    }
    else
    {
        // Using IPv6
        Icmpv6Echo echo(true);

        echo.SetSeq(m_seq);
        echo.SetId(PING_ID);

        // In the Icmpv6Echo the payload is just the content of the packet.
        p = dataPacket->Copy();

        p->AddHeader(echo);

        /* use Loose Routing (routing type 0) */
        if (!m_routers.empty())
        {
            Ipv6ExtensionLooseRoutingHeader routingHeader;
            routingHeader.SetNextHeader(Ipv6Header::IPV6_ICMPV6);
            routingHeader.SetTypeRouting(0);
            routingHeader.SetSegmentsLeft(m_routers.size());
            routingHeader.SetRoutersAddress(m_routers);
            p->AddHeader(routingHeader);
            m_socket->SetAttribute("Protocol", UintegerValue(Ipv6Header::IPV6_EXT_ROUTING));
        }

        returnValue =
            m_socket->SendTo(p, 0, Inet6SocketAddress(Ipv6Address::ConvertFrom(m_destination), 0));

        // Loose routing could have changed (temporarily) the protocol. Set it again to receive the
        // replies.
        m_socket->SetAttribute("Protocol", UintegerValue(Ipv6Header::IPV6_ICMPV6));
    }
    if (returnValue > 0)
    {
        m_sent.emplace_back(Simulator::Now(), false);
        m_txTrace(m_seq, p);
    }
    else
    {
        NS_LOG_INFO("Send failure; socket return value: " << returnValue);
    }
    m_seq++;
    delete[] data;

    if (m_count == 0 || m_seq < m_count)
    {
        m_next = Simulator::Schedule(m_interval, &Ping::Send, this);
    }

    // We have sent all the requests. Schedule a shutdown after the linger time
    if (m_count > 0 && m_seq == m_count)
    {
        Time lingerTime = m_avgRtt.Count() > 0 ? MilliSeconds(2 * m_avgRtt.Max()) : m_timeout;
        Simulator::Schedule(lingerTime, &Ping::StopApplication, this);
    }
}

void
Ping::StartApplication()
{
    NS_LOG_FUNCTION(this);

    if (m_destination.IsInvalid())
    {
        NS_ABORT_MSG("Destination Address value must be set when starting application");
    }

    m_appSignature = GetApplicationSignature();

    m_started = Simulator::Now();
    m_reportPrinted = false;
    if (m_verbose == VerboseMode::VERBOSE || m_verbose == VerboseMode::QUIET)
    {
        if (Ipv4Address::IsMatchingType(m_destination))
        {
            InetSocketAddress realFrom = Ipv4Address::ConvertFrom(m_destination);
            std::cout << "PING " << realFrom.GetIpv4() << " - " << m_size << " bytes of data; "
                      << m_size + 28 << " bytes including ICMP and IPv4 headers.\n";
        }
        else if (Ipv6Address::IsMatchingType(m_destination))
        {
            Inet6SocketAddress realFrom = Ipv6Address::ConvertFrom(m_destination);
            std::cout << "PING " << realFrom.GetIpv6() << " - " << m_size << " bytes of data; "
                      << m_size + 48 << " bytes including ICMP and IPv6 headers.\n";
        }
        else
        {
            NS_ABORT_MSG("Invalid Address");
        }
    }

    if (Ipv4Address::IsMatchingType(m_destination))
    {
        m_socket =
            Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::Ipv4RawSocketFactory"));
        NS_ASSERT_MSG(m_socket, "Ping::StartApplication: can not create socket.");
        m_socket->SetAttribute("Protocol", UintegerValue(1)); // icmp
        m_socket->SetRecvCallback(MakeCallback(&Ping::Receive, this));
        m_useIpv6 = false;

        Ipv4Address dst = Ipv4Address::ConvertFrom(m_destination);
        m_multipleDestinations = dst.IsMulticast() || dst.IsBroadcast();
    }
    else if (Ipv6Address::IsMatchingType(m_destination))
    {
        m_socket =
            Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::Ipv6RawSocketFactory"));
        NS_ASSERT_MSG(m_socket, "Ping::StartApplication: can not create socket.");
        m_socket->SetAttribute("Protocol", UintegerValue(Ipv6Header::IPV6_ICMPV6));
        m_socket->SetRecvCallback(MakeCallback(&Ping::Receive, this));
        m_socket->SetRecvPktInfo(true);
        m_useIpv6 = true;

        Ipv6Address dst = Ipv6Address::ConvertFrom(m_destination);
        m_multipleDestinations = dst.IsMulticast();
    }
    else
    {
        NS_ABORT_MSG("Destination Address value must be of type Ipv4 or Ipv6");
    }

    if (!m_interfaceAddress.IsInvalid())
    {
        if (Ipv4Address::IsMatchingType(m_interfaceAddress))
        {
            InetSocketAddress senderInet(Ipv4Address::ConvertFrom(m_interfaceAddress));
            int status = m_socket->Bind(senderInet);
            NS_ASSERT_MSG(status == 0, "Failed to bind IPv4 socket");
        }
        else if (Ipv6Address::IsMatchingType(m_interfaceAddress))
        {
            Inet6SocketAddress senderInet =
                Inet6SocketAddress(Ipv6Address::ConvertFrom(m_interfaceAddress));
            int status = m_socket->Bind(senderInet);
            NS_ASSERT_MSG(status == 0, "Failed to bind IPv6 socket");
        }
        else
        {
            NS_ABORT_MSG("Sender Address value must be of type Ipv4 or Ipv6");
        }
    }

    // Guess how large should be the data storage and pre-book it.
    if (m_count == 0)
    {
        Time delta = m_stopTime - Now();
        int64_t guessedTx = Div(delta, m_interval) + 1;
        m_sent.reserve(guessedTx);
    }
    else
    {
        m_sent.reserve(m_count);
    }

    Send();
}

void
Ping::StopApplication()
{
    NS_LOG_FUNCTION(this);
    if (m_stopEvent.IsRunning())
    {
        m_stopEvent.Cancel();
    }
    if (m_next.IsRunning())
    {
        m_next.Cancel();
    }
    if (m_socket)
    {
        m_socket->Close();
    }
    PrintReport();
}

void
Ping::PrintReport()
{
    if (m_reportPrinted)
    {
        return;
    }
    m_reportPrinted = true;

    if (m_verbose == VerboseMode::VERBOSE || m_verbose == VerboseMode::QUIET)
    {
        std::ostringstream os;
        os.precision(4);
        if (Ipv4Address::IsMatchingType(m_destination))
        {
            InetSocketAddress realFrom = Ipv4Address::ConvertFrom(m_destination);
            os << "\n--- " << realFrom.GetIpv4() << " ping statistics ---\n";
        }
        else if (Ipv6Address::IsMatchingType(m_destination))
        {
            Inet6SocketAddress realFrom = Ipv6Address::ConvertFrom(m_destination);
            os << "\n--- " << realFrom.GetIpv6() << " ping statistics ---\n";
        }
        os << m_seq << " packets transmitted, " << m_recv << " received, ";
        if (m_duplicate)
        {
            os << m_duplicate << " duplicates, ";
        }

        // note: integer math to match Linux implementation and avoid turning a 99.9% into a 100%.
        os << ((m_seq - m_recv) * 100 / m_seq) << "% packet loss, "
           << "time " << (Simulator::Now() - m_started).GetMilliSeconds() << "ms\n";

        if (m_avgRtt.Count() > 0)
        {
            os << "rtt min/avg/max/mdev = " << m_avgRtt.Min() << "/" << m_avgRtt.Avg() << "/"
               << m_avgRtt.Max() << "/" << m_avgRtt.Stddev() << " ms\n";
        }
        std::cout << os.str();
    }
    PingReport report;
    report.m_transmitted = m_seq;
    report.m_received = m_recv;
    // note: integer math to match Linux implementation and avoid turning a 99.9% into a 100%.
    report.m_loss = (m_seq - m_recv) * 100 / m_seq;
    report.m_duration = (Simulator::Now() - m_started);
    report.m_rttMin = m_avgRtt.Min();
    report.m_rttAvg = m_avgRtt.Avg();
    report.m_rttMax = m_avgRtt.Max();
    report.m_rttMdev = m_avgRtt.Stddev();
    m_reportTrace(report);
}

void
Ping::SetRouters(const std::vector<Ipv6Address>& routers)
{
    m_routers = routers;
}

} // namespace ns3
