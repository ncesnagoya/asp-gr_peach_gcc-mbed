#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sil.h"
#include "scif.h"
#include "test_lib.h"

#include "mbed.h"
#include "EthernetInterface.h"
#include "syssvc/logtask.h"

#include "lwip/sockets.h"

#include  <wolfssl/ssl.h>          /* wolfSSL security library */
#include  <wolfssl/wolfcrypt/error-crypt.h>
#include  <wolfssl/wolfcrypt/logging.h>

#include <picoquic.h>
#include <picoquic_internal.h>
#include <picosocks.h>
#include <util.h>
#include <h3zero.h>
#include <democlient.h>
#include <picoquic_logger.h>

#include <userq_settings.h>

#include <quicClient.h>

q_stored_ticket_t SESSION_TICKET = {{0}, 0};

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/**** User Selection *********/
/** Network setting **/
#define USE_DHCP               (0)                 /* Select  0(static configuration) or 1(use DHCP) */
#if (USE_DHCP == 0)
  #define IP_ADDRESS           ("10.0.0.2")     /* IP address      */
  #define SUBNET_MASK          ("255.255.255.0")   /* Subnet mask     */
  #define DEFAULT_GATEWAY      ("10.0.0.3")     /* Default gateway */
#endif

#define SERVER "10.0.0.1"
#define HTTP_REQ "GET /iisstart.htm HTTP/1.0\r\nhost: 192.168.0.3\r\n\r\n"
#define DEFAULT_SNI "www.test.com"

#define HTTPS_PORT 8443

static const picoquic_demo_stream_desc_t test_scenario[] = 
    {{ 0, PICOQUIC_DEMO_STREAM_ID_INITIAL, "index.html", "index.html", 0 }};
static const size_t test_scenario_nb = sizeof(test_scenario) / sizeof(picoquic_demo_stream_desc_t);
static const char* ticket_store_filename = "demo_ticket_store.bin";
static const char* token_store_filename = "demo_token_store.bin";
#define PICOQUIC_DEMO_CLIENT_MAX_RECEIVE_BATCH 4

int sample_rand(uint8_t *buf, unsigned size){
    return RAND_bytes(buf, int (size));
}

/* Client client migration to a new port number: 
 *  - close the current socket.
 *  - open another socket at a randomly picked port number.
 *  - call the create probe API.
 * This is a bit tricky because the probe API requires passing the new address,
 * but in many cases the client will be behind a NAT, so it will not know its
 * actual IP address.
 */
int quic_client_migrate(picoquic_cnx_t * cnx, SOCKET_TYPE * fd, struct sockaddr * server_address, 
    int force_migration) 
{
    int ret = 0;

    if (force_migration != 2) {
        SOCKET_TYPE fd_m;

        fd_m = picoquic_open_client_socket(server_address->sa_family);
        if (fd_m == INVALID_SOCKET) {
            syslog(LOG_NOTICE, "Could not open new socket.\n");
            return EXIT_FAILURE;
        }
        else {
            SOCKET_CLOSE(*fd);
            *fd = fd_m;
        }
    }

    if (force_migration == 1) {
        syslog(LOG_NOTICE, "Switch to new port. Will test NAT rebinding support.\n");
    }
    else if (force_migration == 2) {
        ret = picoquic_renew_connection_id(cnx);
        if (ret != 0) {
            if (ret == PICOQUIC_ERROR_MIGRATION_DISABLED) {
                syslog(LOG_NOTICE, "Migration disabled, cannot test CNXID renewal.\n");
            }
            else {
                syslog(LOG_NOTICE, "Renew CNXID failed, error: %x.\n", ret);
            }
        }
        else {
            syslog(LOG_NOTICE, "Switching to new CNXID.\n");
        }
    }
    else {
        ret = picoquic_create_probe(cnx, server_address, NULL);
        if (ret != 0) {
            if (ret == PICOQUIC_ERROR_MIGRATION_DISABLED) {
                syslog(LOG_NOTICE, "Migration disabled, will test NAT rebinding support.\n");
                ret = 0;
            }
            else {
                syslog(LOG_NOTICE, "Create Probe failed, error: %x.\n", ret);
            }
        }
        else {
            syslog(LOG_NOTICE, "Switch to new port, sending probe.\n");
        }
    }

    return ret;
}

