/*
2. Implement transmission of ping messages/trace route over a network topology
consisting of 6 nodes and find the number of packets dropped due to congestion.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CsmaPingExample");

static void PingRtt(std::string context, Time rtt)
{
    std::cout << context << " " << rtt << std::endl;
}

int main(int argc, char *argv[])
{
    std::string socketType = "ns3::UdpSocketFactory";

    CommandLine cmd;
    cmd.Parse(argc, argv);

    NodeContainer nodes;
    nodes.Create(6);

    InternetStackHelper stack;
    stack.Install(nodes);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(DataRate(10000)));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(0.2)));
    
    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");

    NetDeviceContainer devices;
    devices = csma.Install(nodes);
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    uint16_t port = 9;

    OnOffHelper onoff(socketType, Address(InetSocketAddress(interfaces.GetAddress(2), port)));
    onoff.SetConstantRate(DataRate("500Mbps"));

    ApplicationContainer app = onoff.Install(nodes.Get(0));
    app.Start(Seconds(6.0));
    app.Stop(Seconds(10.0));

    PacketSinkHelper sink(socketType, Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    app = sink.Install(nodes.Get(2));
    app.Start(Seconds(0.0));

    V4PingHelper ping = V4PingHelper(interfaces.GetAddress(2));

    NodeContainer pingers;
    pingers.Add(nodes.Get(0));
    pingers.Add(nodes.Get(1));

    ApplicationContainer apps = ping.Install(pingers);
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(5.0));

    Config::Connect("/NodeList/*/ApplicationList/*/$ns3::V4Ping/Rtt", MakeCallback(&PingRtt));

    AsciiTraceHelper ascii;
    csma.EnableAsciiAll(ascii.CreateFileStream("ping1.tr"));

    Simulator::Run();
    Simulator::Destroy();
}