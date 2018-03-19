## Lab-01. Installing and Running ns-3  

In this lab, you will install ns-3 on your Linux machine, and run your first simulation.  

### Install ns-3 on your machine

(1) Download ns-3 from the web.

```
wget http://nsnam.org/release/ns-allinone-3.27.tar.bz2
```

(2) Extract ns-3 code from the zip file.

```
tar xjf ns-allinone-3.27.tar.bz2
```

(3) Build the code.

```
cd ns-allinone-3.27
./build.py
```

If the build fails, you may need to [check](https://www.nsnam.org/wiki/Installation#Prerequisites) 
whether all required libraries are installed.  

(4) Try running an example code.

```
cd ns-3.27
cp examples/wireless/wifi-simple-adhoc.cc scratch
```

The simulation script should go into `scratch` directory.

```
./waf --run scratch/wifi-simple-adhoc
```

If you see the following message, the simulation is complete and the installation is successful.

```
Testing 1 packets sent with receiver rss -80
Received one packet!
```


