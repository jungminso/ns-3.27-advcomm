### Lab-03. ns-3의 무선랜 모듈 이해

이 실습에서는 무선랜 시뮬레이션 스크립트를 실행했을 때 ns-3 내부에서 어떤 일이 일어나는지를 확인해본다.
내용이 방대하기 때문에 모든 부분을 자세히 볼 수는 없고, 중요한 기능 위주로 분석한다.

---

#### 03.01. 개요

먼저 소스코드 분석을 위해 Lab-02에서 사용한 스크립트 script01.cc를 재사용한다. 이 파일에서, 딱 한 곳만 
변경을 시키는데, 아래 라인이다.

```cpp
	myClient.SetAttribute("Interval", TimeValue(Seconds(0.8)));
```

다시 말해, 노드가 패킷을 전송하는 간격을 0.8초로 늘렸다. 시뮬레이션 시간이 1초밖에 안되기 때문에, 이렇게 하면 송신 노드가
수신 노드에게 패킷을 딱 두 개만 전송하게 된다. 새롭게 만든 파일의 이름을 [script03.cc](script03.cc)로 하였다.

스크립트를 실행시키면 아래와 같은 결과를 얻는다.

```
./waf --run scratch/script03

throughput: 0.023552Mbps
```

아마도 1초 동안에 1472바이트 패킷을 두개 전송하면 이런 전송량이 나오나보다.



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
WifiNetDevice:NotifyNewAggregate(0x1812610)
WifiNetDevice:NotifyNewAggregate(0x18185d0)
WifiNetDevice:Send(0x18185d0, 0x1824650, 06-06-ff:ff:ff:ff:ff:ff, 2054)
WifiNetDevice:ForwardUp(0x1812610, 0x17d5840, 00:00:00:00:00:02, ff:ff:ff:ff:ff:ff)
WifiNetDevice:Send(0x1812610, 0x1825d10, 00-06-00:00:00:00:00:02, 2054)
WifiNetDevice:ForwardUp(0x18185d0, 0x18047a0, 00:00:00:00:00:01, 00:00:00:00:00:02)
WifiNetDevice:Send(0x18185d0, 0x1812810, 00-06-00:00:00:00:00:01, 2048)
WifiNetDevice:ForwardUp(0x1812610, 0x1811550, 00:00:00:00:00:02, 00:00:00:00:00:01)
WifiNetDevice:Send(0x18185d0, 0x1824420, 00-06-00:00:00:00:00:01, 2048)
WifiNetDevice:ForwardUp(0x1812610, 0x1816d40, 00:00:00:00:00:02, 00:00:00:00:00:01)
WifiNetDevice:DoDispose()
WifiNetDevice:DoDispose()
throughput: 0.023552Mbps
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

이 라인을 실행한 결과는 아래와 같다.

```
WifiNetDevice:Send(0x15eb5d0, 0x15f7620, 06-06-ff:ff:ff:ff:ff:ff, 2054)
```

NS_LOG_FUNCTION 함수를 쓰면 일단 출력할 로그메시지의 앞부분에 컴포넌트이름:현재함수이름이 붙는다. 그 뒤에는 인자로 넣은
변수의 값들이 들어가는데 여기서는 다음과 같다.

```
this: 0x15eb5d0
packet: 0x15f7620
dest: 06-06-ff:ff:ff:ff:ff:ff
protocolNumber: 2054
```

