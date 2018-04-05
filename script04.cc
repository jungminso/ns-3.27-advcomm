#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/random-variable-stream.h"

#define MAX_STAS 100

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SimulationScript");

Ptr<UniformRandomVariable> m_random;
uint32_t seed;

uint16_t num_rooms_rows;
uint16_t num_rooms_cols;
uint16_t num_stas;

double   begin_time;
double   sim_time;
uint32_t payload_size;
double   grid_length;

double   pre_begin_time;
double   pre_interval;
double   pre_duration;

uint16_t mybs[MAX_STAS];

NetDeviceContainer apDevices;
NetDeviceContainer staDevices;

static void PrintProgress(void) {
  fprintf(stderr, ".");
  if(Simulator::Now() > Seconds(begin_time + sim_time - 0.1/2)) fprintf(stderr, "\n");
  Simulator::Schedule(Seconds(0.1), &PrintProgress);
}

int main(int argc, char *argv[]) {

    m_random = CreateObject<UniformRandomVariable>();
    seed           = 1;

    num_rooms_rows = 2;
    num_rooms_cols = 2;
    uint16_t num_aps = num_rooms_rows*num_rooms_cols;
    num_stas       = 10;

    begin_time     = 1000.0;
    sim_time       = 1.0;
    payload_size   = 1472;
    grid_length    = 10;

    pre_begin_time = 100.0;
    pre_interval   = 1.0;
    pre_duration   = 0.01;

	// Process command line arguments
    CommandLine cmd;
    cmd.AddValue("seed", "Random Seed", seed);
    cmd.Parse(argc, argv);

    // Set random seed
    m_random->SetStream(seed);

    // Create nodes
    NodeContainer apNodes;
    apNodes.Create(num_rooms_rows*num_rooms_cols);
    NodeContainer staNodes;
    staNodes.Create(num_stas);

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
    apDevices = wifi.Install(phy, mac, apNodes);
    staDevices = wifi.Install(phy, mac, staNodes);

	// Set node positions and mobility pattern
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> apPositionAlloc = CreateObject<ListPositionAllocator>();
    for(uint16_t i=0; i<num_rooms_rows*num_rooms_cols; i++) {
        double x = double(i%num_rooms_rows*grid_length+grid_length/2);
        double y = double(i/num_rooms_rows*grid_length+grid_length/2);
        apPositionAlloc->Add(Vector(x, y, 0.0));
        NS_LOG_UNCOND("ap position: " << x << " " << y);
    }
    mobility.SetPositionAllocator(apPositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);

    Ptr<ListPositionAllocator> staPositionAlloc = CreateObject<ListPositionAllocator>();
    for(uint16_t i=0; i<num_stas; i++) {
        double x = m_random->GetValue()*num_rooms_cols*grid_length;
        double y = m_random->GetValue()*num_rooms_rows*grid_length;
        staPositionAlloc->Add(Vector(x, y, 0.0));
        NS_LOG_UNCOND("sta position: " << x << " " << y);
    }
    mobility.SetPositionAllocator(staPositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(staNodes);

    // Select default AP for each STA
    double max_dist;
    int16_t nearest_ap;
    double dist;
    for(uint16_t i=0; i<num_stas; i++) {
        nearest_ap = -1;
        Vector sta = staPositionAlloc->GetNext();
        for(uint16_t j=0; j<num_rooms_cols*num_rooms_rows; j++) {
           Vector ap = apPositionAlloc->GetNext();
           dist = (sta.x-ap.x)*(sta.x-ap.x) + (sta.y-ap.y)*(sta.y-ap.y);
           if(nearest_ap == -1 || dist < max_dist) {
               nearest_ap = j;
               max_dist = dist;
           } 
        }
        mybs[i] = nearest_ap;
    }
    
	// Create and configure NETWORK layer (IP)
	InternetStackHelper internet;
	internet.Install(apNodes);
	internet.Install(staNodes);
	Ipv4AddressHelper ipv4;
	ipv4.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer ip_ap = ipv4.Assign(apDevices);
	Ipv4InterfaceContainer ip_sta = ipv4.Assign(staDevices);

    // Traffic for ARP activation
    for(uint16_t i=0; i<num_aps; i++) {
        UdpServerHelper myServerPre(19);
        ApplicationContainer serverAppPre = myServerPre.Install(apNodes.Get(i));
        serverAppPre.Start(Seconds(pre_begin_time));
        serverAppPre.Stop(Seconds(pre_begin_time+pre_interval*num_stas+pre_duration));
    }
    for(uint16_t j=0; j<num_stas; j++) {
        UdpClientHelper myClientPre(ip_ap.GetAddress(mybs[j]), 19);
        myClientPre.SetAttribute("MaxPackets", UintegerValue(4294967295u));
        myClientPre.SetAttribute("Interval", TimeValue(Time("0.001")));
        myClientPre.SetAttribute("PacketSize", UintegerValue(payload_size));
        ApplicationContainer clientAppPre = myClientPre.Install(staNodes.Get(j));
        clientAppPre.Start(Seconds(pre_begin_time+pre_interval*j));
        clientAppPre.Stop(Seconds(pre_begin_time+pre_interval*j+pre_duration));
    }

    // Real traffic
    std::vector<ApplicationContainer> servers;

    for(uint16_t i=0; i<num_aps; i++) {
        UdpServerHelper myServer(9);
        ApplicationContainer serverApp = myServer.Install(apNodes.Get(i));
        serverApp.Start(Seconds(begin_time));
        serverApp.Stop(Seconds(begin_time+sim_time));
        servers.push_back(serverApp);
    }
    for(uint16_t j=0; j<num_stas; j++) {
        UdpClientHelper myClient(ip_ap.GetAddress(mybs[j]), 9);
        myClient.SetAttribute("MaxPackets", UintegerValue(4294967295u));
        myClient.SetAttribute("Interval", TimeValue(Seconds(0.0002)));
        myClient.SetAttribute("PacketSize", UintegerValue(payload_size));
        ApplicationContainer clientApp = myClient.Install(staNodes.Get(j));
        clientApp.Start(Seconds(begin_time+0.001*(j)));
        clientApp.Stop(Seconds(begin_time+sim_time));
    }

    // Predefined schedules
    Simulator::Schedule(Seconds(begin_time), &PrintProgress);
    
	// Run simulation
	Simulator::Stop(Seconds(begin_time+sim_time+0.1));
	Simulator::Run();
	Simulator::Destroy();

    // Throughput calculation ----------------------------------------------------
    double total_throughput = 0.0;
    for(uint16_t i=0; i<num_aps; i++) {
        uint32_t totalPacketsThrough = DynamicCast<UdpServer>(servers[i].Get(0))->GetReceived();
        double throughput = totalPacketsThrough * payload_size * 8 / (sim_time * 1000000.0); //Mbps
        total_throughput += throughput;
        NS_LOG_UNCOND("throughput: " << throughput << "Mbps");
    }

    NS_LOG_UNCOND("total throughput: " << total_throughput << "Mbps");
	return 0;
}

	
	
	
    

