
<br>
<div style="text-align:center" align="center"><img src ="../docs/resources/JP4Agent_layers.png" width="200"/></div>
<br>
<br>

### Compile jp4agent
```
cd build

To build JPAgent with aft target:
./make-all-aft.sh

To build JPAgent with Broadcom target:
1. Set the following environment variables:
   export BCM_HALP_DIR_PATH=<path to halp-directory>
   export BCM_SDK_DIR_PATH=<path to sdk-directory>

2. ./make-all-brcm.sh

To build JPAgent with null target:
./make-all-null.sh
```

### Run jp4agent
```
To run JPAgent (aft target):
cd src/targets/aft/bin
./run-jp4agent 

To run JPAgent (Broadcom target):
cd src/targets/brcm/bin
./run-jp4agent 

To run JPAgent (null target):
cd src/targets/null/bin
./run-jp4agent 

```
