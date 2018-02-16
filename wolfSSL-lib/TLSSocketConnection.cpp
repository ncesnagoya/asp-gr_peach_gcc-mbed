
#include "TLSSocketConnection.h"
#include "stdio.h"

#define PRINT_ERR(x, ...) printf("[ERROR:TLSSocketConnection]"x"\n", ##__VA_ARGS__);

    /** TLS socket connection
     *  tls == true : TLS connect, tls == false: TCP connect
     *  sc  == true : TLS Server,  sc  == false: TSL Client
    */
TLSSocketConnection::TLSSocketConnection() {
        tls = false ;
}

int TLSSocketConnection::TlsInit(bool sc, bool verify = true)
{
    #ifdef WOLFSSL_LIB
    WOLFSSL_METHOD *method ;
    #endif
    
    tls = true ;
    
    #ifdef WOLFSSL_LIB
    wolfSSL_Init() ;
    wolfSSL_Debugging_ON() ;
    
    printf("TlsInit(sc=%d, verify=%d)\n", sc, verify) ;
    method = sc ? wolfTLSv1_2_server_method() : wolfTLSv1_2_client_method() ;
    ctx = wolfSSL_CTX_new(method) ;
    if(ctx == NULL){
       PRINT_ERR("wolfSSL_CTX_new\n") ;
    }
    if(!verify){ /* no peer verification */
        wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0) ;
    }
    ssl = wolfSSL_new(ctx) ;
    if(ssl == NULL){
        PRINT_ERR("wolfSSL_new\n") ;
        return -1 ;

    }
    return 0 ;
    #endif
}

    /** Connects this TCP socket to the server
    \param host The host to connect to. It can either be an IP Address or a hostname that will be resolved with DNS.
    \param port The host's port to connect to.
    \return 0 on success, -1 on failure.
    */

int TLSSocketConnection::TlsConnect(const char* host, const int port)
{
    int ret ;
    connect(host, port) ;

    #ifdef WOLFSSL_LIB
    if(tls){
        ret = wolfSSL_connect(ssl) ;
        if(ret == NULL){
            PRINT_ERR("wolfSSL_connect\n") ;
            return -1 ;
        }
    }
    #endif
    
    return 0 ;
}


/** Send data to the remote host.
    \param data The buffer to send to the host.
    \param length The length of the buffer to send.
    \return the number of written bytes on success (>=0) or -1 on failure
     */

int TLSSocketConnection::TlsSend(char* data, int length)
{
    #ifdef WOLFSSL_LIB
    if(tls){
        return(wolfSSL_send(ssl, data, length, 0)) ;
    }
    #endif
    
    return(send(data, length)) ;    

}

    /** Receive data from the remote host.
    \param data The buffer in which to store the data received from the host.
    \param length The maximum length of the buffer.
    \return the number of received bytes on success (>=0) or -1 on failure
     */
     
int TLSSocketConnection::TlsReceive(char* data, int length)
{
    #ifdef WOLFSSL_LIB    
    if(tls){
        return(wolfSSL_recv(ssl, data, length, 0)) ;
    }
    #endif
    return(receive(data, length)) ;
}

void TLSSocketConnection::TlsDisconn()
{
    #ifdef WOLFSSL_LIB
    if(tls){
        wolfSSL_shutdown(ssl) ;
        wolfSSL_free(ssl);
    }
    #endif
    
    close() ;
}

#ifdef WOLFSSL_LIB
    /* get wolfSSL context so the user can submit wolfSSL functions as needed */
WOLFSSL_CTX *TLSSocketConnection::TlsGet_CTX() 
{
    return ctx ;
}
#endif


