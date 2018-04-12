#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/flow-monitor-module.h"

#define MAX_STAS 100

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SimulationScript");

struct StatElement {
    uint32_t tx_bytes;
    uint32_t rx_bytes;
    uint32_t tx_packets;
    uint32_t rx_packets;
    double   duration;

    double   throughput;
    double   delay;
};

std::vector<StatElement> stat_vector;

Ptr<UniformRandomVariable> m_random;
uint32_t seed;

uint16_t num_rooms_rows;
uint16_t num_rooms_cols;
uint16_t num_aps;
uint16_t num_stas;

double   begin_time;
double   sim_time;
uint32_t payload_size;
double   grid_length;

double   pre_begin_time;
double   pre_interval;
double   pre_duration;

uint16_t mybs[MAX_STAS];
uint16_t latex_out;

NetDeviceContainer apDevices;
NetDeviceContainer staDevices;

static void PrintProgress(void) {
    fprintf(stderr, ".");
    if(Simulator::Now() > Seconds(begin_time + sim_time - 0.1/2)) fprintf(stderr, "\n");
    Simulator::Schedule(Seconds(0.1), &PrintProgress);
}

static void ExportTopologyToLatex(Ptr<ListPositionAllocator> aps, Ptr<ListPositionAllocator> stas) {

    NS_LOG_UNCOND("Exporting topology to a tex file...");

    int topo_x = 500;
    int topo_y = 500;
    int area_x = grid_length * num_rooms_cols;
    int area_y = grid_length * num_rooms_rows;
    double scale_x = (double)topo_x / area_x;
    double scale_y = (double)topo_y / area_y;

    FILE *outfile;
    char outfilename[200];
    strcpy(outfilename, "topology.tex");
    outfile = fopen(outfilename, "w");

    fprintf(outfile, "\\documentclass{article}\n");
    fprintf(outfile, "\\usepackage{epsfig}\n");
    fprintf(outfile, "\\usepackage{color}\n");
    fprintf(outfile, "\\usepackage{epic}\n");
    fprintf(outfile, "\\usepackage{nopageno}\n");
    fprintf(outfile, "\\definecolor{black}{rgb}{0,0,0}\n");
    fprintf(outfile, "\\definecolor{red}{rgb}{1,0,0}\n");
    fprintf(outfile, "\\definecolor{gray}{rgb}{0.5,0.5,0.5}\n");
    fprintf(outfile, "\\definecolor{darkgray}{rgb}{0.25,0.25,0.25}\n");
    fprintf(outfile, "\\definecolor{green}{rgb}{0,1,0}\n");
    fprintf(outfile, "\\definecolor{blue}{rgb}{0,0,1}\n");
    fprintf(outfile, "\\begin{document}\n");
    fprintf(outfile, "\\setlength{\\unitlength}{0.2mm}\n");

    fprintf(outfile, "\\begin{picture}(500,500)\n");
    fprintf(outfile, "\\thicklines\n");
    fprintf(outfile, "\\put(0,0){\\line(1,0){500}}\n");
    fprintf(outfile, "\\put(500,0){\\line(0,1){500}}\n");
    fprintf(outfile, "\\put(500,500){\\line(-1,0){500}}\n");
    fprintf(outfile, "\\put(0,500){\\line(0,-1){500}}\n");

    // draw rooms
    for(uint16_t i=1; i<num_rooms_cols; i++) {
        int pos = (int)(i*(topo_x/num_rooms_cols));
        fprintf(outfile, "\\put(%03d,0){\\line(0,1){500}}\n", pos);
    }

    for(uint16_t i=1; i<num_rooms_rows; i++) {
        int pos = (int)(i*(topo_y/num_rooms_rows));
        fprintf(outfile, "\\put(0,%03d){\\line(1,0){500}}\n", pos);
    }

    for(uint16_t i=0; i<num_aps; i++) {
        Vector pos = aps->GetNext();
        fprintf(outfile, "\\thinlines\\color{blue}\\put(%3d,%3d){\\circle*{10}}\n", (int)(pos.x*scale_x), (int)(pos.y*scale_y));
        fprintf(outfile, "\\thicklines\\color{blue}\\put(%3d,%3d){\\small %d}\n", (int)(pos.x*scale_x)+5, (int)(pos.y*scale_y)+5, i+1);
    }

    for(uint16_t i=0; i<num_stas; i++) {
        Vector pos = stas->GetNext();
        fprintf(outfile, "\\thinlines\\color{black}\\put(%3d,%3d){\\circle*{10}}\n", (int)(pos.x*scale_x), (int)(pos.y*scale_y));
        fprintf(outfile, "\\thicklines\\color{black}\\put(%3d,%3d){\\small %d}\n", (int)(pos.x*scale_x)+5, (int)(pos.y*scale_y)+5, i+1+num_aps);
    }

    fprintf(outfile, "\\end{picture}\n");
    fprintf(outfile, "\\end{document}\n");

    fclose(outfile);

    system("rm -f topology.dvi topology.ps topology.pdf");
    system("latex topology.tex > /dev/null 2>&1");
    system("dvips topology.dvi > /dev/null 2>&1");
    system("ps2pdf topology.ps");
    fprintf(stderr, "topology.pdf generated\n");
}

