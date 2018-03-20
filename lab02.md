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
포함시킬 수 있다. 예를 들어, ```wifi-module.h```를 포함시키면, wifi 디렉토리에
있는 헤더파일 전체가 포함된다.
