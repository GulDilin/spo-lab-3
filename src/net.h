#ifndef SPO_LAB3_NET_H
#define SPO_LAB3_NET_H

#include "stdio.h"
#include "stdbool.h"

#ifdef __WIN32__
#include "windows.h"
#include "ws2tcpip.h"
#define SHUT_RDWR SD_BOTH
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include "./common.h"

typedef struct {
    unsigned char command;
    unsigned char parameter;
} command_frame;

typedef struct {
    unsigned char command;
    book book;
} book_command_frame;

int socket_open(int *connect_socket);
int socket_connect (const int *client_socket, struct sockaddr_in *server_address);
int socket_check_connect(const int *socket);
void socket_close(const int *socket);
int socket_set_name(const int *connect_socket, int *reuse);
int socket_bind(const int *connect_socket, struct sockaddr_in *server_address);
int socket_listen(const int *connect_socket, int max_clients_amount);

size_t init_connection(int *connect_socket, struct sockaddr_in *server_address, long port, int *reuse, int max_clients_amount);
bool validate_ip(char * address);
bool validate_port(int port);

int send_command(int socket, command_frame * frame);
int unpack_command(int socket, command_frame * frame);

int send_book_command(int socket, book_command_frame * frame);
int unpack_book_command(int socket, book_command_frame * frame);

// commands
#define DEFAULT_COMMAND  1
#define CONNECTED  2
#define QUIT 3
#define LIST_BOOK 4
#define EOF_BOOK 5
#define RETRIEVE_BOOK 6
#define UPDATE_BOOK_MODIFY 7
#define UPDATE_BOOK_INFO 8

//status
#define ACTIVE 1
#define INACTIVE 0

//validation
#define VALID 1
#define INVALID 0

#define CI_SIZE(s) (sizeof(s)/sizeof(command))
#define PRINTLN printf("\n")

// return codes
#define ERROR_NO_SUCH_COMMAND -1
#define NORMAL_END 0
#define ERROR_END 1
#define ERROR_CLIENTS_OVERFLOW 2
#define ERROR_SOCKET_ACCEPT 3
#define ERROR_SOCKET_OPEN 4
#define ERROR_SOCKET_SET_NAME 5
#define ERROR_SOCKET_BIND  6
#define ERROR_SOCKET_LISTEN 7
#define ERROR_RETRIEVE_BOOK 8
#define ERROR_CONNECT 9
#define ERROR_TERMINAL 10


#define ERROR_MESSAGE_CLIENTS_OVERFLOW "Amount of clients is overflowed"
#define ERROR_MESSAGE_SOCKET_ACCEPT "Socket accept() failed"
#define ERROR_MESSAGE_CONNECT_NEW_CLIENT "New client cant be connected"
#define ERROR_MESSAGE_UNDEFINED "Undefined error"
#define ERROR_MESSAGE_SOCKET_OPEN "Open socket failed"
#define ERROR_MESSAGE_SOCKET_CONNECT "Connect socket failed"
#define ERROR_MESSAGE_SOCKET_LISTEN "Listen socket failed"
#define ERROR_MESSAGE_SOCKET_SET_NAME "Set socket name failed"
#define ERROR_MESSAGE_SOCKET_BIND "Bind socket failed"
#define ERROR_MESSAGE_UNPACK "Message unpack failed"
#define ERROR_MESSAGE_SEND "Message send failed"

#define MESSAGE_CLIENTS_FULL "Amount of client is full"
#define MESSAGE_SOCKET_OPEN "Open socket..."
#define MESSAGE_SOCKET_CONNECT "Connect socket..."
#define MESSAGE_SOCKET_LISTEN "Listen socket..."
#define MESSAGE_SOCKET_SET_NAME "Set socket name..."
#define MESSAGE_SOCKET_BIND "Bind socket..."
#define MESSAGE_SOCKET_CLOSE "Close socket"
#define MESSAGE_INIT_FINISH "Init finished"
#define MESSAGE_OK "OK"

#endif
