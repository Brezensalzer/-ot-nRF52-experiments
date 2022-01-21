# Build and flash the OpenThread Firmware on the Makerdiary nRF52840-mdk Dongle
## Router-Coprocessor RCP-Firmware

**Guide: https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/openthread_rcp_nrf_dongle.md**

```
stl@raspi4:~/github $ git clone --recursive https://github.com/openthread/ot-nrf528xx.git
stl@raspi4:~/github $ cd ot-nrf528xx
stl@raspi4:~/github/ot-nrf528xx $ ./script/bootstrap
stl@raspi4:~/github/ot-nrf528xx $ script/build nrf52840 USB_trans -DOT_BOOTLOADER=USB -DOT_THREAD_VERSION=1.2
stl@raspi4:~/github/ot-nrf528xx $ arm-none-eabi-objcopy -O ihex build/bin/ot-rcp build/bin/ot-rcp.hex
```

* the uf2-Bootloader is already installed on the dongle
* uf2conv.py https://github.com/microsoft/uf2/blob/master/utils/uf2conv.py
* copy to ~/.local/bin
* read https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/programming/#dfu-via-uf2-bootloader

```
stl@raspi4:~ $ cd github/ot-nrf528xx/build/bin/
stl@raspi4:~/github/ot-nrf528xx/build/bin $ uf2conv.py ot-rcp.hex -c -f 0xADA52840
Converted to uf2, output size: 141824, start address: 0x1000
Wrote 141824 bytes to flash.uf2
```

* copy flash.uf2 on the flash disk of the dongle

## Full-Thread-Device FTD-Firmware
```
stl@raspi4:~/github $ git clone --recursive https://github.com/openthread/ot-nrf528xx.git
stl@raspi4:~/github $ cd ot-nrf528xx
stl@raspi4:~/github/ot-nrf528xx $ ./script/bootstrap
stl@raspi4:~/github/ot-nrf528xx $ script/build nrf52840 USB_trans -DOT_BOOTLOADER=USB -DOT_THREAD_VERSION=1.2 -DOPENTHREAD_FTD=1
stl@raspi4:~/github/ot-nrf528xx $ arm-none-eabi-objcopy -O ihex build/bin/ot-cli-ftd build/bin/ot-cli-ftd.hex
```
```
stl@raspi4:~ $ cd github/ot-nrf528xx/build/bin/
stl@raspi4:~/github/ot-nrf528xx/build/bin $ uf2conv.py ot-cli-ftd.hex -c -f 0xADA52840
Converted to uf2, output size: 964159, start address: 0x1000
Wrote 964159 bytes to flash.uf2
```
* copy flash.uf2 on the flash disk of the dongle