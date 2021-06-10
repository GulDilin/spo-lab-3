#ifndef SPO_LAB3_SERVER_MAIN_H
#define SPO_LAB3_SERVER_MAIN_H

#include <signal.h>
#include <pthread.h>
#include <string.h>

#include "../net.h"
#include "../common.h"

#define MAX_CLIENTS_AMOUNT 30

typedef struct {
    pthread_t thread_id;
    size_t * client_number;
    int socket;
    size_t connection_status;
    struct sockaddr_in * address;
    socklen_t *socket_len;
} connection_thread;

typedef struct {
    size_t client_number;
    size_t * client_number_src;
    size_t books_length;
    book ** books;
} event_info;

typedef size_t (*function)(event_info);

int run_server(long port);

#define MESSAGE_FORMAT_CLIENT_PACKET_UNKNOWN "Client #%zu <- Unknown packet"
#define MESSAGE_FORMAT_CLIENT_CONNECTED "Client #%zu <- Connected"
#define MESSAGE_FORMAT_CLIENT_BOOKS_LIST "Client #%zu <- List Books"
#define MESSAGE_FORMAT_CLIENT_QUIT "Client #%zu <- Quit"
#define MESSAGE_FORMAT_CLIENT_BOOKS_UPDATE "Client #%zu <- Quit"
#define MESSAGE_FORMAT_CLIENT_SEND_COMMAND "Client #%d -> command %d"
#define MESSAGE_BOOKS_READ "Books read finished"

#endif
