### Lab-05. 아이디어 도출

이번 실습에서는 특정 실험환경을 예로 들어 어떻게 하면 전송량과 공평도를 높일 수 있을지 고민해본다.

---

#### 05.01. 시뮬레이션 환경 구성

이번 실습에 사용할 시뮬레이션 스크립트는 script05.cc이다. 대부분의 내용은 지난 실습에서 다뤘던
것과 유사하고, 여기에 실험 수행과 결과 분석을 쉽게 하기 위한 기능이 추가되었다. 추가된 내용들은
다음과 같다.

(1) Topology를 tex 파일로 저장

ExportTopologyToLatex라는 함수가 추가되었다. 이 함수를 호출하면, 현재의 AP와 station의
위치를 tex 파일 포맷으로 출력해준다. 이 파일은 latex를 이용하여 PDF로 바꾸어 확인할 수 있는데,
PDF로 바꾸는 부분까지를 한꺼번에 실행하기 위해서는 다음의 패키지들이 설치되어있어야 한다.

- texlive-full
- ghostscript

이 기능을 사용할 경우 아래와 같은 tex 파일이 생성된다.

```
\documentclass{article}
\usepackage{epsfig}
\usepackage{color}
\usepackage{epic}
\usepackage{nopageno}
\definecolor{black}{rgb}{0,0,0}
\definecolor{red}{rgb}{1,0,0}
\definecolor{gray}{rgb}{0.5,0.5,0.5}
\definecolor{darkgray}{rgb}{0.25,0.25,0.25}
\definecolor{green}{rgb}{0,1,0}
\definecolor{blue}{rgb}{0,0,1}
\begin{document}
\setlength{\unitlength}{0.2mm}
\begin{picture}(500,500)
\thicklines
\put(0,0){\line(1,0){500}}
\put(500,0){\line(0,1){500}}
\put(500,500){\line(-1,0){500}}
\put(0,500){\line(0,-1){500}}
\put(166,0){\line(0,1){500}}
\put(332,0){\line(0,1){500}}
\put(0,166){\line(1,0){500}}
\put(0,332){\line(1,0){500}}
\thinlines\color{blue}\put( 83, 83){\circle*{10}}
\thicklines\color{blue}\put( 88, 88){\small 1}
\thinlines\color{blue}\put(250, 83){\circle*{10}}
\thicklines\color{blue}\put(255, 88){\small 2}
\thinlines\color{blue}\put(416, 83){\circle*{10}}
\thicklines\color{blue}\put(421, 88){\small 3}
\thinlines\color{blue}\put( 83,250){\circle*{10}}
\thicklines\color{blue}\put( 88,255){\small 4}
\thinlines\color{blue}\put(250,250){\circle*{10}}
\thicklines\color{blue}\put(255,255){\small 5}
\thinlines\color{blue}\put(416,250){\circle*{10}}
\thicklines\color{blue}\put(421,255){\small 6}
\thinlines\color{blue}\put( 83,416){\circle*{10}}
\thicklines\color{blue}\put( 88,421){\small 7}
\thinlines\color{blue}\put(250,416){\circle*{10}}
\thicklines\color{blue}\put(255,421){\small 8}
\thinlines\color{blue}\put(416,416){\circle*{10}}
\thicklines\color{blue}\put(421,421){\small 9}
\thinlines\color{black}\put(117,116){\circle*{10}}
\thicklines\color{black}\put(122,121){\small 10}
\thinlines\color{black}\put(270,149){\circle*{10}}
\thicklines\color{black}\put(275,154){\small 11}
\thinlines\color{black}\put(418, 25){\circle*{10}}
\thicklines\color{black}\put(423, 30){\small 12}
\thinlines\color{black}\put( 22,201){\circle*{10}}
\thicklines\color{black}\put( 27,206){\small 13}
\thinlines\color{black}\put(190,188){\circle*{10}}
\thicklines\color{black}\put(195,193){\small 14}
\thinlines\color{black}\put(386,264){\circle*{10}}
\thicklines\color{black}\put(391,269){\small 15}
\thinlines\color{black}\put(141,371){\circle*{10}}
\thicklines\color{black}\put(146,376){\small 16}
\thinlines\color{black}\put(190,384){\circle*{10}}
\thicklines\color{black}\put(195,389){\small 17}
\thinlines\color{black}\put(344,394){\circle*{10}}
\thicklines\color{black}\put(349,399){\small 18}
\end{picture}
\end{document}
```

그리고 이것을 PDF로 바꾸면 다음과 같은 그림을 얻을 수 있다.

<center><img src="./topology.png" width="400" height="400"></center>
