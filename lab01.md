## Lab-01. ns-3 설치와 실행

이 실습에서는, 리눅스 머신에 ns-3를 설치하고, 시뮬레이션을 실행해본다.  
참고로 이 실습과 이후의 실습들은 ns-3.27 버전을 기반으로 작성하였다.

### ns-3 설치

(1) ns-3를 웹으로부터 다운로드받는다.

```
wget http://nsnam.org/release/ns-allinone-3.27.tar.bz2
```

(2) 압축을 푼다.

```
tar xjf ns-allinone-3.27.tar.bz2
```

(3) 코드를 빌드한다.

```
cd ns-allinone-3.27
./build.py
```

만약 빌드가 실패하면, 필요한 라이브러리가 없어서일 가능성이 크다. 이런 경우에 필요한 라이브러리가 다 설치되었는지
[이 사이트](https://www.nsnam.org/wiki/Installation#Prerequisites)를 참고하여 확인하고 필요하면 설치해준다.

(4) 예제코드를 실행해본다.

```
cd ns-3.27
cp examples/wireless/wifi-simple-adhoc.cc scratch
```

시뮬레이션 스크립트는 ```scratch``` 디렉토리에 들어가야 한다.

```
./waf --run scratch/wifi-simple-adhoc
```

만약 실행 후 아래와 같은 메시지가 출력된다면, ns-3 설치에 성공한 것이다.

```
Testing 1 packets sent with receiver rss -80
Received one packet!
```

[다음 실습](lab02.md)에서는, 무선랜 시뮬레이션 스크립트를 이해하도록 한다.





