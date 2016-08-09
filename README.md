# What's this?

[TOPPERS/ASP Kernel](http://toppers.jp/en/asp-kernel.html) and useful libraries for [GR-PEACH](http://gadget.renesas.com/en/product/peach.html) have been distributed here.

![Stack](https://raw.github.com/wiki/ncesnagoya/asp-gr_peach_gcc-mbed/images/stack.png)



Build Environment in Travis | Status
------------ | -------------
Ubuntu 14.04.3 LTS | [![Build Status](https://travis-ci.org/ncesnagoya/asp-gr_peach_gcc-mbed.svg?branch=master)](https://travis-ci.org/ncesnagoya/asp-gr_peach_gcc-mbed)

## TOPPERS/ASP Kernel

TOPPERS/ASP kernel is a sophisticated real-time kernel for embedded real-time systems.
ASP stands for Advanced Standard Profile which is improved version of the standard profile defined in μITRON4.0 specification. 

TOPPERS/ASP kernel has been distributed with [TOPPERS License](http://toppers.jp/en/license.html).
The detail specification of TOPPERS/ASP kernel can be found at Refereces in this page.

## Libraries

Application developers can use the following useful libraries.

- MBED Library (mbed-lib) [MIT License]
 - HAL(Hardware Abstruction Layer) (mbed-src)
 - Ethernet Interface (EthernetInterface)
   - [LwIP](https://en.wikipedia.org/wiki/LwIP)[GPLv2] is included.
 - FAT File System (FATFileSystem)
   - [FatFs] (http://elm-chan.org/fsw/ff/00index_e.html)[[BSD-style License ](http://elm-chan.org/fsw/ff/en/appnote.html#license)] is included.
 - HTTP Server (HttpServer_snapshot)
 - RPC(Remote Procedure Call) (mbed-rpc)
 - I2C (i2c)
 - Softwere PWM (SoftPRM)

- Arduino libaray based on MBED library (arduino-lib) [GPLv2]
 - Digital I/O
 - Analog I/O
 - External Interrupts
 - Ethernet Client
 - LiquidCrystal
 - SD
 - Wire
 - RTC
 - Not supported: EEPROM, Ethernet Server, Firmata, GSM, Servo, SoftwareSerial, Stepper, USB host

# Software Development Enviroment

- Web compiler provided by RENESAS (Recommended for beginners)
 - You can get started programming without installing software in your computer.
 - No high functional debugging such like breakpoints, step-by-step execution, etc. 
- Command Line
 - Install development tools including arm cross-compilers and source code in your computer.
 - Please read Installation Guide for more details.
- IDE
 - TrueStudio or Eclipse can be used to perform high functional debugging.
 - Install manuals will be provided here in near future.
 
# Documents

## For web compiler-based development

- [Get Started for beginners](https://github.com/ncesnagoya/asp-gr_peach_gcc-mbed/wiki/GetStarted)

## For command Line-based development

- [Installation and Building Guide](https://github.com/ncesnagoya/asp-gr_peach_gcc-mbed/wiki/InstallationAndBuilding)

## For all developpers

- [Example programs](https://github.com/ncesnagoya/asp-gr_peach_gcc-mbed/wiki/ExamplePrograms) 

# Refereces and Links

## References

### TOPPERS/ASP Kernel / μITRON Specification

- [TOPPERS/ASP Kernel Specification](https://github.com/ncesnagoya/asp-gr_peach_gcc-mbed/blob/master/docs/TOPPERS_ASP_Kernel_Specification.pdf)
- [TOPPERS Next Generaton Specification](http://toppers.jp/documents.html) (Original, but Japanese only)
- [µITRON4.0 Specification](http://www.ertl.jp/ITRON/SPEC/mitron4-e.html)

### Library

- [Arduino Reference](https://www.arduino.cc/en/Reference/HomePage)
- [MBED Handbook](https://developer.mbed.org/handbook/Homepage)

### GR-PEACH

- [Official site (Renesas)](http://gadget.renesas.com/en/product/peach.html)
- [Official site (Core)](http://www.core.co.jp.e.ko.hp.transer.com/product/m2m/gr-peach/index.html)
- [MBED site](https://developer.mbed.org/platforms/Renesas-GR-PEACH/)
  - The original MBED library for GR-PEACH can be downloaded here.

## Other Links

- [TOPPERS Project](http://toppers.jp/en)
- [Gadget RENESAS](http://gadget.renesas.com/en/)
- [GNU ARM Eclipse](http://gnuarmeclipse.github.io/toolchain/install/)

# Contacts

Please feel free to ask me or send pull requests if you have any questions or requests.
Pull requests and contributions to this project are welcome anytime!
