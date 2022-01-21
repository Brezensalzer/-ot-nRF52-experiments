# Commissioning a thread device with the CLI
## OpenThread Router RCP:

```
atom:~# ot-ctl
> dataset
Active Timestamp: 1
Channel: 15
Channel Mask: 0x07fff800
Ext PAN ID: 1111111122222222
Mesh Local Prefix: fd7d:35df:8a2d:c3f6::/64
Network Key: 00112233445566778899aabbccddeeff
Network Name: OT-Brezensalzer
PAN ID: 0x1234
PSKc: 0bda9bdc50d25c4890943d167a4275c7
Security Policy: 672 onrcb
Done
> state
leader
Done
> commissioner start
Done
> commissioner state
active
Done
> commissioner joiner add * J01NME
Done

=> after the join is started on the other node

> ~ Discovery Request from 46d519422b33d548: version=3,joiner=1
Commissioner: Joiner start eaa058a12cce06be
Commissioner: Joiner connect eaa058a12cce06be
Commissioner: Joiner finalize eaa058a12cce06be
Commissioner: Joiner end eaa058a12cce06be

```

## OpenThread Joiner:
```
tardis:~$ screen /dev/ttyACM1 115200
> state
disabled
Done
> ifconfig up
Done
> ipaddr
fe80:0:0:0:7cfa:9838:b4c5:9dea
Done
> scan
| J | Network Name     | Extended PAN     | PAN  | MAC Address      | Ch | dBm | LQI |
+---+------------------+------------------+------+------------------+----+-----+-----+
| 1 | OT-Brezensalzer  | 1111111122222222 | 1234 | 06c5d074168fe848 | 15 | -79 |  56 |
Done
> panid 0xffff      // Important filter!
Done

==> after the commisssioner is active on the RCP

> joiner start J01NME
Done
> Join success
> thread start
Done
> state
child
Done
```

