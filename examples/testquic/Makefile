#
#  ターゲットの指定（Makefile.targetで上書きされるのを防ぐため）
#
all:

#
#  アプリケーションファイル
#
APPLNAME = testquic
USE_CXX = true
APPL_CFG = $(APPLNAME).cfg

DEBUG = true
OMIT_OPTIMIZATION = true

#
#  ライブラリのディレクトリの定義
#
MBED_LIB_DIR = ../../mbed-lib
WOLFSSL_ROOT_DIR = ../../wolfSSL-lib
WOLFSSL_LIB_DIR = $(WOLFSSL_ROOT_DIR)/wolfssl
PICOTLS_ROOT_DIR = ../../picotls-lib
PICOTLS_LIB_DIR = $(PICOTLS_ROOT_DIR)/picotls
MINICRYPTO_LIB_DIR = $(PICOTLS_LIB_DIR)/deps
PICOQUIC_ROOT_DIR = ../../picoquic-lib
PICOQUIC_LIB_DIR = $(PICOQUIC_ROOT_DIR)/picoquic

#
#  ASPソースファイルのディレクトリの定義
#
SRCDIR = ../../asp-1.9.2-utf8

#
#	実行形式
#
EXECUTE_ON = ROM


#
#	quic用ライブラリ
#
include $(PICOTLS_ROOT_DIR)/Makefile
include $(PICOQUIC_ROOT_DIR)/Makefile

#
#	picoquicテスト用ライブラリ
#
include $(PICOQUIC_ROOT_DIR)/Makefile.test

#
#  wolfSSLライブラリ
#
include $(WOLFSSL_ROOT_DIR)/Makefile

#
#  MBEDライブラリのビルド
#
include $(MBED_LIB_DIR)/common/Makefile.cmn
include $(MBED_LIB_DIR)/mbed-src/Makefile.src
include $(MBED_LIB_DIR)/EthernetInterface/Makefile.eif
include $(MBED_LIB_DIR)/mbed-rpc/Makefile.rpc
include $(MBED_LIB_DIR)/HttpServer_snapshot/Makefile.hss
include $(MBED_LIB_DIR)/FATFileSystem/Makefile.ffs
include $(MBED_LIB_DIR)/i2c/Makefile.i2c
include $(MBED_LIB_DIR)/Makefile.mbd

#
#  ASPカーネルライブラリ
#
KERNEL_LIB = .
include $(SRCDIR)/Makefile.asp

