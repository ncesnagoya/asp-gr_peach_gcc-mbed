#
#  ターゲットの指定（Makefile.targetで上書きされるのを防ぐため）
#
all:

#
#  アプリケーションファイル
#
APPLNAME = camtank
USE_CXX = true
APPL_CFG = $(APPLNAME).cfg

APPL_CXXOBJS += task_timelapse.o

#
#  MBEDライブラリのディレクトリの定義
#
MBED_LIB_DIR = ../../mbed-lib

#
#  ASPソースファイルのディレクトリの定義
#
SRCDIR = ../../asp-1.9.2-utf8

#
#  MBEDライブラリのビルド
#
include $(MBED_LIB_DIR)/common/Makefile.cmn
include $(MBED_LIB_DIR)/mbed-src/Makefile.src
include $(MBED_LIB_DIR)/EthernetInterface/Makefile.eif
include $(MBED_LIB_DIR)/GraphicsFramework/Makefile.grf
include $(MBED_LIB_DIR)/GR-PEACH_Camera/Makefile.cam
include $(MBED_LIB_DIR)/GR-PEACH_video/Makefile.vid
include $(MBED_LIB_DIR)/GR-PEACH_WlanBP3595STA/Makefile.lan
include $(MBED_LIB_DIR)/HttpServer_snapshot/Makefile.hss
include $(MBED_LIB_DIR)/i2c/Makefile.i2c
include $(MBED_LIB_DIR)/mbed-rpc/Makefile.rpc
include $(MBED_LIB_DIR)/R_BSP/Makefile.bsp
include $(MBED_LIB_DIR)/FATFileSystem/Makefile.ffs
include $(MBED_LIB_DIR)/SDFileSystem/Makefile.sdf
include $(MBED_LIB_DIR)/Makefile.mbd

APPL_LIBS += $(MBED_LIB_DIR)/GR-PEACH_WlanBP3595STA/WlanBP3595STA/TOOLCHAIN_GCC_ARM/libWlanBP3595STA.a

#
#  ASPカーネルライブラリ
#
KERNEL_LIB = .
include $(SRCDIR)/Makefile.asp
