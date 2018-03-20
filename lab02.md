## Lab-02. 첫번째 시뮬레이션 스크립트

이 실습에서는, 무선랜 실험을 위한 시뮬레이션 스크립트를 이해한다.  
우리가 사용할 파일은 [script01.cc](script01.cc)이다.

아래에서는 라인별로 코드를 설명하였다.


```cpp
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
```

헤더파일을 include하는 부분이다. ns-3에서는 사용하는 함수에 대한 헤더파일을 각각 
포함시킬 수도 있지만, 지금의 경우와 같이 특정 디렉토리에 있는 헤더 전체를 묶어서
포함시킬 수 있다. 예를 들어, ```wifi-module.h```를 포함시키면,```wifi``` 디렉토리에
있는 헤더파일 전체가 포함된다. ```core-module```은 반드시 포함되어야 하고, ```mobility```,
```wifi```, ```internet```, ```applications```는 시뮬레이션 스크립트에서 사용되기
때문에 포함을 시켰다.

```cpp
using namespace ns3;
```

ns3 namespace를 사용하겠다고 선언하는 것이다. 이를 선언하지 않으면 많은 함수들에서 ```ns3::```를
앞에 붙여야 한다. 뒤에 나오는 ```std::string```과 같은 함수도 만약 ```std``` namespace를
선언해놓으면 ```std::```를 붙이지 않아도 되지만, 여기서는 ```ns3``` namespace를 사용하기 때문에
```std::string```과 같이 사용하였다.

```cpp
NS_LOG_COMPONENT_DEFINE("SimulationScript");
```

로깅(logging)을 위해서 이름을 선언해주는 부분이다. 이렇게 선언해놓으면 이 파일에서 ```NS_LOG_FUNCTION```등을 이용하여
로그메시지를 출력할 때 앞에 ```SimulationScript```라는 이름이 붙게 된다.

```cpp
CommandLine cmd;
cmd.Parse(argc, argv);
```

시뮬레이션을 실행시킬 때 주는 커맨드라인 인자를 정의하는 부분이다. 현재는 아무것도 정의하지 않은 상태이다.

```cpp
NodeContainer wifiNodes;
wifiNodes.Create(2);
```

```NodeContainer``` 클래스는 노드들을 한꺼번에 관리하기 위한 클래스이다. 이 클래스의 인스턴스를 생성하고,
2개의 노드를 만든다. 노드를 생성한 다음에는 무선채널, PHY, MAC, Network, Transport, Application 등
각 레이어에 해당하는 프로토콜을 선택하고 파라미터를 설정해 주어야 한다.

```cpp
YansWifiChannelHelper channel;
channel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
```

무선채널을 생성한다. 신호감쇄모델로는 실내환경을 가정할 때 주로 쓰이는 log-distance path loss 모델을 사용하고,
지연모델은 노드가 움직이지 않는 것을 가정한 constant speed 모델을 사용한다. 다른 모델을 사용하고 싶으면
이 부분을 변경해주면 된다. (예를 들어, log-distance 모델 대신 실외환경을 가정한 Friis 모델을 사용할 수 있다.)

```cpp
YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
phy.SetChannel(channel.Create());
phy.Set("ShortGuardEnabled", BooleanValue(1));
phy.Set("TxPowerStart", DoubleValue(20.0));
phy.Set("TxPowerEnd", DoubleValue(20.0));
phy.Set("EnergyDetectionThreshold", DoubleValue(-96.0));
phy.Set("CcaMode1Threshold", DoubleValue(-99.0));
```

PHY 모델을 생성하고 무선채널과 연결시켜준다. 그리고 PHY의 파라미터들을 설정한다. ```ShortGuardEnabled```는 
short guard interval (400ns)를 사용할 것인지, long guard interval (800ns)를 사용할 것인지
결정하는 것이다. ```TxPowerStart```와 ```TxPowerEnd```는 노드가 사용할 수 있는 전송파워의 범위를
정해준다. 만약 자동으로 노드의 전송파워를 변경하는 기술을 사용할 경우, 노드는 이 범위 안에서 파워를 조절할 수 있게
된다. 단위는 dBm이다.

```EnergyDetectionThreshold```와 ```CcaMode1Threshold```는 노드가 신호를 수신하는 수신세기의
임계값에 해당하는 파라미터이다. 수신 신호의 세기가 ```EnergyDetectionThreshold``` 값보다 커야 노드가
패킷을 수신할 수 있고, 감지되는 신호의 세기가 ```CcaMode1Threshold``` 값보다 크면 노드가 현재 채널
상태를 busy로 판단한다. 일반적으로 ```EnergyDetectionThreshold```가 ```CcaMode1Threshold```보다
크다. 두 값의 단위는 dBm이다.

```cpp
WifiMacHelper mac;
mac.SetType("ns3::AdhocWifiMac");
```

MAC 모델을 생성하고 ad-hoc 모드로 설정해준다. Ad-hoc 모드의 경우 AP없이 모바일 노드끼리 직접 통신하는 형태를
말하며, 이 외에도 AP 모드(```APWifiMac```)와 station 모드(```StaWifiMac```) 가 있다.





















