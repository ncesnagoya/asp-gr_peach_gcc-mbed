#
#
#

WOLFSSL_OBJ += $(WOLFSSL_LIB_DIR)/src/crl.o \
$(WOLFSSL_LIB_DIR)/src/internal.o \
$(WOLFSSL_LIB_DIR)/src/io.o \
$(WOLFSSL_LIB_DIR)/src/keys.o \
$(WOLFSSL_LIB_DIR)/src/ocsp.o \
$(WOLFSSL_LIB_DIR)/src/sniffer.o \
$(WOLFSSL_LIB_DIR)/src/ssl.o \
$(WOLFSSL_LIB_DIR)/src/tls.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/aes.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/arc4.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/asm.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/asn.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/blake2b.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/camellia.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/chacha.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/chacha20_poly1305.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/cmac.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/coding.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/compress.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/curve25519.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/des3.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/dh.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/dsa.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/ecc.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/ecc_fp.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/ed25519.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/error.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/fe_low_mem.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/fe_operations.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/ge_low_mem.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/ge_operations.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/hash.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/hc128.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/hmac.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/idea.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/integer.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/logging.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/md2.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/md4.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/md5.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/memory.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/pkcs7.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/poly1305.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/pwdbased.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/rabbit.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/random.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/ripemd.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/rsa.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/sha.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/sha256.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/sha512.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/signature.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/srp.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/tfm.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/wc_encrypt.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/wc_port.o \
$(WOLFSSL_LIB_DIR)/wolfcrypt/src/wolfevent.o 
#$(WOLFSSL_LIB_DIR)/wolfcrypt/src/wolfmath.o 

TLS_CLASS_OBJ = $(WOLFSSL_ROOT_DIR)/TLSSocketConnection.o

INCLUDE_PATHS += -I$(WOLFSSL_ROOT_DIR)/ -I$(WOLFSSL_LIB_DIR)/

#CC_SYMBOLS += -DWOLFSSL_USER_SETTINGS

C_OBJ += $(WOLFSSL_OBJ)
CXX_OBJ += $(TLS_CLASS_OBJ)

