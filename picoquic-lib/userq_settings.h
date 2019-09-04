 
 
//    #define PICOQUIC_TOPPERS    

#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

    #define USE_LWIP
    #define USE_WOLFSSL_KX
    #define USE_WOLFSSL_AEAD
	#define NO_FILESYSTEM
	#ifdef NO_FILESYSTEM
	#define SSL_CA_ECC_PEM                                                                                                   \
	"-----BEGIN CERTIFICATE-----\n"                                                                                      \
    "MIICizCCAjCgAwIBAgIJAP0OKSFmy0ijMAoGCCqGSM49BAMCMIGXMQswCQYDVQQG\n"                                                 \
    "EwJVUzETMBEGA1UECAwKV2FzaGluZ3RvbjEQMA4GA1UEBwwHU2VhdHRsZTEQMA4G\n"                                                 \
    "A1UECgwHd29sZlNTTDEUMBIGA1UECwwLRGV2ZWxvcG1lbnQxGDAWBgNVBAMMD3d3\n"                                                 \
    "dy53b2xmc3NsLmNvbTEfMB0GCSqGSIb3DQEJARYQaW5mb0B3b2xmc3NsLmNvbTAe\n"                                                 \
    "Fw0xODA0MTMxNTIzMTBaFw0yMTAxMDcxNTIzMTBaMIGXMQswCQYDVQQGEwJVUzET\n"                                                 \
    "MBEGA1UECAwKV2FzaGluZ3RvbjEQMA4GA1UEBwwHU2VhdHRsZTEQMA4GA1UECgwH\n"                                                 \
    "d29sZlNTTDEUMBIGA1UECwwLRGV2ZWxvcG1lbnQxGDAWBgNVBAMMD3d3dy53b2xm\n"                                                 \
    "c3NsLmNvbTEfMB0GCSqGSIb3DQEJARYQaW5mb0B3b2xmc3NsLmNvbTBZMBMGByqG\n"                                                 \
    "SM49AgEGCCqGSM49AwEHA0IABALT2W7WAY5FyLmQMeXATOOerSk4mLoQ1ukJKoCp\n"                                                 \
    "LhcquYq/M4NG45UL5HdAtTtDRTMPYVN8N0TBy/yAyuhD6qejYzBhMB0GA1UdDgQW\n"                                                 \
    "BBRWjprD8ELeGLlFVW75k8/qw/OlITAfBgNVHSMEGDAWgBRWjprD8ELeGLlFVW75\n"                                                 \
    "k8/qw/OlITAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAKBggqhkjO\n"                                                 \
    "PQQDAgNJADBGAiEA8HvMJHMZP2Fo7cgKVEq4rHnvEDKRUiw+v1CqXxjBl/UCIQDZ\n"                                                 \
    "S2Nnb5spqddrY5uYnzKCNtrwqfdRtJeq+vrd7+9Krg==\n"                                                                     \
    "-----END CERTIFICATE-----\n" /*ca-ecc-cert.pem*/
	#endif /*NO_FILESYSTEM*/
	//#define PTLS_DBG 1
	#define DISABLE_DEBUG_PRINTF
    // #define QUICIPV6
    // #define QUIC_SERVER
    // #define QUIC_CLIENT
    // #define ENABLE_FILESYSTEM

    typedef struct st_q_stored_ticket_t {
        unsigned char ticket[2048];
        unsigned int stored;
    } q_stored_ticket_t;
    
    extern q_stored_ticket_t SESSION_TICKET;

#endif /* USER_SETTINGS_H */