int main(int argc, char *argv[]) {

    m_random = CreateObject<UniformRandomVariable>();
    seed           = 1;

    num_rooms_rows = 3;
    num_rooms_cols = 3;
    num_aps = num_rooms_rows*num_rooms_cols;
    num_stas       = 10;
    grid_length    = 20;

    begin_time     = 1000.0;
    sim_time       = 1.0;
    payload_size   = 1472;

    pre_begin_time = 100.0;
    pre_interval   = 1.0;
    pre_duration   = 0.01;

    latex_out      = 0;

	// Process command line arguments
    CommandLine cmd;
    cmd.AddValue("cols", "Number of columns in the grid", num_rooms_cols);
    cmd.AddValue("rows", "Number of rows in the grid", num_rooms_rows);
    cmd.AddValue("stas", "Number of mobile stations", num_stas);
    cmd.AddValue("size", "Grid size", grid_length);
    cmd.AddValue("seed", "Random seed", seed);
    cmd.AddValue("latex", "Output topology to a tex file", latex_out);
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
        //NS_LOG_UNCOND("ap position: " << x << " " << y);
    }
    mobility.SetPositionAllocator(apPositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);

    Ptr<ListPositionAllocator> staPositionAlloc = CreateObject<ListPositionAllocator>();
    for(uint16_t i=0; i<num_stas; i++) {
        uint16_t room = i % num_aps;
        double x = m_random->GetValue()*grid_length+(room%num_rooms_cols)*grid_length;
        double y = m_random->GetValue()*grid_length+(room/num_rooms_cols)*grid_length;
        staPositionAlloc->Add(Vector(x, y, 0.0));
        //NS_LOG_UNCOND("sta position: " << x << " " << y);
    }
    mobility.SetPositionAllocator(staPositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(staNodes);

    if(latex_out == 1) {
        ExportTopologyToLatex(apPositionAlloc, staPositionAlloc);
    }

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
        myClient.SetAttribute("Interval", TimeValue(Seconds(0.0002)));  // 60Mbps
        myClient.SetAttribute("PacketSize", UintegerValue(payload_size));
        ApplicationContainer clientApp = myClient.Install(staNodes.Get(j));
        clientApp.Start(Seconds(begin_time+0.001*(j)));
        clientApp.Stop(Seconds(begin_time+sim_time));
    }

    // Install flow monitor
    FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // Predefined schedules
    Simulator::Schedule(Seconds(begin_time), &PrintProgress);
    
	// Run simulation
	Simulator::Stop(Seconds(begin_time+sim_time+0.1));

    NS_LOG_UNCOND("Starting Simulation...");
	Simulator::Run();

    // Read statistics from the flow monitor
    uint32_t total_rx_packets = 0;
    uint32_t total_rx_packets_sq = 0;

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    fprintf(stderr, "-------------------------------------------------------------------------\n");
    fprintf(stderr, "| ID |   src addr      |   dst addr      |    tx    |    rx    |  tput  |\n");
    fprintf(stderr, "-------------------------------------------------------------------------\n");
    int c = 0;
    for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i=stats.begin(); i!=stats.end(); ++i) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        if(i->second.txPackets < 100) continue;
        c++;
        std::ostringstream oss;
        oss << t.sourceAddress;
        std::string sstr = oss.str();
        oss.str(std::string());
        oss << t.destinationAddress;
        std::string dstr = oss.str();
        double tput = i->second.rxBytes * 8.0 / sim_time / 1024.0 / 1024.0;

        total_rx_packets += i->second.rxPackets;
        total_rx_packets_sq += i->second.rxPackets * i->second.rxPackets;

        fprintf(stderr, "| %2d | %-15s | %-15s | %8lu | %8lu | %6.2f |\n", c, sstr.c_str(), dstr.c_str(), i->second.txBytes, i->second.rxBytes, tput);
    }
    fprintf(stderr, "-------------------------------------------------------------------------\n");
    

    // Throughput calculation ----------------------------------------------------
    double total_throughput = 0.0;
    for(uint16_t i=0; i<num_aps; i++) {
        uint32_t totalPacketsThrough = DynamicCast<UdpServer>(servers[i].Get(0))->GetReceived();
        double throughput = totalPacketsThrough * payload_size * 8 / (sim_time * 1000000.0); //Mbps
        total_throughput += throughput;
        //NS_LOG_UNCOND("throughput: " << throughput << "Mbps");
    }
    NS_LOG_UNCOND("total throughput: " << total_throughput << "Mbps");

    // Fairness calculation ------------------------------------------------------
    double fairness = pow((double)total_rx_packets, 2) / (num_stas * (double)total_rx_packets_sq);
    NS_LOG_UNCOND("Jain's fairness index: " << fairness);

	Simulator::Destroy();
	return 0;
}




