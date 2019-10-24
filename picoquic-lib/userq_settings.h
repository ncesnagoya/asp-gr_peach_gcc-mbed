 
 
//    #define PICOQUIC_TOPPERS    

#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

    #define USE_LWIP
	#define NO_FILESYSTEM
    //#define TEST_BAD_CERT
	#ifdef NO_FILESYSTEM
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