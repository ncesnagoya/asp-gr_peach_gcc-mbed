#
#  ターゲットの指定（Makefile.targetで上書きされるのを防ぐため）
#
all:

#
#  アプリケーションファイル
#
APPLNAME = multitask_arduino
USE_CXX = true
#APPL_CXXOBJS += $(APPLNAME).o
#APPL_COBJS   += 
APPL_CFG = $(APPLNAME).cfg

DEBUG = true
OMIT_OPTIMIZATION = true

#
#  MBEDライブラリのディレクトリの定義
#
MBED_LIB_DIR = ../../mbed-lib

#
#  MBEDライブラリのビルド
#
include $(MBED_LIB_DIR)/common/Makefile.cmn
include $(MBED_LIB_DIR)/arduino-lib/Makefile.ard # 同名のヘッダファイルがあるのでmbed-srcよりも前で読み込む
include $(MBED_LIB_DIR)/mbed-src/Makefile-Arduino.src # Makefile.src の代わりにこちらを使用
include $(MBED_LIB_DIR)/EthernetInterface/Makefile.eif
include $(MBED_LIB_DIR)/Makefile.mbd

#
#  ASPソースファイルのディレクトリの定義
#
SRCDIR = ../../asp-1.9.2-utf8

#
#  ASPカーネルのビルドに関するその他のオプション
#
APPL_CFLAGS =
APPL_LIBS = -lmbed
APPL_DIR = .

INCLUDES += $(INCLUDE_PATHS) # MBEDライブラリのヘッダファイルをアプリで使う
COPTS += -mfpu=vfpv3 -mfloat-abi=hard
CDEFS += -DTOPPERS_OMIT_BSS_INIT -DTOPPERS_OMIT_DATA_INIT # ASPのスタートアップルーチンでは初期化処理は不要
CDEFS += -DGRPEACH

APPL_CXXFLAGS += $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti
CXXLIBS += $(LD_SYS_LIBS)
LDFLAGS += $(LD_FLAGS)
LDSCRIPT = $(LINKER_SCRIPT)

#
#  ASPカーネルライブラリ
#
KERNEL_LIB = .
include $(SRCDIR)/Makefile.asp
