#
#
#

PICOTLS_OBJ += $(PICOTLS_LIB_DIR)/lib/openssl.o \
$(PICOTLS_LIB_DIR)/lib/picotls.o \
$(PICOTLS_LIB_DIR)/lib/minicrypto-pem.o \
$(PICOTLS_LIB_DIR)/lib/cifra.o \
$(PICOTLS_LIB_DIR)/lib/wolfcrypt.o \
$(PICOTLS_LIB_DIR)/lib/pembase64.o \
$(PICOTLS_LIB_DIR)/lib/uecc.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/aes.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/blockwise.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/chacha20.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/chash.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/curve25519.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/drbg.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/hmac.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/gcm.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/gf128.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/modes.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/poly1305.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/sha256.o \
$(MINICRYPTO_LIB_DIR)/cifra/src/sha512.o \
$(MINICRYPTO_LIB_DIR)/micro-ecc/uECC.o 

INCLUDE_PATHS += -I$(PICOQUIC_ROOT_DIR)/ -I$(PICOTLS_ROOT_DIR)/ -I$(PICOTLS_LIB_DIR)/include/ -I$(PICOTLS_LIB_DIR)/deps/cifra/src/ext/ -I$(PICOTLS_LIB_DIR)/deps/cifra/src/ -I$(PICOTLS_LIB_DIR)/deps/micro-ecc/

#CC_SYMBOLS += -DPICOTLS_USER_SETTINGS

C_OBJ += $(PICOTLS_OBJ)