int q_client(const char* ip_address_text, int server_port, const char * sni, const char * root_crt,
    uint32_t proposed_version, int force_zero_share, int force_migration, int nb_packets_before_key_update, 
    int mtu_max, int client_cnx_id_length, char * client_scenario_text)
{
    /* Start: start the QUIC process with cert and key files */
    int ret = 0;
    picoquic_quic_t* qclient = NULL;
    picoquic_cnx_t* cnx_client = NULL;
    picoquic_demo_callback_ctx_t callback_ctx;
    SOCKET_TYPE fd = INVALID_SOCKET;
    struct sockaddr_storage server_address;
    struct sockaddr_storage client_address;
    struct sockaddr_storage packet_from;
    struct sockaddr_storage packet_to;
    unsigned long if_index_to;
    socklen_t from_length;
    socklen_t to_length;
    int server_addr_length = 0;
    uint8_t buffer[1536];
    uint8_t send_buffer[1536];
    size_t send_length = 0;
    uint64_t key_update_done = 0;
    int bytes_recv;
    int bytes_sent;
    uint64_t current_time = 0;
    int client_ready_loop = 0;
    int client_receive_loop = 0;
    int established = 0;
    int is_name = 0;
    int migration_started = 0;
    int address_updated = 0;
    int64_t delay_max = 10000000;
    int64_t delta_t = 0;
    int notified_ready = 0;
    int zero_rtt_available = 0;
    size_t client_sc_nb = 0;
    picoquic_demo_stream_desc_t * client_sc = NULL;
    const char * alpn = "hq-17";
    
    wolfSSL_RAND_seed(NULL, 0);
#ifndef default_RNG_defined
    uECC_set_rng(sample_rand);
#endif

    ret = picoquic_demo_client_initialize_context(&callback_ctx, test_scenario, test_scenario_nb, alpn);
    if (ret != 0){
        syslog(LOG_EMERG, "Client context initialize error[%d]", ret);
        return EXIT_FAILURE;
    }

    ret = picoquic_get_server_address(ip_address_text, server_port, &server_address, &server_addr_length, &is_name);
    if (sni == NULL && is_name != 0) {
        sni = ip_address_text;
    }

    if (ret != 0){
        syslog(LOG_EMERG, "Cannot get server address");
        return EXIT_FAILURE;
    }

    /* Open a UDP socket */
    fd = picoquic_open_client_socket(server_address.ss_family);
    if (fd == INVALID_SOCKET) {
        syslog(LOG_EMERG, "Cannot open client socket");
        return EXIT_FAILURE;
    }

    /* Create QUIC context */
    current_time = picoquic_current_time();
    callback_ctx.last_interaction_time = current_time;

    qclient = picoquic_create(8, NULL, NULL, root_crt, alpn, NULL, NULL, NULL, NULL, NULL, current_time, NULL, ticket_store_filename, NULL, 0);

    picoquic_set_default_congestion_algorithm(qclient, picoquic_cubic_algorithm);

    if (picoquic_load_tokens(&qclient->p_first_token, current_time, token_store_filename) != 0) {
        syslog(LOG_NOTICE, "Could not load tokens from <%s>.\n", token_store_filename);
    }

    if (qclient == NULL) {
        syslog(LOG_EMERG, "Cannot open client socket");
        return EXIT_FAILURE;
    } else {
        if (force_zero_share) {
            qclient->flags |= picoquic_context_client_zero_share;
        }
        qclient->mtu_max = mtu_max;

        (void)picoquic_set_default_connection_id_length(qclient, (uint8_t)client_cnx_id_length);

        if (sni == NULL) {
            /* Standard verifier would crash */
            syslog(LOG_NOTICE, "No server name specified, certificate will not be verified.\n");
            picoquic_set_null_verifier(qclient);
        }
#ifndef NO_CERTFILE
        else if (root_crt == NULL) {
            /* Standard verifier would crash */
            syslog(LOG_NOTICE, "No root crt list specified, but certificate will be verified.\n");
            picoquic_set_null_verifier(qclient);
        }
#endif
    }
    

    /* Create the client connection */
    cnx_client = picoquic_create_cnx(qclient, picoquic_null_connection_id, picoquic_null_connection_id,
        (struct sockaddr*)&server_address, current_time, proposed_version, sni, alpn, 1);

    if (cnx_client == NULL) {
        syslog(LOG_EMERG, "Cannot create client cnx");
        return EXIT_FAILURE;
    }
    else {
        picoquic_set_callback(cnx_client, picoquic_demo_client_callback, &callback_ctx);

        if (callback_ctx.tp != NULL) {
            picoquic_set_transport_parameters(cnx_client, callback_ctx.tp);
        }

        ret = picoquic_start_client_cnx(cnx_client);
        if (ret != 0){
            syslog(LOG_EMERG, "Cannot open client socket");
            return EXIT_FAILURE;
        }

        if (picoquic_is_0rtt_available(cnx_client) && (proposed_version & 0x0a0a0a0a) != 0x0a0a0a0a) {
            zero_rtt_available = 1;

            /* Queue a simple frame to perform 0-RTT test */
            /* Start the download scenario */
            ret = picoquic_demo_client_start_streams(cnx_client, &callback_ctx, PICOQUIC_DEMO_STREAM_ID_INITIAL);
            if (ret != 0){
                syslog(LOG_EMERG, "Cannot start client streams");
                return EXIT_FAILURE;
            }
        }

        /* TODO: once migration is supported, manage addresses */
        ret = picoquic_prepare_packet(cnx_client, current_time,
            send_buffer, sizeof(send_buffer), &send_length, NULL, NULL, NULL, NULL);
        if (ret != 0){
            syslog(LOG_EMERG, "Cannot prepare packet");
            return EXIT_FAILURE;
        }

        if (send_length > 0) {
            bytes_sent = lwip_sendto(fd, send_buffer, (int)send_length, 0,
                (struct sockaddr*)&server_address, server_addr_length);

            if (bytes_sent <= 0)
            {
                syslog(LOG_NOTICE, "Cannot send first packet to server, returns %d\n", bytes_sent);
                ret = -1;
            } else {
                //syslog(LOG_NOTICE, "Send %d bytes, T=%d\n", bytes_sent, (uint32_t)current_time);
            }
        }
    }

    /* Wait for packets */
    while (ret == 0 && picoquic_get_cnx_state(cnx_client) != picoquic_state_disconnected) {
        unsigned char received_ecn;
        from_length = to_length = sizeof(struct sockaddr_storage);

        bytes_recv = picoquic_select(&fd, 1, &packet_from, &from_length, &packet_to, &to_length,
            &if_index_to, &received_ecn, buffer, sizeof(buffer), delta_t, &current_time);

        if (bytes_recv != 0) {
            syslog(LOG_NOTICE, "\nSelect returns %d, T=%d\n", bytes_recv, (uint32_t)current_time);
        }

        if (bytes_recv != 0 && to_length != 0) {
            /* Keeping track of the addresses and ports, as we 
             * need them to verify the migration behavior */
            if (!address_updated) {
                struct sockaddr_storage local_address;
                if (picoquic_get_local_address(fd, &local_address) != 0) {
                    memset(&local_address, 0, sizeof(struct sockaddr_storage));
                }

                address_updated = 1;
                picoquic_store_addr(&client_address, (struct sockaddr *)&packet_to);
                if (client_address.ss_family == AF_INET) {
                    ((struct sockaddr_in *)&client_address)->sin_port =
                        ((struct sockaddr_in *)&local_address)->sin_port;
                }
                syslog(LOG_NOTICE, "Local address updated\n");
            }


            if (client_address.ss_family == AF_INET) {
                ((struct sockaddr_in *)&packet_to)->sin_port =
                    ((struct sockaddr_in *)&client_address)->sin_port;
            }
        }

        if (bytes_recv < 0) {
            ret = -1;
        } else {
            if (bytes_recv > 0) {
                /* Submit the packet to the client */
                ret = picoquic_incoming_packet(qclient, buffer, (size_t)bytes_recv, (struct sockaddr*)&packet_from,
                    (struct sockaddr*)&packet_to, if_index_to, received_ecn, current_time);
                client_receive_loop++;

                picoquic_log_processing(cnx_client, bytes_recv, ret);
                
                if (picoquic_get_cnx_state(cnx_client) == picoquic_state_client_almost_ready && notified_ready == 0) {
                    if (picoquic_tls_is_psk_handshake(cnx_client)) {
                        syslog(LOG_NOTICE, "The session was properly resumed!");
                    }

                    if (cnx_client->zero_rtt_data_accepted) {
                        syslog(LOG_NOTICE, "Zero RTT data is accepted!\n");
                    }
                    syslog(LOG_NOTICE, "Almost ready!\n\n");
                    notified_ready = 1;
                }

                if (ret != 0) {
                    picoquic_log_error_packet(buffer, (size_t)bytes_recv, ret);
                }

                delta_t = 0;
            }

            /* In normal circumstances, the code waits until all packets in the receive
             * queue have been processed before sending new packets. However, if the server
             * is sending lots and lots of data this can lead to the client not getting
             * the occasion to send acknowledgements. The server will start retransmissions,
             * and may eventually drop the connection for lack of acks. So we limit
             * the number of packets that can be received before sending responses. */

            if (bytes_recv == 0 || (ret == 0 && client_receive_loop > PICOQUIC_DEMO_CLIENT_MAX_RECEIVE_BATCH)) {
                client_receive_loop = 0;

                if (ret == 0 && (picoquic_get_cnx_state(cnx_client) == picoquic_state_ready || 
                    picoquic_get_cnx_state(cnx_client) == picoquic_state_client_ready_start)) {
                    if (established == 0) {
                        picoquic_log_transport_extension(cnx_client, 0);
                        syslog(LOG_NOTICE, "Connection established. Version = %x, I-CID: %lx%lx\n",
                            picoquic_supported_versions[cnx_client->version_index].version,
                            (uint32_t)(picoquic_val64_connection_id(picoquic_get_logging_cnxid(cnx_client)) >> 32),
                            (uint32_t)(picoquic_val64_connection_id(picoquic_get_logging_cnxid(cnx_client))));
                        established = 1;

                        if (zero_rtt_available == 0) {
                            /* Start the download scenario */

                            picoquic_demo_client_start_streams(cnx_client, &callback_ctx, PICOQUIC_DEMO_STREAM_ID_INITIAL);
                        }
                    }

                    client_ready_loop++;

                    if (force_migration && migration_started == 0 && 
                        (cnx_client->cnxid_stash_first != NULL || force_migration == 1)
                        && picoquic_get_cnx_state(cnx_client) == picoquic_state_ready) {
                        int mig_ret = quic_client_migrate(cnx_client, &fd,
                            (struct sockaddr *)&server_address, force_migration);

                        migration_started = 1;
                        address_updated = 0;

                        if (mig_ret != 0) {
                            syslog(LOG_NOTICE, "Will not test migration.\n");
                            migration_started = -1;
                        }
                    }

                    if (nb_packets_before_key_update > 0 && !key_update_done &&
                        cnx_client->pkt_ctx[picoquic_packet_context_application].first_sack_item.end_of_sack_range > (uint64_t)nb_packets_before_key_update) {
                        int key_rot_ret = picoquic_start_key_rotation(cnx_client);
                        if (key_rot_ret != 0) {
                            syslog(LOG_NOTICE, "Will not test key rotation.\n");
                            key_update_done = -1;
                        }
                        else {
                            syslog(LOG_NOTICE, "Key rotation started.\n");
                            key_update_done = 1;
                        }
                    }

                    if ((bytes_recv == 0 || client_ready_loop > 4) && picoquic_is_cnx_backlog_empty(cnx_client)) {
                        if (callback_ctx.nb_open_streams == 0) {
                            if (cnx_client->nb_zero_rtt_sent != 0) {
                                syslog(LOG_NOTICE, "Out of %d zero RTT packets, %d were acked by the server.\n",
                                    cnx_client->nb_zero_rtt_sent, cnx_client->nb_zero_rtt_acked);
                            }
                            syslog(LOG_NOTICE, "All done, Closing the connection.\n");
                            if (picoquic_get_data_received(cnx_client) > 0) {
                                uint64_t duration_usec = current_time - picoquic_get_cnx_start_time(cnx_client);

                                if (duration_usec > 0) {
                                    double receive_rate_mbps = 8.0*((double)picoquic_get_data_received(cnx_client)) / duration_usec;
                                    syslog(LOG_NOTICE, "Received %lu%lu bytes in %d micro seconds, %d Mbps.\n",
                                        (uint32_t)(picoquic_get_data_received(cnx_client) >> 32),
                                        (uint32_t)picoquic_get_data_received(cnx_client),
                                        (uint32_t)duration_usec, (uint32_t)receive_rate_mbps);
                                }
                            }

                            ret = picoquic_close(cnx_client, 0);
                        } else if (
                            current_time > callback_ctx.last_interaction_time && current_time - callback_ctx.last_interaction_time > 10000000ull) {
                            syslog(LOG_NOTICE, "No progress for 10 seconds. Closing. \n");
                            ret = picoquic_close(cnx_client, 0);
                        }
                    }
                }

                if (ret == 0) {
                    struct sockaddr_storage x_to;
                    int  x_to_length;
                    struct sockaddr_storage x_from;
                    int  x_from_length;

                    send_length = PICOQUIC_MAX_PACKET_SIZE;

                    ret = picoquic_prepare_packet(cnx_client, current_time,
                        send_buffer, sizeof(send_buffer), &send_length, &x_to, &x_to_length, &x_from, &x_from_length);

                    if (migration_started && force_migration == 3){
                        if (address_updated) {
                            if (picoquic_compare_addr((struct sockaddr *)&x_from, (struct sockaddr *)&client_address) != 0) {
                                syslog(LOG_NOTICE, "Dropping packet sent from wrong address\n");
                                send_length = 0;
                            }
                        }
                    }

                    if (ret == 0 && send_length > 0) {
                        bytes_sent = lwip_sendto(fd, send_buffer, (int)send_length, 0,
                            (struct sockaddr*)&x_to, x_to_length);

                        if (bytes_sent <= 0)
                        {
                            syslog(LOG_NOTICE, "Cannot send packet to server, returns %d\n", bytes_sent);
                        } else {
                            syslog(LOG_NOTICE, "Send %d bytes, T=%d\n", bytes_sent, (uint32_t)current_time);
                        }
                    }
                }

                delta_t = picoquic_get_next_wake_delay(qclient, current_time, delay_max);

                if (delta_t > 10000 && callback_ctx.nb_open_streams == 0 &&
                    picoquic_is_cnx_backlog_empty(cnx_client)) {
                    delta_t = 10000;
                }
            }
        }
    }

    /* Clean up */
    picoquic_demo_client_delete_context(&callback_ctx);

    if (qclient != NULL) {
        uint8_t* ticket;
        uint16_t ticket_length;

        if (sni != NULL && 0 == picoquic_get_ticket(qclient->p_first_ticket, current_time, sni, (uint16_t)strlen(sni), alpn, (uint16_t)strlen(alpn), &ticket, &ticket_length, 0)) {
            picoquic_log_picotls_ticket(picoquic_null_connection_id, ticket, ticket_length);
        }

        if (picoquic_save_tickets_buffer(qclient->p_first_ticket, current_time, &SESSION_TICKET) != 0) {
            syslog(LOG_NOTICE, "Could not store the saved session tickets.");
        }


        if (picoquic_save_tokens(qclient->p_first_token, current_time, token_store_filename) != 0) {
            syslog(LOG_NOTICE, "Could not save tokens to <%s>.", token_store_filename);
        }

        picoquic_free(qclient);
    }

    wolfSSL_RAND_Cleanup();
    if (fd != INVALID_SOCKET) {
        SOCKET_CLOSE(fd);
        syslog(LOG_NOTICE, "Socket Close");
    }


    if (client_scenario_text != NULL && client_sc != NULL) {
        demo_client_delete_scenario_desc(client_sc_nb, client_sc);
        client_sc = NULL;
        syslog(LOG_NOTICE, "Delete Client scenario");
    }
    return ret;
}

