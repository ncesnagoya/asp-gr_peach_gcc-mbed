/*

TSL Connection with wolfSSL

*/

#include "TCPSocketConnection.h"

#ifdef WOLFSSL_LIB
#include "wolfssl/ssl.h"
#endif

class TLSSocketConnection : public TCPSocketConnection {
    friend class TCPSocketServer;
    
public:
    TLSSocketConnection() ;
    
    /** Initialize TLS socket
     *  sc  == true : TLS Server,  sc  == false: TSL Client
     *  verify == true : verify peer, verify == false : verify none
    */
    int TlsInit(bool sc, bool verify);
    
    /** Connects this TCP socket to the server
    \param host The host to connect to. It can either be an IP Address or a hostname that will be resolved with DNS.
    \param port The host's port to connect to.
    \return 0 on success, -1 on failure.
    */
    int TlsConnect(const char* host, const int port);

    /** Send data to the remote host.
    \param data The buffer to send to the host.
    \param length The length of the buffer to send.
    \return the number of written bytes on success (>=0) or -1 on failure
     */
    int TlsSend(char* data, int length);

    /** Receive data from the remote host.
    \param data The buffer in which to store the data received from the host.
    \param length The maximum length of the buffer.
    \return the number of received bytes on success (>=0) or -1 on failure
     */
    int TlsReceive(char* data, int length);
    
    void TlsDisconn();
    /* get wolfSSL context so the user can submit wolfSSL functions as needed */
    #ifdef WOLFSSL_LIB
    WOLFSSL_CTX *TlsGet_CTX() ;
    #endif
    
private:
    bool tls ;
    #ifdef WOLFSSL_LIB
    WOLFSSL_CTX *ctx ;
    WOLFSSL     *ssl ;
    #endif
};
