 
 
//    #define PICOQUIC_TOPPERS    

#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

    #define USE_LWIP
    #define USE_WOLFSSL_KX
    #define USE_WOLFSSL_AEAD
	#define NO_FILESYSTEM
    //#define TEST_BAD_CERT
	#ifdef NO_FILESYSTEM
	#define SSL_CA_ECC_PEM                                                                                               \
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

    #define TLS_ECDSA_CERT                                                                                          \
    "-----BEGIN CERTIFICATE-----\n"                                                                                 \
    "MIIDUDCCAvWgAwIBAgICEAAwCgYIKoZIzj0EAwIwgZcxCzAJBgNVBAYTAlVTMRMw\n"                                            \
    "EQYDVQQIDApXYXNoaW5ndG9uMRAwDgYDVQQHDAdTZWF0dGxlMRAwDgYDVQQKDAd3\n"                                            \
    "b2xmU1NMMRQwEgYDVQQLDAtEZXZlbG9wbWVudDEYMBYGA1UEAwwPd3d3LndvbGZz\n"                                            \
    "c2wuY29tMR8wHQYJKoZIhvcNAQkBFhBpbmZvQHdvbGZzc2wuY29tMB4XDTE3MTAy\n"                                            \
    "MDE4MTkwNloXDTI3MTAxODE4MTkwNlowgY8xCzAJBgNVBAYTAlVTMRMwEQYDVQQI\n"                                            \
    "DApXYXNoaW5ndG9uMRAwDgYDVQQHDAdTZWF0dGxlMRAwDgYDVQQKDAdFbGlwdGlj\n"                                            \
    "MQwwCgYDVQQLDANFQ0MxGDAWBgNVBAMMD3d3dy53b2xmc3NsLmNvbTEfMB0GCSqG\n"                                            \
    "SIb3DQEJARYQaW5mb0B3b2xmc3NsLmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEH\n"                                            \
    "A0IABLszrEwnUErGSqUEwzzenzbbci3OlOor+ssgCTksFuhhAumvTdMCk5oxW5eS\n"                                            \
    "IX/wzxjakRECNIboIFgzC4A0idijggE1MIIBMTAJBgNVHRMEAjAAMBEGCWCGSAGG\n"                                            \
    "+EIBAQQEAwIGQDAdBgNVHQ4EFgQUXV0m76x+NvmbdhUrSiUCI++yiTAwgcwGA1Ud\n"                                            \
    "IwSBxDCBwYAUVo6aw/BC3hi5RVVu+ZPP6sPzpSGhgZ2kgZowgZcxCzAJBgNVBAYT\n"                                            \
    "AlVTMRMwEQYDVQQIDApXYXNoaW5ndG9uMRAwDgYDVQQHDAdTZWF0dGxlMRAwDgYD\n"                                            \
    "VQQKDAd3b2xmU1NMMRQwEgYDVQQLDAtEZXZlbG9wbWVudDEYMBYGA1UEAwwPd3d3\n"                                            \
    "LndvbGZzc2wuY29tMR8wHQYJKoZIhvcNAQkBFhBpbmZvQHdvbGZzc2wuY29tggkA\n"                                            \
    "l7S9Fnj4R/IwDgYDVR0PAQH/BAQDAgOoMBMGA1UdJQQMMAoGCCsGAQUFBwMBMAoG\n"                                            \
    "CCqGSM49BAMCA0kAMEYCIQC+uFjw5BUBH99wVHNKbEAfd6i061Iev/UNsTPKasR2\n"                                            \
    "uQIhAJcI3iwowUVxtixUh5hjdqghNJCo954//AKw59MJMSfk\n"                                                            \
    "-----END CERTIFICATE-----\n"

    #define ECDSA_PRIVATE_KEY \
    "-----BEGIN EC PRIVATE KEY-----\n"                                                                              \
    "MHcCAQEEIEW2aQJznGyFoThbcujox6zEA41TNQT6bCjcNI3hqAmMoAoGCCqGSM49\n"                                            \
    "AwEHoUQDQgAEuzOsTCdQSsZKpQTDPN6fNttyLc6U6iv6yyAJOSwW6GEC6a9N0wKT\n"                                            \
    "mjFbl5Ihf/DPGNqREQI0huggWDMLgDSJ2A==\n"                                                                        \
    "-----END EC PRIVATE KEY-----\n"

    #ifdef TEST_BAD_CERT
    #define BAD_CA                                                                                                  \
    "-----BEGIN CERTIFICATE-----\n"                                                                                 \
    "MIIDMjCCAhqgAwIBAgIJAPh4W88oNy7tMA0GCSqGSIb3DQEBBQUAMBoxGDAWBgNV\n"                                            \
    "BAMTD3BpY290bHMgdGVzdCBjYTAeFw0xODAyMjMwMjI0NDJaFw0yODAyMjEwMjI0\n"                                            \
    "NDJaMBoxGDAWBgNVBAMTD3BpY290bHMgdGVzdCBjYTCCASIwDQYJKoZIhvcNAQEB\n"                                            \
    "BQADggEPADCCAQoCggEBAMewDZVDfz1PFT4TfGvG4uF27Tv7w64/bxFB0ZK4Wjpj\n"                                            \
    "eMxdiWBrw7dyZ9KAqxrcIw0KLBqeVRUvokTNSLGLM7j6CVMNFtH0dKqQ7hef9xB8\n"                                            \
    "NSPoNkTMs/Cf2te79ifOCd0+QHlIWi7Qzt2Ito+sKzuACFP+8zXIkksxHWGLLNSz\n"                                            \
    "Q0PfmDHNp+WnoTmTDIcwjhfhb3PUZVNZONFhVjXgrkCqgbutna96InsN/7TWGotT\n"                                            \
    "xSjb2xOuSSvoueCYGSFFb5a9UVMwWbAmquc8KnhTAvqwCa8QbaiOVujUWCL2k0H4\n"                                            \
    "EVlkzn+QfIiDNRk28SvwazcOtz7HPj795XwMYXPXiKcCAwEAAaN7MHkwHQYDVR0O\n"                                            \
    "BBYEFL95ypeyYHgglqpGV5zfp7Ij9SVjMEoGA1UdIwRDMEGAFL95ypeyYHgglqpG\n"                                            \
    "V5zfp7Ij9SVjoR6kHDAaMRgwFgYDVQQDEw9waWNvdGxzIHRlc3QgY2GCCQD4eFvP\n"                                            \
    "KDcu7TAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4IBAQBQ9EyGzIm8uX8U\n"                                            \
    "MIYkvGyQiSAl4v7Y9PZhtJIbuSn/hV8rutKs550AMFpPL5tijNpyUvZyR+Wpuvs9\n"                                            \
    "TGrOPIFhetcBF3tVUsg4lVvhMcxojUKysv0UwfEJQVbu1yoZmRdXOnKGiVnqvpI8\n"                                            \
    "ZjcgNtMacoBViQV44cR805bu6zBNWLaac3Q1wgT9NQSdBuQp0tAzVFQkE3ZRigfT\n"                                            \
    "LdQMb73jddaWZG8wnDfebK0klZo2oif2kGq53OOBooN/QUWKinMPPWdQVcY5Texa\n"                                            \
    "TmOVYk7HnWQEQ+Wr+9/o8EUs+3B/Af7lV7q9redWIiyYdyKPKmx090XHBy6HTPyO\n"                                            \
    "o9citOWg\n"                                                                                                    \
    "-----END CERTIFICATE-----\n"
    #endif /* TEST_BAD_CERT */
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