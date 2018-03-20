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
