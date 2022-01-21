# Installation of the OpenThread-Border-Router on Raspberry Pi and Debian x64

## Raspberry Pi
* plug the prepared nRF52840 dongle into an USB port.
* check /dev/serial/by-id for the new interface:
`stl@raspi4:~$ ls /dev/serial/by-id/
usb-Nordic_Semiconductor_nRF528xx_OpenThread_Device_EA27A5FC585E-if01`
* use this distinct interface and not /dev/ttyAMC\* ! 
* the OpenThread WebGUI uses port 80/tcp, so stop any other Webserver

```
stl@raspi4:~$ cd github
stl@raspi4:~/github$ git clone https://github.com/openthread/ot-br-posix.git
stl@raspi4:~/github$ cd ot-br-posix
stl@raspi4:~/github/ot-br-posix$ ./script/bootstrap
stl@raspi4:~/github/ot-br-posix$ INFRA_IF_NAME=eth0 ./script/setup

stl@raspi4:~/github/ot-br-posix $ cat /etc/default/otbr-agent 
# Default settings for otbr-agent. This file is sourced by systemd

# Options to pass to otbr-agent
OTBR_AGENT_OPTS="-I wpan0 -B eth0 spinel+hdlc+uart:///dev/serial/by-id/usb-Nordic_Semiconductor_nRF528xx_OpenThread_Device_EA27A5FC585E-if01 trel://eth0"

stl@raspi4:~/github/ot-br-posix $ sudo su -

root@raspi4:~# systemctl start mdns.service
root@raspi4:~# systemctl status mdns.service 
● mdns.service
     Loaded: loaded (/etc/init.d/mdns; generated)
     Active: active (running) since Sun 2022-01-16 09:50:14 CET; 2s ago

root@raspi4:~# systemctl start otbr-web.service
root@raspi4:~# systemctl status otbr-web.service 
● otbr-web.service - Border Router Web
     Loaded: loaded (/lib/systemd/system/otbr-web.service; enabled; vendor preset: enabled)
     Active: active (running) since Sun 2022-01-16 09:51:47 CET; 2s ago

root@raspi4:~# systemctl start otbr-agent.service
root@raspi4:~# systemctl status otbr-agent.service 
● otbr-agent.service - Border Router Agent
     Loaded: loaded (/lib/systemd/system/otbr-agent.service; enabled; vendor preset: enabled)
     Active: active (running) since Sun 2022-01-16 10:40:08 CET; 2s ago
```

**=> WebGUI on http://localhost available!**

## Installation on Debian x64
After cloning of the git repository, you have to edit one file. Otherwise the WebGUI will not be built.
```
stl@atom: vim ~/github/ot-br-posix/examples/platforms/debian/default
...
NAT64=0
DNS64=0
DHCPV6_PD=0
NETWORK_MANAGER=0
BACKBONE_ROUTER=1
BORDER_ROUTING=1
REST_API=1
WEB_GUI=1       <= add this line!
```

Then the procedure is identical to the Raspberry Pi, but you have to look for LAN interface. In my case it is *eno1* and not *eth0*.