this와 packet은 특정 인스턴스의 주소 값이라는 것을 알 수 있다. dest는 목적지의 MAC address인데, 이 내용을 보면
브로드캐스트인것을 알 수 있고, protocolNumber가 [2054](https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml)
인데 이는 현재 전송하려는 메시지가 [ARP](https://ko.wikipedia.org/wiki/%EC%A3%BC%EC%86%8C_%EA%B2%B0%EC%A0%95_%ED%94%84%EB%A1%9C%ED%86%A0%EC%BD%9C)
메시지임을 알 수 있다. 이와 같이 로그메시지를 이용해 현재 시뮬레이터 내에서 어떤 일이 벌어지고 있는지 확인이 가능하다.

로그메시지를 출력할 때, 해당 로그메시지가 찍힌 시뮬레이션 상의 시간을 알고 싶은 경우가 많다. 이를 위해서는 LogComponentEnable 함수의 인자를 다음과
같이 바꾸면 된다.

```cpp
	LogComponentEnable("WifiNetDevice", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
```


---

#### 03.03. 패킷 전송과정 이해 - WifiNetDevice

script03.cc를 실행시켜서 출력된 로그메시지를 보면 여러가지 정보를 얻을 수가 있다.

```
+0.000000000s WifiNetDevice:WifiNetDevice()
+0.000000000s WifiNetDevice:WifiNetDevice()
+0.000000000s WifiNetDevice:NotifyNewAggregate(0x1812610)
+0.000000000s WifiNetDevice:NotifyNewAggregate(0x18185d0)
+60.006000000s WifiNetDevice:Send(0x18185d0, 0x1824650, 06-06-ff:ff:ff:ff:ff:ff, 2054)
+60.006112066s WifiNetDevice:ForwardUp(0x1812610, 0x17d5840, 00:00:00:00:00:02, ff:ff:ff:ff:ff:ff)
+60.006112066s WifiNetDevice:Send(0x1812610, 0x1825d10, 00-06-00:00:00:00:00:02, 2054)
+60.006259132s WifiNetDevice:ForwardUp(0x18185d0, 0x18047a0, 00:00:00:00:00:01, 00:00:00:00:00:02)
+60.006259132s WifiNetDevice:Send(0x18185d0, 0x1812810, 00-06-00:00:00:00:00:01, 2048)
+60.006612198s WifiNetDevice:ForwardUp(0x1812610, 0x1811550, 00:00:00:00:00:02, 00:00:00:00:00:01)
+60.800000000s WifiNetDevice:Send(0x18185d0, 0x1824420, 00-06-00:00:00:00:00:01, 2048)
+60.800248066s WifiNetDevice:ForwardUp(0x1812610, 0x1816d40, 00:00:00:00:00:02, 00:00:00:00:00:01)
WifiNetDevice:DoDispose()
WifiNetDevice:DoDispose()
throughput: 0.023552Mbps
WifiNetDevice:~WifiNetDevice()
WifiNetDevice:~WifiNetDevice()
```

먼저, 맨 처음에 WifiNetDevice의 인스턴스가 두 개 생성되고, 맨 마지막에 두 개가 소멸되는 것을 알 수 있다. 노드의 개수가 2개이고,
각 노드마다 하나의 무선네트워크 인터페이스를 가지고 있기 때문이다.

다음으로 NotifyNewAggregate이라는 함수가 나오는데 이는 지금 이름만 봐서는 잘 모르니 나중에 알아보도록 한다.

다음으로는 Send - ForwardUp 이 두 함수가 쌍으로 네 번이 나온다. Send 함수는 메시지를 송신할 때 호출되는 함수이고, ForwardUp은
수신한 메시지를 상위 레이어로 올릴 때 사용하는 함수이다. 따라서 송신과 수신이 네 번 일어난 것이다. 시뮬레이션 스크립트에서는 트래픽의 간격(interval)을
0.8초로 길게 잡아서 패킷이 두 번만 전송되도록 하였는데, 왜 네 개나 전송이 되었을까라는 의문이 든다.

Send의 함수를 보면 위에서 분석한 대로 출력되는 로그메시지에 수신자의 MAC 주소와, 패킷의 protocol 넘버가 있다. 
첫번째와 두번째 패킷은 ARP 패킷이고, 세번째와 네번째는 IPv4 패킷[(2048)](https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml), 
즉 데이터 패킷이라는 것을 알 수 있다. 시뮬레이션 스크립트에서 전송하도록 한 트래픽은 이 IPv4 패킷 두개이고, 나머지는 네트워크에서 생성되는 컨트롤 메시지이다.

마지막으로 DoDispose라는 함수가 나오는데, 이것은 시뮬레이션을 종료하거나 리셋할때 호출하는 함수이다.

WifiNetDevice 컴포넌트에 대한 로그메시지 출력을 통해서 알수 있는 사실을 정리하면 다음과 같다.

- 패킷이 전송되고 수신될 때 WifiNetDevice의 Send 함수를 거친다.
- 패킷이 전송되고 수신될 때 WifiNetDevice의 ForwardUp 함수를 거친다.
- 패킷을 한번 전송하도록 했는데, 그 패킷이 전송되기 전 두 개의 ARP 메시지가 전송된다.
- 첫번째 ARP 메시지는 브로드캐스트 되고, 두번째 ARP 메시지는 00:00:00:00:00:01로부터 00:00:00:00:00:02로 향하는 메시지이다.

일단 여기서 ARP에 대해 간단하게 이야기 하면, ARP는 IP 주소와 MAC 주소를 매핑해주기 위한 프로토콜이다. 패킷이 송신될 때, IP 모듈로부터 내려온 패킷에는
수신자의 IP 주소가 적혀있지만, WifiNetDevice가 전송을 할 때는 송신자의 MAC 주소가 필요하다. 아직 데이터를 한번도 전송해본 적 없는 수신자에 대해서는,
IP 주소와 MAC 주소간의 매핑을 알 수 없기 때문에 메시지 교환을 통해 알아낼 수 밖에 없다. 이 때 사용하는 것이 ARP이다.

송신자는 먼저 ARP Request를 브로드캐스트한다. (수신자의 MAC 주소를 FF:FF:FF:FF:FF:FF로 설정함.) 주변에 있는 노드들 중 송신자가 알고자 하는
MAC 주소를 가진 노드는 송신자에게 ARP Reply를 보낸다. 송신자는 ARP Reply를 받으면 이제 해당 노드에 대한 IP주소-MAC주소 매핑을 갖게 된다.

---

#### 03.04. 패킷 전송과정 이해 - MAC, PHY

패킷이 전송될 때 WifiNetDevice 컴포넌트를 지나간다는 것은 알았다. 이제는 또 어떤 컴포넌트들이 사용되는지 알아보고자 한다. src/wifi/model에 있는데
여러가지 컴포넌트들에 대해 로그메시지를 출력함으로써 어떤 함수들이 불리게 되는지 알아본다.

script03.cc의 main 함수에 다음과 같은 라인을 넣는다.

```cpp
    LogComponentEnable("WifiNetDevice", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("WifiMac", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("RegularWifiMac", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("AdhocWifiMac", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("MacTxMiddle", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("MacRxMiddle", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("MacLow", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("WifiPhy", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("YansWifiPhy", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("YansWifiChannel", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
```

여기에 있는 컴포넌트들에서 나오는 로그메시지를 출력하라는 뜻이다. 이제 시뮬레이션을 실행하되, 로그메시지가 너무 길기 때문에 파일로 리다이렉션한다.

```
./waf --run scratch/script03 2> log
```

로그 파일을 열어보면 약 500라인 정도가 된다. 이 중에서, 우리가 볼 부분은 60.8초에 패킷이 전송되는 과정이다. 거기에 해당하는 로그는 아래와 같다.
분량이 많아서 로그가 발생한 시간에 따라 나눠서 본다.

```
+60.800000000s WifiNetDevice:Send(0x25bb5b0, 0x25a77a0, 00-06-00:00:00:00:00:01, 2048)
+60.800000000s AdhocWifiMac:Enqueue(0x25bb980, 0x25a77a0, 00:00:00:00:00:01)
+60.800000000s MacTxMiddle:GetNextSequenceNumberFor(0x25bb6b0)
+60.800000000s [mac=00:00:00:00:00:02] MacLow:StartTransmission(0x25bbb70, 0x25a77a0, 0x25bd3f8, [send rts=0, next size=0, dur=+0.0ns, ack=normal], 0x25bd380)
+60.800000000s [mac=00:00:00:00:00:02] MacLow:CancelAllEvents(0x25bbb70)
+60.800000000s [mac=00:00:00:00:00:02] startTx size=1536, to=00:00:00:00:00:01, dca=0x25bd380
+60.800000000s [mac=00:00:00:00:00:02] MacLow:SendDataPacket(0x25bbb70)
+60.800000000s WifiPhy:GetPayloadDuration(1536, OfdmRate54Mbps)
+60.800000000s [mac=00:00:00:00:00:02] MacLow:GetSifs(0x25bbb70)
+60.800000000s WifiPhy:GetPayloadDuration(14, OfdmRate24Mbps)
+60.800000000s [mac=00:00:00:00:00:02] MacLow:ForwardDown(0x25bbb70, 0x25b9930, 0x25bbce8, mode: OfdmRate54Mbps txpwrlvl: 0 retries: 0 preamble: 0 channel width: 20 GI: 800 NTx: 1 Nss: 1 Ness: 0 MPDU aggregation: 0 STBC: 0)
+60.800000000s [mac=00:00:00:00:00:02] send DATA, to=00:00:00:00:00:01, size=1536, mode=OfdmRate54Mbps, preamble=0, duration=+44000.0ns, seq=0x20
+60.800000000s WifiPhy:SendPacket(0x25bffa0, 0x25b9930, OfdmRate54Mbps, 54000000, 0, 0, 0)
+60.800000000s WifiPhy:GetPayloadDuration(1536, OfdmRate54Mbps)
+60.800000000s Start transmission: signal power before antenna gain=20dBm
+60.800000000s YansWifiChannel:Send(0x25b2b50, 0x25bffa0, 0x25be720, 20, 0.000248)
+60.800000000s propagation: txPower=20dbm, rxPower=-65.7086dbm, distance=20m, delay=+66.0ns
```

패킷이 전송될 때 호출되는 주요 함수들은 다음과 같다.

- WifiNetDevice:Send - IP패킷을 NIC를 통해 전송하려고 할 때 호출된다. 
- AdhocWifiMac:Enqueue - NIC의 interface queue에 전송하려는 패킷을 넣어놓고 DCF에 따라 전송 기회를 기다린다.
- MacLow:StartTransmission - 채널 경쟁에 통해 전송할 기회를 얻었으므로 패킷을 전송한다. 이를 위해 PHY로 패킷을 내린다.
- WifiPhy:SendPacket - PHY에서 패킷을 전송하기 위해 해야 할 일들을 처리한다. 그리고 나서는 채널로 내려보낸다.
- YansWifiChannel:Send - 채널을 통해 패킷을 전달한다.

실제로는 레이어를 통해 패킷이 내려오면서 여러가지 처리가 일어나기 때문에 프로세싱 시간이 있지만, 시뮬레이션에서는 이 시간을 무시하고
여기까지는 전부 60.8초에 일어난 것으로 본다.

```
+60.800020066s WifiPhy:StartReceivePacket(0x25bacb0, 0x25b4d40, OfdmRate54Mbps, 0, 0)
+60.800020066s snr(dB)=28.2574, per=0
+60.800020066s receiving plcp payload
```

패킷이 전송되면 수신자의 PHY에서 패킷을 수신하는 과정이 있다. 패킷의 송신과 수신 사이에 약 20us 정도의 차이가 있는데, 이것은 수신자가
송신된 패킷의 앞부분에 있는 프리앰블(preamble)을 수신하는 시간을 고려한 것이다. 프리앰블에 관해서는 [여기](http://rfmw.em.keysight.com/wireless/helpfiles/89600b/webhelp/subsystems/wlan-ofdm/content/ofdm_80211-overview.htm)와 같은 사이트를 참조하면 된다.
프리앰블을 수신하고 나면 StartReceivePacket 함수에서 SNR을 계산하여 제대로 송신이 되는지를 판단한다.

```
+60.800248066s WifiPhy:EndReceive(0x25bacb0, 0x25b4d40, 0x257cc70)
+60.800248066s mode=54000000, snr(dB)=28.2574, per=0, size=1536
+60.800248066s [mac=00:00:00:00:00:01] MacLow:DeaggregateAmpduAndReceive(0x25b7470)
+60.800248066s [mac=00:00:00:00:00:01] MacLow:ReceiveOk(0x25b7470, 0x25b4d40, 669.481, OfdmRate54Mbps, 0)
+60.800248066s [mac=00:00:00:00:00:01] duration/id=+44000.0ns
+60.800248066s [mac=00:00:00:00:00:01] rx unicast/sendAck from=00:00:00:00:00:02
+60.800248066s [mac=00:00:00:00:00:01] MacLow:GetSifs(0x25b7470)
+60.800248066s MacRxMiddle:Receive(0x25b4d40, 0x7ffc6ef891d0)
+60.800248066s MacRxMiddle:Lookup(0x7ffc6ef891d0)
+60.800248066s MacRxMiddle:IsDuplicate(0x7ffc6ef891d0, 0x258ed60)
+60.800248066s MacRxMiddle:HandleFragments(0x25b4d40, 0x7ffc6ef891d0, 0x258ed60)
+60.800248066s forwarding data from=00:00:00:00:00:02, seq=2, frag=0
+60.800248066s AdhocWifiMac:Receive(0x25b72b0, 0x25b4d40, 0x7ffc6ef891d0)
+60.800248066s RegularWifiMac:ForwardUp(0x25b72b0, 0x25b4d40, 00:00:00:00:00:02, 00:00:00:00:00:01)
+60.800248066s WifiNetDevice:ForwardUp(0x25b5610, 0x25b4d40, 00:00:00:00:00:02, 00:00:00:00:00:01)
```

프리앰블과 페이로드 (payload)까지 프레임 전부를 수신하고 나면 WifiPhy:EndReceive가 호출된다. 패킷이 MAC 레이어로 올라가면서 MacLow:ReceiveOk와
MacRxMiddle:Receive, AdhocWifiMac:Receive, RegularWifiMac:ForwardUp, WifiNetDevice:ForwardUp 등의 함수를 거친다.

```
+60.800264066s [mac=00:00:00:00:00:01] MacLow:SendAckAfterData(0x25b7470)
+60.800264066s WifiPhy:GetPayloadDuration(14, OfdmRate24Mbps)
+60.800264066s [mac=00:00:00:00:00:01] MacLow:GetSifs(0x25b7470)
+60.800264066s [mac=00:00:00:00:00:01] MacLow:ForwardDown(0x25b7470, 0x25bd810, 0x7ffc6ef89880, mode: OfdmRate24Mbps txpwrlvl: 0 retries: 0 preamble: 0 channel width: 20 GI: 800 NTx: 1 Nss: 1 Ness: 0 MPDU aggregation: 0 STBC: 0)
+60.800264066s [mac=00:00:00:00:00:01] send CTL_ACK, to=00:00:00:00:00:02, size=14, mode=OfdmRate24Mbps, preamble=0, duration=+0.0ns, seq=0x0
+60.800264066s WifiPhy:SendPacket(0x25bacb0, 0x25bd810, OfdmRate24Mbps, 24000000, 0, 0, 0)
+60.800264066s WifiPhy:GetPayloadDuration(14, OfdmRate24Mbps)
+60.800264066s Start transmission: signal power before antenna gain=20dBm
+60.800264066s YansWifiChannel:Send(0x25b2b50, 0x25bacb0, 0x25b3e60, 20, 2.8e-05)
+60.800264066s propagation: txPower=20dbm, rxPower=-65.7086dbm, distance=20m, delay=+66.0ns
```

수신자가 프레임을 받고 나면 송신자에게 acknowledgment (ACK)을 전송해야 한다. 데이터 프레임을 다 받고나서 ACK을 전송하는 사이의 간격이 SIFS
(Short Inter-Frame Spacing)인데, 이 시간이 16us이다. 따라서 위의 과정은 프레임을 받고 16us가 지난 후 일어난다.

```
+60.800264132s YansWifiChannel:Receive(0x25bffa0, 0x2578840, -65.7086, 2.8e-05)
+60.800264132s WifiPhy:StartReceivePreambleAndHeader(0x25bffa0, 0x2578840, -65.7086, +28000.0ns)
+60.800264132s WifiPhy:StartRx(0x25bffa0, 0x2578840, mode: OfdmRate24Mbps txpwrlvl: 0 retries: 0 preamble: 0 channel width: 20 GI: 800 NTx: 1 Nss: 1 Ness: 0 MPDU aggregation: 0 STBC: 0, 0, 2.68621e-10, +28000.0ns)
+60.800264132s sync to signal (power=2.68621e-10W)
```

ACK을 전송할때는 데이터 프레임 전송과는 달리 채널 경쟁 과정이 없다. 따라서 바로 채널로 보낸다.

```
+60.800284132s WifiPhy:StartReceivePacket(0x25bffa0, 0x2578840, OfdmRate24Mbps, 0, 0)
+60.800284132s snr(dB)=28.2574, per=0
+60.800284132s receiving plcp payload
```

데이터 프레임을 수신할 때와 마찬가지로 20us 동안 프리앰블을 먼저 수신한다. 그것이 끝나면 본격적으로 프레임을 수신하기 시작한다.

```
+60.800292132s WifiPhy:EndReceive(0x25bffa0, 0x2578840, 0x25c89f0)
+60.800292132s mode=24000000, snr(dB)=28.2574, per=0, size=14
+60.800292132s [mac=00:00:00:00:00:02] MacLow:DeaggregateAmpduAndReceive(0x25bbb70)
+60.800292132s [mac=00:00:00:00:00:02] MacLow:ReceiveOk(0x25bbb70, 0x2578840, 669.481, OfdmRate24Mbps, 0)
+60.800292132s [mac=00:00:00:00:00:02] duration/id=+0.0ns
+60.800292132s [mac=00:00:00:00:00:02] receive ack from=00:00:00:00:00:01
+60.800292132s RegularWifiMac:TxOk(0x25bb980, DATA ToDS=0, FromDS=0, MoreFrag=0, Retry=0, MoreData=0 Duration/ID=28408us, DA=00:00:00:00:00:01, SA=00:00:00:00:00:02, BSSID=00:00:00:00:00:02, FragNumber=0, SeqNumber=2)
```

ACK은 매우 작은 패킷이기 때문에 프리앰블을 제외하고 수신하는데 8us 밖에 걸리지 않았다. 맨 마지막에 있는 RegularWifiMac:TxOk는, 송신자가 ACK을 
제대로 수신하여 데이터 전송이 성공적으로 끝났을 때 호출되는 함수이다.

정리하자면, ns-3에서는 패킷을 전송할 때 NIC->MAC->PHY->CHANNEL->PHY->MAC->NIC의 순서로 패킷이 송신자에서 수신자로 이동하게 된다.

---

#### 03.05. DCF 이해

지금까지 패킷이 전송될 때 거쳐가는 컴포넌트들에 대해 알아보았다. 하지만 위에서는 802.11 DCF에 의한 채널 경쟁 과정은 생략 되었다. WifiNetDevice:Send가 호출되고 AdhocWifiMac:Enqueue를 통해 패킷이 인터페이스 큐에
들어가고 나면, CSMA/CA 방식을 통해 채널에 접근할 기회를 찾는다. 먼저 캐리어센싱을 통해 채널이 IDLE인지 BUSY인지 확인을 한다. IDLE일 경우 패킷을 전송할 수 있다. 만약 BUSY이면 random backoff를 통해 일정 시간을
기다린 후 전송을 하게 되어있다. 이러한 과정을 거쳐서 전송기회를 얻게 되면 MacLow:StartTransmission이 호출되고 패킷이 전송되게 된다.

여기서는 채널경쟁과 관련된 컴포넌트들이 무엇인지 확인해본다. script03.cc 파일의 로그메시지 설정 부분을 아래와 같이 바꾼다.

```
    LogComponentEnable("WifiNetDevice", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("WifiMac", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("RegularWifiMac", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("AdhocWifiMac", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("MacLow", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("DcaTxop", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
    LogComponentEnable("DcfManager", LogLevel(LOG_LEVEL_ALL|LOG_PREFIX_TIME));
```

DcaTxop와 DcfManager의 두 컴포넌트로부터 로그메시지를 출력할 수 있도록 하였다. 시뮬레이션을 실행하면 매우 많은 로그메시지가 출력되는데, 그 중에서 중요한 부분만 아래에 간추려놓았다.
60.8초에 생성된 트래픽이 전송되는 과정에서 발생한 로그메시지이다.

```
+60.800000000s WifiNetDevice:Send(0x95d5a0, 0x9576d0, 00-06-00:00:00:00:00:01, 2048)
+60.800000000s AdhocWifiMac:Enqueue(0x95d970, 0x9576d0, 00:00:00:00:00:01)
+60.800000000s [mac=00:00:00:00:00:02] DcaTxop:Queue(0x95e370, 0x9576d0, 0x7ffc76153ba0)
+60.800000000s [mac=00:00:00:00:00:02] DcaTxop:StartAccessIfNeeded(0x95e370)
+60.800000000s DcfManager:RequestAccess(0x95efe0, 0x95f3d0)
+60.800000000s [mac=00:00:00:00:00:02] DcaTxop:NotifyAccessGranted(0x95e370)
+60.800000000s [mac=00:00:00:00:00:02] MacLow:StartTransmission(0x95db90, 0x9576d0, 0x95e3e8, [send rts=0, next size=0, dur=+0.0ns, ack=normal], 0x95e370)
+60.800000000s DcfManager:NotifyTxStartNow(0x95efe0, +248000.0ns)
+60.800000000s tx start for +248000.0ns
+60.800000066s DcfManager:NotifyRxStartNow(0x1af3650, +248000.0ns)
+60.800000066s rx start for=+248000.0ns
+60.800248066s DcfManager:NotifyRxEndOkNow(0x95a650)
+60.800248066s rx end ok
+60.800248066s [mac=00:00:00:00:00:01] MacLow:ReceiveOk(0x959470, 0x95f2d0, 669.481, OfdmRate54Mbps, 0)
+60.800248066s [mac=00:00:00:00:00:01] duration/id=+44000.0ns
+60.800248066s [mac=00:00:00:00:00:01] rx unicast/sendAck from=00:00:00:00:00:02
+60.800248066s [mac=00:00:00:00:00:01] MacLow:GetSifs(0x959470)
+60.800248066s AdhocWifiMac:Receive(0x9592b0, 0x95f2d0, 0x7ffc76154d70)
+60.800248066s RegularWifiMac:ForwardUp(0x9592b0, 0x95f2d0, 00:00:00:00:00:02, 00:00:00:00:00:01)
+60.800248066s WifiNetDevice:ForwardUp(0x957610, 0x95f2d0, 00:00:00:00:00:02, 00:00:00:00:00:01)
+60.800264066s [mac=00:00:00:00:00:01] MacLow:SendAckAfterData(0x959470)
+60.800264132s DcfManager:NotifyRxStartNow(0x95efe0, +28000.0ns)
+60.800264132s rx start for=+28000.0ns
+60.800292132s DcfManager:NotifyRxEndOkNow(0x95efe0)
+60.800292132s rx end ok
+60.800292132s [mac=00:00:00:00:00:02] MacLow:ReceiveOk(0x95db90, 0x8f6f20, 669.481, OfdmRate24Mbps, 0)
+60.800292132s [mac=00:00:00:00:00:02] duration/id=+0.0ns
+60.800292132s [mac=00:00:00:00:00:02] receive ack from=00:00:00:00:00:01
+60.800292132s DcfManager:NotifyAckTimeoutResetNow(0x95efe0)
+60.800292132s DcfManager:DoRestartAccessTimeoutIfNeeded(0x95efe0)
+60.800292132s [mac=00:00:00:00:00:02] DcaTxop:GotAck(0x95e370)
+60.800292132s [mac=00:00:00:00:00:02] got ack. tx done.
```

가장 먼저 DcaTxop:Queue는 인터페이스 큐에 패킷을 넣는 함수이다. 인터페이스 큐에 패킷을 넣으면, DCF 프로토콜을 관장하는 DcfManager에게 채널에 접근해야 한다는 것을 알리는데, 이 때 호출되는
함수가 **DcfManager:RequestAccess**이다. DcfManager는 일단 접근요청이 들어오면 현재 채널의 상태를 확인한다. 만약 채널이 IDLE 상태라면 DcfManager는 바로 패킷을 전송할 수 있다고
DcaTxop에게 말을 한다. 이를 수행하는 함수가 **DcaTxop:NotifyAccessGranted**이다. NotifyAccessGranted가 호출되면, 노드는 MacLow:StartTransmission을 통해 패킷을 하위
레이어로 보내어 채널에 전송되도록 한다. MAC이 패킷을 전송했다는 것은 DcfManager로 NotifyTxStartNow 함수를 통해 전달이 된다. 위의 예의 경우 프레임이 전송되는데 248us의 시간이 걸린다.

수신자 측에서, 패킷 수신을 시작하면 DcfManager:NotifyRxStartNow가 호출되고 패킷 수신이 끝나면 NotifyRxEndOkNow가 호출된다. 패킷이 성공적으로 수신되었다면,
MacLow:ReceiveOk가 호출된다. 그리고 SIFS 이후에 ACK을 전송하기 위해 MacLow:SendAckAfterData가 호출된다. ACK이 전송되면 ACK을 받는 노드에서는 마찬가지로 DcfManager:NotifyRxStartNow와
DcfManager:NotifyRxEndOkNow가 호출되고, MacLow:ReceiveOk가 호출된다. MacLow:ReceiveOk에서는 지금 받은 패킷이 어떤 종류의 메시지인지 (데이터인지 컨트롤인지)를 확인하는데,
ACK을 수신한 경우에는 DcaTxop:GotAck이 호출된다. 송신자가 수신자에게 데이터프레임을 전송하고 ACK을 받으면 하나의 트랜잭션이 끝나는데, GotAck(혹은 MissedAck)은 트랜잭션이 끝날 때 호출되는
함수라고 보면 된다. DcaTxop:GotAck에서는 다음 패킷 송신을 위한 준비를 하는데, contention window로부터 숫자를 하나 랜덤으로 선택하여 backoff countdown을 시작하는 루틴이 이 함수에 있다.
DCF에서는 하나의 패킷을 송신하고 나면 반드시 random backoff를 해야 하고, backoff counter가 0이 되어야 다음 패킷을 전송할 수 있다.


---

### Task 1. DCF 코드 변경

이제부터는 ns-3의 소스코드를 변경해보도록 하겠다. 일단은 매우 간단하게 random backoff 부분을 변경하면서 성능에 어떤 변화가 일어나는지 살펴본다.
위에서 설명했듯이, 노드가 패킷을 하나 전송하고 난 다음에는, 반드시 random backoff를 통해 어느 정도 시간이 흐른 다음 다시 패킷을 전송할 수 있다.
이렇게 하는 이유는 여러 노드가 있을 때 하나의 노드가 채널을 점유하는 현상을 막기 위해서이다.

ns-3 코드에서는 노드가 패킷을 전송하고 수신자로부터 ACK을 받냐 못받냐에 따라 GotAck 또는 MissedAck 함수가 호출된다. (브로드캐스트 패킷의 경우에는
ACK이 없기 때문에 이 경우에는 다른 함수에 의해 처리된다.) 예를 들어 GotAck 함수는 다음과 같은 내용으로 되어있다.

```cpp
void
DcaTxop::GotAck (void)
{
  NS_LOG_FUNCTION (this);
  if (!NeedFragmentation ()
      || IsLastFragment ())
    {
      NS_LOG_DEBUG ("got ack. tx done.");
      if (!m_txOkCallback.IsNull ())
        {
          m_txOkCallback (m_currentHdr);
        }

      /* we are not fragmenting or we are done fragmenting
       * so we can get rid of that packet now.
       */
      m_currentPacket = 0;
      m_dcf->ResetCw ();
      m_dcf->StartBackoffNow (m_rng->GetInteger (0, m_dcf->GetCw ()));
      RestartAccessIfNeeded ();
    }
  else
    {
      NS_LOG_DEBUG ("got ack. tx not done, size=" << m_currentPacket->GetSize ());
    }
}
```

여기서 주목할 부분은 다음 두 라인이다.

```cpp
      m_dcf->ResetCw ();
      m_dcf->StartBackoffNow (m_rng->GetInteger (0, m_dcf->GetCw ()));
```

먼저 첫번째 라인에서는 contention window를 초기값으로 리셋한다. Contention window는 초기값에서 출발하여, 노드가 패킷 전송에 실패할때마다
두 배씩 증가하다가, 패킷 전송에 성공하게 되면 바로 초기상태로 돌아오게 된다. 

두번째 라인에서는 contention window에서 랜덤 넘버를 선택하여 backoff를 시작한다. DcfManager의 함수 StartBackoffNow를 호출할 때
사용되는 인자가 backoff counter가 된다.

원래 backoff를 하는 이유는, 여러 노드가 서로 채널을 얻기 위해 경쟁을 할 때 이들이 전송하는 시간을 다르게 하기 위함이다. 하지만 전송하는 노드가
하나밖에 없는 경우에는 backoff가 필요하지 않다. 따라서 이 부분을 다음과 같이 고쳐보도록 한다.

```cpp
      m_dcf->ResetCw ();
      m_dcf->StartBackoffNow (1);
```

이렇게 하면 랜덤넘버를 뽑는 대신 항상 backoff counter가 1에서 출발하게 된다. 이제 시뮬레이션 스크립트를 실행할 건데, 로그 부분을 없애고
트래픽의 interval은 다시 0.0001로 변경해준다. 그리고 시뮬레이션을 실행하여 전송량을 확인한다. 이러한 전송량이 나오는 이유는 무엇일까?


---

### Task 2. 내부 모듈의 파라미터 변경

이번에는 Task 1과 같이 코드에 직접 값을 넣는 방법 대신, 시뮬레이션 스크립트에서 contention window의 범위를 지정해 줄 수 있도록 코드를 작성해본다.
일단 contention window와 관련된 파라미터를 찾는다. dcf-state.h 파일에 보면, 다음의 세가지 파라미터가 정의되어 있는 것을 알 수 있다.

```cpp
  uint32_t m_cwMin;       //!< the CW minimum
  uint32_t m_cwMax;       //!< the CW maximum
  uint32_t m_cw;          //!< the current CW
```

이 변수들의 의미는 주석을 보고 짐작할 수 있을 것이다. 일단 이 실습에서는 m_cwMin을 시뮬레이션 스크립트에서 바꿔보고자 한다.

이를 위하여는 먼저 컴포넌트들이 어떻게 연결되었는지를 알아야 한다. 일단 시뮬레이션 스크립트에서는 WifiNetDevice 클래스를 직접 접근할 수 있다.
script03.cc 안에 보면 MAC과 PHY 등을 연결하여 네트워크 디바이스를 만드는 부분이 있을 것이다.

```
    NetDeviceContainer devices = wifi.Install(phy, mac, wifiNodes);
```

devices를 이용하여 다음과 같이 각 노드의 네트워크 디바이스를 접근할 수 있다.

```
    Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(devices.Get(0));
```

이렇게 하면 0번 노드의 네트워크 디바이스를 가져올수 있다. 여기서 DynamicCast가 쓰이는데, Ptr이 포인터라면 DynamicCast는
동적캐스팅이라고 보면 된다. devices.Get 함수의 리턴타입은 Ptr<NetDevice>인데, 이것은 WifiNetDevice의 상위 클래스이다.
우리가 device를 WifiNetDevice의 포인터로 선언하였으므로, 리턴된 값을 WifiNetDevice의 포인터로 동적캐스팅해준다.

이제 WifiNetDevice 클래스와 DcfState가 어떻게 연결되어있는지를 알면 m_cwMin에도 접근할 수가 있다.

먼저 wifi-net-device.h를 보면 멤버변수에

```cpp
Ptr<WifiMac> m_mac;
```

이라고 선언되어있는 것을 찾을 수 있다. Ptr<WifiMac>은 WifiMac 타입의 포인터를 의미하는 것으로써, WifiMac*과 같이 취급하면 된다.
일반적으로 클래스의 멤버변수가 있으면, 그 멤버변수를 접근하기 위한 함수가 있는데, m_mac에 대해서는 다음과 같은 함수가 있는 것을 확인할 수 있다.

```cpp
Ptr<WifiMac> GetMac (void) const;
```

이 함수를 이용하여 m_mac을 접근할 수 있다. 그런데 우리가 사용하고자 하는 MAC의 클래스는 RegularWifiMac이고, 여기에서 리턴되는 클래스는
WifiMac이므로 또한번 DynamicCasting을 해주어야 한다.

```cpp
    Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(devices.Get(0));
	Ptr<RegularWifiMac> mac = DynamicCast<RegularWifiMac>(device->GetMac());
```

이제 regular-wifi-mac.h를 보면 다음과 같이 멤버변수에 대한 선언과 그 멤버변수에 대한 Get 함수가 있다.

```cpp
    Ptr<DcaTxop> m_dca;
    Ptr<DcaTxop> GetDcaTxop (void) const;
```

그런데 여기서 한가지, GetDcaTxop 함수가 protected로 선언이 되어있다. 우리는 외부에서 GetDcaTxop 함수를 접근할 것이므로,
이 함수를 public 부분으로 옮겨준다.

이제 DcaTxop 클래스도 접근이 가능해졌다.

```cpp
    Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(devices.Get(0));
	Ptr<RegularWifiMac> mac = DynamicCast<RegularWifiMac>(device->GetMac());
	Ptr<DcaTxop> dca = mac->GetDcaTxop();
```
	
이제 dca-txop.h 안에 정의되어있는 함수들을 사용해서 원하는 목적을 이룰 수 있다. 이 때, 모든 노드에 대해서 MinCw 값을 바꿔주기 위해서는
for loop을 이용해 각각의 노드에 대해 처리해준다.

```cpp
    for(uint16_t i=0; i<2; i++) {
        Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(devices.Get(i));
        Ptr<RegularWifiMac> mac = DynamicCast<RegularWifiMac>(device->GetMac());
        Ptr<DcaTxop> dca = mac->GetDcaTxop();
        dca->SetMinCw(2);
    }
```	

이제 시뮬레이션을 수행하여 결과를 확인한다. 또한, SetMinCw의 인자를 바꿔보면서 성능이 어떻게 달라지는지도 확인한다.




