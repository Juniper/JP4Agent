JP4Agent documentation


## Working with VMX

### Login to VCP
```
telnet localhost 8601     <<< Console
or
ssh root@<vcp ip>
Username: root
Password: JunAFI
```

### Login to VFP
```
telnet localhost 8602     <<< Console
or
ssh root@<vfp ip>
Username: pfe
Password: pfe
Root password: root
```


### Configuring forwarding sandbox
```
root@6a7738d10b2a:~# ssh root@<vcp ip>
Password: 
Last login: Sat Dec 23 12:20:58 2017 from 172.18.0.4
--- JUNOS 17.3I20171118_2053_sksodhi Kernel 64-bit  JNPR-10.3-20170605.150032_fbsd-
root@:~ # cli
root> configure 
Entering configuration mode

[edit]
root# set forwarding-options forwarding-sandbox jp4agent port p1 interface xe-0/0/0:1 

[edit]
root# set forwarding-options forwarding-sandbox jp4agent port p2 interface xe-0/0/0:2 

[edit]
root#
root# show | compare 
[edit]
+  forwarding-options {
+      forwarding-sandbox jp4agent {
+          port p1 {
+              interface xe-0/0/0:1;
+          }
+          port p2 {
+              interface xe-0/0/0:2;
+          }
+      }
+  }

[edit]
root# commit 
commit complete

[edit]
root#
```

