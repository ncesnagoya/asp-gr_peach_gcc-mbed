#
#
#

PICOTLS_OBJ += $(PICOTLS_LIB_DIR)/lib/openssl.o \
$(PICOTLS_LIB_DIR)/lib/picotls.o \
$(PICOTLS_LIB_DIR)/lib/ffx.o \
$(PICOTLS_LIB_DIR)/lib/wolfcrypt.o 

INCLUDE_PATHS += -I$(PICOQUIC_ROOT_DIR)/ -I$(PICOTLS_ROOT_DIR)/ -I$(PICOTLS_LIB_DIR)/include/ -I$(PICOTLS_LIB_DIR)/deps/cifra/src/ext/ -I$(PICOTLS_LIB_DIR)/deps/cifra/src/ -I$(PICOTLS_LIB_DIR)/deps/micro-ecc/

#CC_SYMBOLS += -DPICOTLS_USER_SETTINGS

C_OBJ += $(PICOTLS_OBJ)
