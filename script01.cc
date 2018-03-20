#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SimulationScript");

int main(int argc, char *argv[]) {

    // Process command line arguments
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // Create nodes
    NodeContainer wifiNodes;
    wifiNodes.Create(2);

    // Create and configure wireless channel
    YansWifiChannelHelper channel;
    channel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    // Create and configure PHY layer
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());
    phy.Set("ShortGuardEnabled", BooleanValue(1));
    phy.Set("TxPowerStart", DoubleValue(20.0));
    phy.Set("TxPowerEnd", DoubleValue(20.0));
    phy.Set("EnergyDetectionThreshold", DoubleValue(-96.0));
    phy.Set("CcaMode1Threshold", DoubleValue(-99.0));

    // Create and configure MAC layer
    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    // Select which WLAN standard to use
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
    std::string dataMode("OfdmRate54Mbps");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue(dataMode));

    // Create network device and install PHY and MAC
    NetDeviceContainer devices = wifi.Install(phy, mac, wifiNodes);

    // Set node positions and mobility pattern
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(20.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiNodes);

    // Create and configure NETWORK layer (IP)
    InternetStackHelper internet;
    internet.Install(wifiNodes);
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ip = ipv4.Assign(devices);

    // Create and configure TRANSPORT layer and APPLICATION layer (traffic)
    // server socket
    UdpServerHelper myServer(9);
    // server application
    ApplicationContainer serverApp = myServer.Install(wifiNodes.Get(0));
    serverApp.Start(Seconds(60.0));
    serverApp.Stop(Seconds(61.0));
    // client socket
    UdpClientHelper myClient(ip.GetAddress(0), 9);
    myClient.SetAttribute("MaxPackets", UintegerValue(4294967295u));
    myClient.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
    myClient.SetAttribute("PacketSize", UintegerValue(1472));
    // client application
    ApplicationContainer clientApp = myClient.Install(wifiNodes.Get(1));
    clientApp.Start(Seconds(60.0));
    clientApp.Stop(Seconds(61.0));

    // Run simulation
    Simulator::Stop(Seconds(61.1));
    Simulator::Run();
    Simulator::Destroy();

    // Calculate throughput
    uint32_t totalPacketsReceived = DynamicCast<UdpServer>(serverApp.Get(0))->GetReceived();
    double throughput = totalPacketsReceived * 1472 * 8 / (1.0 * 1000000.0);

    // Print throughput
    NS_LOG_UNCOND("throughput: " << throughput << "Mbps");
    return 0;
}

