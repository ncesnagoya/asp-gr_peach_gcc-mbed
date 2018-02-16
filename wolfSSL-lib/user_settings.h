 
 
//    #define WOLFSSL_TOPPERS    
    //#define SINGLE_THREADED

    #define NO_WRITEV
    #define NO_DEV_RANDOM
    #define NO_DH
    #define HAVE_ECC
    #define NO_SESSION_CACHE // For Small RAM
    //#define IGNORE_KEY_EXTENSIONS
    #define NO_WOLFSSL_DIR  
    //#define DEBUG_WOLFSSL
    #define WOLFSSL_STATIC_MEMORY /* use wolf memory pool management */
    #define USE_FAST_MATH
    /* Options for Sample program */
    //#define WOLFSSL_NO_VERIFYSERVER
    //#define NO_FILESYSTEM
    #ifndef WOLFSSL_NO_VERIFYSERVER
        #define TIME_OVERRIDES
        #define XTIME time
        #define XGMTIME localtime
    #endif

    #define NO_FILESYSTEM
    #define USE_CERT_BUFFERS_1024
    #define WOLFSSL_DH_CONST
    #define WOLFSSL_LWIP

    #define WOLFSSL_GENSEED_FORTEST /* THIS IS STRICTLY FOR TEST USE ONLY */ 
    #define USE_WOLFSSL_MEMORY
    #define IGNORE_KEY_EXTENSIONS
    //#define WOLFSSL_MALLOC_CHECK
