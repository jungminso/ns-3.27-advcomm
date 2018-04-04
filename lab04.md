### Lab-04. 대규모 시뮬레이션

지난 실습에서는 두 개의 노드만으로 실습을 수행했었다. 본 실습에서는 두 개 이상의 노드가 공존하는
환경에서의 실험을 통해 시스템 성능을 확인한다.

---

#### 04.01. 시뮬레이션 환경 구성

<center><img src="./simenv.png" width="400" height="400"></center>

위의 그림과 같은 환경에서 실험을 수행할 것이다. 오피스 빌딩 안에 여러 개의 방 또는 공간이 있고,
각 공간의 중앙에 무선랜 AP가 설치되어있다. 모바일 노드는 랜덤으로 위치가 결정된다.
각각의 모바일 노드는 자신이 통신을 할 AP를 정하는데, 이 실험에서는 가장 가까운 위치에 있는
AP로 정하게 된다. 트래픽은 일단 업로드 트래픽(모바일 노드에서 AP로 향하는 트래픽)만 있다고 가정한다.

이 실험을 수행하기 위한 시뮬레이션 스크립트가 script04.cc이다. 내용을 보면 지난 실습의 script03.cc와
많은 부분이 유사하고, 일부 내용이 바뀌었는데, 이 바뀐 부분을 중심으로 아래에 코드를 설명하였다.


```cpp
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
```

시뮬레이션 스크립트의 맨 앞부분이다. 지금까지는 main 함수 내에 있었던 변수들을 전역변수로 선언하였는데, 이렇게 한 이유는 main 함수 외에서도 이 변수들을 접근하기 위함이다.
물론 이렇게 하지 않고 main 함수에 변수들을 선언한 다음, 함수를 호출할 때마다 인자로 넣어줄 수도 있다.

가장 먼저 설명할 내용은 변수 m_random과 seed이다. m_random은 랜덤넘버 추출을 위해서 필요한 변수인데 Ptr<UniformRandomVariable> 형의 변수로 선언해주면 된다.
랜덤넘버에 관해서는 다음의 세 가지만 알고 있으면 된다.

(1) 포인터로 선언되어 있으므로 사용하기 전에 인스턴스를 만들어 준다.

```cpp
m_random = CreateObject<UniformRandomVariable>();
```

(2) 랜덤 시드를 정해준다. 이 시드에 따라 랜덤넘버 추출의 순서가 달라지게 된다. 만약 시드가 같으면 항상 같은 시퀀스의 랜덤넘버가 나오게 된다.

```cpp
m_random->SetStream(seed);
```

(3) 이제부터는 랜덤넘버를 하나씩 추출하여 사용하면 된다. 랜덤넘버는 0과 1 사이의 실수값이다.

```cpp
double x = m_random->GetValue();
```

seed는 SetStream 함수에 인자로 넣어주기 위한 변수이다.

다음 세 줄에 선언되어 있는 num_rooms_cols, num_rooms_rows, num_stas는 AP와 모바일노드의 개수에 관한 것이다. AP의 개수는 num_rooms_cols*num_rooms_rows가 되고,
num_stas는 모바일 노드의 개수이다. 또한, 아래에 선언되어 있는 grid_length는 각 방의 한 면의 길이를 의미한다. 여기서 모든 방은 정사각형 모양이라고 가정하였다.

begin_time과 sim_time은 전 실습에서도 있었기 때문에 이해할 수 있겠지만, 여기에는 pre_begin_time, pre_interval, pre_duration이 추가가 되었다. 아래의 코드에서 보면
본격적으로 트래픽을 전송하면서 실험하기 전에 먼저 약간의 트래픽을 전송하는 부분이 있다는 것을 알 수 있다. 이 부분은 ARP activation을 위한 부분으로, ARP 실패로 인한 성능 저하를
방지하려고 만든 부분이다. 즉, 이 부분에서 ARP는 모두 activation이 되고, 뒷부분에 본격적으로 트래픽을 전송할 때는 ARP Request와 ARP Reply가 전송되지 않는다. 

여기서 한 가지 주의할 것은, pre_begin_time과 begin_time 사이의 차이가 너무 크면, 그 사이에 ARP table의 엔트리가 시간초과로 리셋될 수 있다는 것이다. ARP가 activation 된 후
얼마의 시간동안 유효한지는 src/internet/model/arp-cache.cc 에서 수정할 수 있다. GetTypeId 함수 안에 선언되어있는 여러 attribute 중 AliveTimeout과 DeadTimeout을 길게 잡아주면
시뮬레이션 기간 동안에 ARP table의 엔트리가 expire되지 않도록 만들 수 있다.




