#
#  ターゲットの指定（Makefile.targetで上書きされるのを防ぐため）
#
all:

#
#  アプリケーションファイル
#
APPLNAME = blinky
USE_CXX = true
#APPL_CXXOBJS += $(APPLNAME).o
#APPL_COBJS   += 
APPL_CFG = $(APPLNAME).cfg

#
#  Serialの扱い
#   ARDUINO_SERIAL
#    NOUSE_SERIAL
#    USE_SERIAL
#    USE_SERIALUSB_AS_SERIAL
#    USE_SERIAL5_AS_SERIAL
#
#ARDUINO_SERIAL = NOUSE_SERIAL
#ARDUINO_SERIAL = USE_SERIAL
#ARDUINO_SERIAL = USE_SERIALUSB_AS_SERIAL
#ARDUINO_SERIAL = USE_SERIAL5_AS_SERIAL

#
#  ライブラリの使用
#  
#USE_ARDUINO_SPI = true
#USE_ARDUINO_NCESCAN = true
#USE_ARDUINO_SD  = true
#USE_ARDUINO_TFT = true
#USE_ETHERNET2 = true
#USE_NAXESMOTION = true
#USE_ARDUINO_RTC = true
#USE_WIRE = true

DEBUG = true
OMIT_OPTIMIZATION = true

#
#  MBEDライブラリのディレクトリの定義
#
MBED_LIB_DIR = ../../mbed-lib

#
#  MBEDライブラリのビルド
#
include $(MBED_LIB_DIR)/Makefile

#
#  ASPソースファイルのディレクトリの定義
#
SRCDIR = ../../asp-1.9.2-utf8

#
#  その他のオプション
#
APPL_CFLAGS =
APPL_LIBS = -lmbed
APPL_DIR = .

INCLUDES += $(INCLUDE_PATHS) # MBEDライブラリのヘッダファイルをアプリで使う
COPTS += -mfpu=vfpv3 -mfloat-abi=hard
CDEFS += -DTOPPERS_OMIT_BSS_INIT -DTOPPERS_OMIT_DATA_INIT # ASPのスタートアップルーチンでは初期化処理は不要

APPL_CXXFLAGS += $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti
CXXLIBS += $(LD_SYS_LIBS)
LDFLAGS += $(LD_FLAGS)
LDSCRIPT = $(LINKER_SCRIPT)

#
#  ASPカーネルライブラリ
#
KERNEL_LIB = .
include ./Makefile.asp
