##### Note: Use of this software is governed by an Apache 2.0 license, and can be found in the “LICENSE” file.

JP4Agent (Juniper P4 Agent)
==========================
Juniper's P4 Runtime server implementation.

<br>
<div style="text-align:center" align="center"><img src ="docs/resources/JP4Agent.png" width="150"/></div>
<br>
<br>


JP4Agent GitHub Repository
=====================
This repository provides:
* JP4Agent source code
* Dockerfile(s) to build Docker container where VMX and JP4Agent run

#### Repository Directory Structure
```
 -- JP4Agent
    |-- LICENSE                     License 
    |-- README.md                   This README file
    |-- docs                        Documentation
    |-- lib                         Library dependencies
    |-- p4                          Sample p4 programs
    |-- p4-backend                  P4 compiler backend for AFI
    |-- src                         JP4Agent source
    |-- test 
    |   |-- controller              Test controller
    |   `-- gtest                   GTests for JP4agent
    `-- tools
        `-- docker                  Files/scripts to build/setup Docker container/s
```

Requirements
=====================
#### Juniper Networks Software
VMX tarball provided by Juniper
```
jnprP4vmx.tgz    : Contains all the images/packages needed to run VMX in a container
```
Note: Please contact Juniper to get 'jnprP4vmx.tgz'. <br>
Contacts listed at the bottom of this page.

#### System Requirements
```
Hardware requirements
=====================
Processor: 
    Any x86 processor (Intel or AMD) with VT-d capability
Number of Cores: 
    4 (1 for VCP and 3 for VFP)
Memory:
    Minimum:
    8 GB (2 GB for VCP, 6 GB for VFP)
    Additional
    2 GB recommended for host OS
Storage: 
    Local or NAS. Minimum 100 GB is recommended.
       
Software requirements
=====================
Operating system
    Ubuntu 16.04 LTS
```

<br>
<br>
<div style="text-align:center" align="center"> <img src="docs/resources/docker-setup.png" width="800"> </div>
<br>
<br>

### STEP 1. Install Docker engine on Linux host server
Please refer to instructions provided on the following page to install Docker engine on the host server  
[https://docs.docker.com/engine/installation/linux/ubuntu/](https://docs.docker.com/engine/installation/linux/ubuntu/)


### STEP 2. Set Docker options

Add following line in /etc/default/docker file
```
DOCKER_OPTS="--bip=172.18.0.1/16"
```

and restart Docker service (sudo service docker restart)

The option '--bip=$DOCKER0_BRIDGE_IP' makes sure that Docker container uses $DOCKER0_BRIDGE_IP as IP for "docker0" bridge.

NOTE: 
By default, Docker uses location '/var/lib/docker' to install images. You can use '-g <location>' option to specify the Docker image installation location.  Please make sure that the location has around 200GB of space available.


### STEP 3. Build Docker container for JP4Agent and VMX
```
git clone git@github.com:Juniper/JP4Agent.git
cd JP4Agent/tools/docker/ 
Note: Please execute following steps in order.
[sudo] docker build -f Dockerfile_p4 -t juniper-p4 .
[sudo] docker build -f Dockerfile_p4_vmx -t juniper-p4-vmx .
```

### STEP 4. Get VMX tarball from Juniper and extract it
Please drop an email to  sksodhi at juniper.net to get 'jnprP4vmx.tgz'.
```
Get 'jnprP4vmx.tgz' tarball for Juniper and extract it to any 
preferred folder on the host.
E.g. -
export DIR_PATH=$HOME/VMX
mkdir $DIR_PATH
cp jnprP4vmx.tgz $DIR_PATH
cd $DIR_PATH
tar xf jnprP4vmx.tgz
```

### STEP 5. Run the container
```
[sudo] docker run --name jnprp4vmx -v $DIR_PATH:/root/VMX --privileged -i -t juniper-p4-vmx
Note:  $DIR_PATH is the absolute path of the folder where the tarball was extracted in previous step.
Example -
[sudo] docker run --name jnprp4vmx -v /home/sandesh/VMX:/root/VMX --privileged -i -t juniper-p4-vmx
```
#### After above step, you are in Docker container. Rest of the steps are performed inside the container

### STEP 6. Clone JP4Agent (inside container)
```
cd /root/
git clone git@github.com:Juniper/JP4Agent.git
```

### STEP 7. Build all JP4Agent components and binaries
```
cd JP4Agent 
./tools/docker/scripts/build-jp4.sh
```

### STEP 8. Start VMX
```
cd /root/VMX
../JP4Agent/tools/docker/scripts/setup_vmx.sh
```
Please wait for the following message to appear before proceeding with next step. <br>
"VMX Setup Complete!" <br>
Please allow around 15-20 minutes for VMX setup to complete. <br>

Note: Please go through "Working with VMX" section of '[**docs/README.md**](./docs/README.md)' to learn <br>
how to work with VMX setup, viz.,  how to login to VCP and VFP, run Junos CLI commands etc.

### STEP 9. Load Openconfig package
```
First copy openconfig package to VCP
scp /root/VMX/<junos-openconfig-xxx.tgz> root@172.18.0.10:/var/tmp

Then run following Junos CLI command to add Junos openconfig package
request system software add /var/tmp/<junos-openconfig-xxx.tgz>
```

### STEP 10. Add CLI configuration for sandbox and Openconfig
```
set forwarding-options forwarding-sandbox jp4agent port p1 interface xe-0/0/0:1
set forwarding-options forwarding-sandbox jp4agent port p2 interface xe-0/0/0:2
set system services netconf ssh

commit
```
<!--
### STEP XX. Allow JP4Agent packets
```
From vFP shell,
iptables -P INPUT ACCEPT
```
[Note: This needs to be improved to have a more specific rule]
-->

### STEP 11. Run jp4agent
```
cd JP4Agent/src
./run-jp4agent
```

### STEP 12. Run GTESTs
```
apt-get install -y tshark
cd JP4Agent/test/gtest
make
./run-gtests
```
Please observe the output of ./run-gtests and when the output shows '[ PASSED ]', you are good to go.

Contact
=======
<b>Sandesh Kumar Sodhi</b> <br>
sksodhi at juniper.net