void
quicClient_main(intptr_t exinf) {
    EthernetInterface network;
    //UDPSocket socket;

    const char * sni = DEFAULT_SNI;
    const char* root_trust_file = NULL;
    uint32_t proposed_version = 0;
    int force_zero_share = 0;
    int force_migration = 0;
    int nb_packets_before_update = 0;
    int client_cnx_id_length = 8;
    int mtu_max = 0;
    char * client_scenario = NULL;
    int ret = 0;

	/* syslogの設定 */
    SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));

    syslog(LOG_NOTICE, "quicClient:");
    syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);
    syslog(LOG_NOTICE, "LOG_NOTICE: Network Setting up...\n");

#if (USE_DHCP == 1)
    if (network.init() != 0) {                             //for DHCP Server
        syslog(LOG_NOTICE, "Network Initialize Error\n");
        return;
    }
#else
    if (network.init(IP_ADDRESS, SUBNET_MASK, DEFAULT_GATEWAY) != 0) { //for Static IP Address (IPAddress, NetMasks, Gateway)
		syslog(LOG_NOTICE, "Network Initialize Error\n");
        return;
    }
#endif

    syslog(LOG_NOTICE, "Network was initialized successfully\n");
    while (network.connect(5000) != 0) {
        syslog(LOG_NOTICE, "LOG_NOTICE: Network Connect Error\n");
    }
    wait(2.6);

    syslog(LOG_NOTICE, "MAC Address is %s\n", network.getMACAddress());
    syslog(LOG_NOTICE, "IP Address is %s\n", network.getIPAddress());
    syslog(LOG_NOTICE, "NetMask is %s\n", network.getNetworkMask());
    syslog(LOG_NOTICE, "Gateway Address is %s\n", network.getGateway());
    syslog(LOG_NOTICE, "Network Setup OK...\n");
    syslog(LOG_NOTICE, "Starting PicoQUIC connection to (%s) on port (%d)\n\n", SERVER, HTTPS_PORT);
    syslog_flush();

    ret = q_client(SERVER, HTTPS_PORT, sni, root_trust_file, proposed_version, force_zero_share, 
            force_migration, nb_packets_before_update, mtu_max, client_cnx_id_length, client_scenario);

    syslog(LOG_NOTICE, "Client exit with code = %d\n", ret);
}

// set mac address
void mbed_mac_address(char *mac) {
	// PEACH1
    mac[0] = 0x00;
    mac[1] = 0x02;
    mac[2] = 0xF7;
    mac[3] = 0xF0;
    mac[4] = 0x00;
    mac[5] = 0x00;
}

 /*
 *  周期ハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
bool_t led_state = true;
void cyclic_handler(intptr_t exinf)
{
	if (led_state == true) {
		led_state = false;
	} else {
		led_state = true;
	}
	set_led(BLUE_LED, led_state);
}
