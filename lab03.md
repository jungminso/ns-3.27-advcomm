### Lab-03. ns-3의 무선랜 모듈 이해

이 실습에서는 무선랜 시뮬레이션 스크립트를 실행했을 때 ns-3 내부에서 어떤 일이 일어나는지를 확인해본다.
내용이 방대하기 때문에 모든 부분을 자세히 볼 수는 없고, 중요한 기능 위주로 분석한다.

---

#### 03.01. 개요

먼저 소스코드 분석을 위해 Lab-02에서 사용한 스크립트 script01.cc를 재사용한다. 이 파일에서, 딱 한 곳만 
변경을 시키는데, 아래 라인이다.

```cpp
	myClient.SetAttribute("Interval", TimeValue(Seconds(10.0)));
```

다시 말해, 노드가 패킷을 전송하는 간격을 10초로 늘렸다. 시뮬레이션 시간이 1초밖에 안되기 때문에, 이렇게 하면 송신 노드가
수신 노드에게 패킷을 딱 한개만 전송하게 된다. 새롭게 만든 파일의 이름을 [script03.cc](script03.cc)로 하였다.

스크립트를 실행시키면 아래와 같은 결과를 얻는다.

```
./waf --run scratch/script03

throughput: 0.011776Mbps
```

아마도 1초 동안에 1472바이트 패킷을 딱 한개 전송하면 이런 전송량이 나오나보다.



이제 소스코드 내에서 디버그 메시지를 출력하도록 하여 어떤 일이 벌어지는지를 확인해보겠다.
위에서 본 전송량에 관한 메시지도, 스크립트 안에서 디버그 메시지를 출력한 것이다. 다음 라인이다.

```cpp
	NS_LOG_UNCOND("throughput: " << throughput << "Mbps");
```

ns-3는 C++를 사용하기 때문에 cout이나 printf를 사용해도 메시지를 출력할 수 있다. 하지만 ns-3에서 사용하는
로깅 방식은 NS_LOG_* 함수들이므로 이를 사용하는 것이 좋다. 로그 메시지는 중요도에 따라 클래스를 부여할 수 있고,
특정 클래스에 해당하는 로그 메시지만 출력하도록 설정할 수 있다. 자세한 내용은 [ns-3 사이트의 로깅 부분](https://www.nsnam.org/docs/manual/html/logging.html)
을 참조하면 되고, 소스코드를 분석하면서 필요한 내용은 설명하도록 한다.

---

#### 03.02. 로그메시지 출력

무선랜과 관련된 소스 코드는 모두 ```src/wifi``` 디렉토리에 있다. 다른 다른 디렉토리들도 같은 구조로 되어있지만,
wifi 밑에는 helper와 model의 두 서브디렉토리가 있다. 기본적으로 model 디렉토리 내에는 기능을 구현한 파일들이 있고,
helper 디렉토리 내에는 이 모듈의 내용을 사용하는데 필요한 wrapper 함수들이 정의되어있다.

소스파일들에는 기본적으로 디버깅을 위하여 다양한 로그 메시지 출력을 위한 라인들이 이미 들어가 있는데, 이는 로그메시지를 출력하도록
설정하는 경우 나타나게 된다.

로그메시지를 출력하도록 설정하기 위하여, 시뮬레이션 스크립트의 main 함수 맨 위에 아래와 같은 라인을 넣고 실행시켜본다.

```cpp
	LogComponentEnableAll(LOG_LEVEL_ALL);
```

실행시키면 엄청난 양의 로그메시지가 출력되는 것을 볼 수 있다. LogComponentEnableAll이란 모든 컴포넌트에서 로그메시지를 출력하라는
뜻이고, LOG_LEVEL_ALL이란 모든 레벨의 로그메시지를 출력하라는 뜻이기 때문에 이렇게 많은 메시지가 출력된다. (여기서 컴포넌트는 각각의
소스파일을 의미한다고 보면 된다.) 이렇게 하면 보기가 불편하기 때문에 보통은 이렇게 하지 않고 원하는 컴포넌트의 원하는 레벨에 해당하는 로그만 
선택적으로 출력하도록 한다. 예를 들어, 위의 라인을 지우고 아래의 라인으로 바꾼다.

```cpp
	LogComponentEnable("WifiNetDevice", LOG_LEVEL_ALL);
```

이렇게 쓰면 WifiNetDevice 라는 컴포넌트에 있는 모든 로그메시지를 출력하라는 뜻이다. 컴포넌트의 이름은 파일의 맨 윗부분에 
NS_LOG_COMPONENT_DEFINE 매크로로 정의가 된다. WifiNetDevice는 wifi-net-device.cc에 정의된 컴포넌트 이름이다.

출력된 로그메시지를 보면 다음과 같다.

```
WifiNetDevice:WifiNetDevice()
WifiNetDevice:WifiNetDevice()
WifiNetDevice:NotifyNewAggregate(0x15e5610)
WifiNetDevice:NotifyNewAggregate(0x15eb5d0)
WifiNetDevice:Send(0x15eb5d0, 0x15f7620, 06-06-ff:ff:ff:ff:ff:ff, 2054)
WifiNetDevice:ForwardUp(0x15e5610, 0x15a8840, 00:00:00:00:00:02, ff:ff:ff:ff:ff:ff)
WifiNetDevice:Send(0x15e5610, 0x15f8ce0, 00-06-00:00:00:00:00:02, 2054)
WifiNetDevice:ForwardUp(0x15eb5d0, 0x15d77a0, 00:00:00:00:00:01, 00:00:00:00:00:02)
WifiNetDevice:Send(0x15eb5d0, 0x15e5810, 00-06-00:00:00:00:00:01, 2048)
WifiNetDevice:ForwardUp(0x15e5610, 0x15e4550, 00:00:00:00:00:02, 00:00:00:00:00:01)
WifiNetDevice:DoDispose()
WifiNetDevice:DoDispose()
throughput: 0.011776Mbps
WifiNetDevice:~WifiNetDevice()
WifiNetDevice:~WifiNetDevice()
```

일단 여기서 출력된 로그메시지는 우리가 출력한 throughput 메시지 외에는 전부 함수의 이름인 것을 알 수 있다. 이들은 NS_LOG_FUNCTION으로
출력된 메시지들이다. 예를 들어, wifi-net-device.cc 파일에는 이런 라인이 있다.

```cpp
bool
WifiNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);
  ...
}
```
























