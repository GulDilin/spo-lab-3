#ifndef SPO_LAB3_CLIENT_H
#define SPO_LAB3_CLIENT_H

#include <signal.h>
#include <pthread.h>

#include "../net.h"
#include "../common.h"
#include "./ui.h"
#include "./events.h"

typedef struct {
    pthread_t thread_id;
    int socket;
    book **books;
    int *length;
    bool *connect;
    bool *need_update;
} client_thread;

typedef struct {
    size_t *active_status;
    int *command;
    bool *need_update;
    size_t *args;
} event_info_client;

typedef size_t (*function_client)(size_t *);

int run_client(char * address,  long port);

#endif
