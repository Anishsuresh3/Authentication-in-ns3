# Authentication-in-ns3
Implemented Open Auth and Shared Key Authentication in ns-3

**Usage**
------------------------------------------
Put the Sample script auth-example.cc in ns-3/scratch directory

Run command
./ns3 run scratch/auth-example.cc

Code Usage
For Open Auth:

STA mac:

mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false), "OpenAuth", BooleanValue(true));

AP mac:

mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid),"SharedKeyAuth", "OpenAuth", BooleanValue(true));


For Shared Key Auth:

STA mac:

mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false),"SharedKeyAuth",StringValue("passWoRD!!"));

AP mac:

mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid),"SharedKeyAuth",StringValue("passWoRD!!"));

**Dependency**
------------------------------------------
Install ns-3 allinone software
Install Openssl and compile it

**Integration**
------------------------------------------
Download and place the files in ns-3/src/wifi/model



