#
#  ターゲットの指定（Makefile.targetで上書きされるのを防ぐため）
#
all:

#
#  アプリケーションファイル
#
APPLDIR = ../../multifile_arduino
APPLNAME = multifile_arduino
USE_CXX = true
APPL_CFG = $(APPLNAME).cfg
USE_TRUESTUDIO = true

DEBUG = true
OMIT_OPTIMIZATION = true

#
#  アプリケーションファイルに関する定義
#  メインファイル以外のソースファイルを記述する
#
APPL_ASMOBJS =
ifdef USE_CXX
  APPL_CXXOBJS = loop1.o loop2.o loop3.o
  APPL_COBJS =
else
  APPL_COBJS =
endif

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
include $(MBED_LIB_DIR)/common/Makefile.cmn
include $(MBED_LIB_DIR)/arduino-lib/Makefile.ard # 同名のヘッダファイルがあるのでmbed-srcよりも前で読み込む
include $(MBED_LIB_DIR)/mbed-src/Makefile-Arduino.src # Makefile.src の代わりにこちらを使用
include $(MBED_LIB_DIR)/EthernetInterface/Makefile.eif
include ../Makefile.mbd

#
#  ASPカーネルライブラリ
#
KERNEL_LIB = .
include ../Makefile.asp

$(ALL_OBJ): kernel_cfg.h

