#
#  ターゲットの指定（Makefile.targetで上書きされるのを防ぐため）
#
all:

#
#  アプリケーションファイル
#
APPLDIR = ../../camtank
APPLNAME = camtank
USE_CXX = true
APPL_CFG = $(APPLNAME).cfg
USE_TRUESTUDIO = true

APPL_CXXOBJS = $(APPLNAME).o
CXX_OBJ += task_timelapse.o

DEBUG = true
OMIT_OPTIMIZATION = true

#
#  MBEDライブラリのディレクトリの定義
#
MBED_LIB_DIR = ../../../mbed-lib

#
#  ASPソースファイルのディレクトリの定義
#
SRCDIR = ../../../asp-1.9.2-utf8

#
#  MBEDライブラリのビルド
#
include $(MBED_LIB_DIR)/mbed-src/Makefile.src
include $(MBED_LIB_DIR)/EthernetInterface/Makefile.eif
include $(MBED_LIB_DIR)/FATFileSystem/Makefile.ffs
include $(MBED_LIB_DIR)/GraphicsFramework/Makefile.grf
include $(MBED_LIB_DIR)/GR-PEACH_Camera/Makefile.cam
include $(MBED_LIB_DIR)/GR-PEACH_video/Makefile.vid
include $(MBED_LIB_DIR)/GR-PEACH_WlanBP3595STA/Makefile.lan
include $(MBED_LIB_DIR)/HttpServer_snapshot/Makefile.hss
include $(MBED_LIB_DIR)/i2c/Makefile.i2c
include $(MBED_LIB_DIR)/mbed-rpc/Makefile.rpc
include $(MBED_LIB_DIR)/R_BSP/Makefile.bsp
include $(MBED_LIB_DIR)/SDFileSystem/Makefile.sdf
include $(MBED_LIB_DIR)/common/Makefile.cmn
include ../Makefile.mbd


#
#  ASPカーネルライブラリ
#
KERNEL_LIB = .
include ../Makefile.asp

$(ALL_OBJ): kernel_cfg.h
