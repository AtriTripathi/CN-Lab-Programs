#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include "ns3/mobile-application-helper.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("EpcFirstExample");

int main(int argc, char *argv[])
{
    uint16_t numberOfNodes = 2;
    double simTime = 1.1;
    double distance = 60.0;
    double interPacketInterval = 100;

    CommandLine cmd;
    cmd.Parse(argc, argv);

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults();

    cmd.Parse(argc, argv);
    Ptr<Node> pgw = epcHelper->GetPgwNode();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);

    InternetStackHelper stack;
    stack.Install(remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2p.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2p.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));

    NetDeviceContainer devices = p2p.Install(pgw, remoteHost);

    Ipv4AddressHelper address;
    address.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // interface 0 is localhost, 1 is the p2p device
    Ipv4Address remoteHostAddr = interfaces.GetAddress(1);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    NodeContainer ueNodes;
    NodeContainer enbNodes;
    enbNodes.Create(numberOfNodes);
    ueNodes.Create(numberOfNodes);

    // Install Mobility Model
    MobileApplicationHelper mobileApplicatonHelper(enbNodes, ueNodes, numberOfNodes);
    mobileApplicatonHelper.SetupMobilityModule(distance);

    // Install LTE Devices to the nodes
    mobileApplicatonHelper.SetupDevices(lteHelper, epcHelper, ipv4RoutingHelper);

    // Install and start applications on UEs and remote host
    uint16_t dlPort = 1234;
    uint16_t ulPort = 2000;
    uint16_t otherPort = 3000;
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;

    mobileApplicatonHelper.SetupApplications(serverApps, clientApps, remoteHost, remoteHostAddr, ulPort, dlPort, otherPort, interPacketInterval);

    serverApps.Start(Seconds(0.01));
    clientApps.Start(Seconds(0.01));
    clientApps.Stop(Seconds(8));
    lteHelper->EnableTraces();

    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll(ascii.CreateFileStream("cdma.tr"));
    p2p.EnablePcapAll("lena-epc-first");

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
