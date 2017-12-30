# P4 compiler backend for AFI 


## Compiling P4 compiler with AFI  backend

### STEP 1.
```
git clone --recursive https://github.com/p4lang/p4c.git
```


### STEP 2. Create symbolic link to afi backend
```
cd p4c
mkdir extensions
cd extensions
ln -s  /root/JP4Agent/p4c-backend/afi
```

### STEP 3. Create symbolic link to p4c.afi.cfg
```
cd p4c/tools/driver/p4c_src 
ln -s  /root/JP4Agent/p4c-backend/p4c.afi.cfg
```

### STEP 3. Make following p4c.afi.cfg to P4C_DRIVER_SRCS
```
Add p4c_src/p4c.afi.cfg to P4C_DRIVER_SRCS in file:p4c/tools/driver/CMakeLists.txt -

...<snip>...
set (P4C_DRIVER_SRCS
  p4c_src/main.py
  p4c_src/driver.py
  p4c_src/util.py
  p4c_src/config.py
  p4c_src/__init__.py
  p4c_src/p4c.bmv2.cfg
  p4c_src/p4c.ebpf.cfg
  p4c_src/p4c.afi.cfg   <<<<< Add this line
  )
...<snip>...
```

### STEP 4. Compile p4c
```
Please follow the steps provided at https://github.com/p4lang/p4c/ for compiling p4c.
Brief summary of steps (to be run after all dependencies have been installed):
mkdir build
cd build
cd p4c/build
cmake .. -DCMAKE_BUILD_TYPE=DEBUG
make -j4
make -j4 check
```

## Using p4 compiler with AFI  backend
```
cd p4c/build
./p4c -b afi-ss-juniper -x p4-14 <p4 program>
Example - 
./p4c -b afi-ss-juniper -x p4-14 simple_router.p4
```

<!--
root@1dccb7e7a3d7:~/p4c/build# ./p4c --target-help
Supported targets in "target-arch-vendor" triplet:
afi-ss-juniper
ebpf-v1model-p4org
bmv2-ss-p4org
bmv2-psa-p4org

Debug -
./p4c -v -Tcontrol:3 -b afi-ss-juniper -x p4-14 ./simple_router.p4
-->


## Contact
```
Sandesh Kumar Sodhi
sksodhi@juniper.net
```
