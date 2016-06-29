# What's this

[TOPPERS/ASP Kernel](http://toppers.jp/en/asp-kernel.html) and useful libraries for [GR-PEACH](http://gadget.renesas.com/en/product/peach.html) have been distributed here.


![Stack](https://raw.github.com/wiki/ncesnagoya/asp-gr_peach_gcc-mbed/images/stack.png)

## TOPPERS/ASP Kernel

TOPPERS/ASP kernel is a sophisticated real-time kernel for embedded real-time systems.
ASP stands for Advanced Standard Profile which is improved version of the standard profile defined in μITRON4.0 specification. 

The detail specification of TOPPERS/ASP kernel can be found at the following sites.

- [TOPPERS Next Generaton Specification](http://toppers.jp/documents.html) (Japanese only)
- [µITRON4.0 Specification](http://www.ertl.jp/ITRON/SPEC/mitron4-e.html) (English version)

TOPPERS/ASP kernel has been distributed with [TOPPERS License](http://toppers.jp/en/license.html).

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
 
# References

- [Installation Guide](https://github.com/ncesnagoya/asp-gr_peach_gcc-mbed/wiki/Installation)

# Links

- GR-PEACH
 - [Official](http://gadget.renesas.com/en/product/peach.html)
 - [MBED site](https://developer.mbed.org/platforms/Renesas-GR-PEACH/)
   - The original MBED library for GR-PEACH can be downloaded here.
- [TOPPERS Project](http://toppers.jp/en)
- [GNU ARM Eclipse](http://gnuarmeclipse.github.io/toolchain/install/)

# Contacts

Please feel free to ask me or send pull requests if you have any questions or requests.
