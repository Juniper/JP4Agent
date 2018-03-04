JP4Agent documentation


## Working with VMX

### Login to VCP
```
Console:
telnet localhost 8601
Username: root
Password: JunAFI

Ssh:
ssh root@<vcp ip>
Password: JunAFI
```

### Login to VFP
```
Console:
telnet localhost 8602     <<< Console
Username: root
Password: root

Ssh:
ssh pfe@<vfp ip>
Password: pfe

After logging as user 'pfe', to become root do 'su' (root password: root)
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

