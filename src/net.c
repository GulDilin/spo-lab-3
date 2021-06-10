#include "./net.h"

int send_command(int socket, command_frame *frame){
    unsigned char frame_array[2];
    frame_array[0] = frame->command;
    frame_array[1] = frame->parameter;

    if ((write(socket, frame_array, 2)) == -1) {
        perror(ERROR_MESSAGE_SEND);
        exit(1);
    }
    return NORMAL_END;
}

int unpack_command(int socket, command_frame *frame) {
    unsigned char frame_array[2];

    long err = read(socket, frame_array, 2);
    if(err == -1) {
        fprintf(stderr, ERROR_MESSAGE_UNPACK);
        exit(1);
    }

    frame->command = frame_array[0];
    frame->parameter = frame_array[1];
    return NORMAL_END;
}

int send_book_command(int socket, book_command_frame *frame) {
    if(send(socket, frame, sizeof(book_command_frame), 0) == -1) {
        perror(ERROR_MESSAGE_SEND);
        return ERROR_RETRIEVE_BOOK;
    }
    return 0;
}

int unpack_book_command(int socket, book_command_frame *frame) {
    if(read(socket, frame, sizeof(book_command_frame)) < 1) {
        fprintf(stderr, ERROR_MESSAGE_UNPACK);
        exit(1);
    }
    return 0;
}

int socket_open(int *connect_socket) {
    printf(MESSAGE_SOCKET_OPEN);
    *connect_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect_socket < 0) {
        printf(ERROR_MESSAGE_SOCKET_OPEN);
        perror(ERROR_MESSAGE_SOCKET_OPEN);
        PRINTLN;
        return ERROR_SOCKET_OPEN;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return NORMAL_END;
}

int socket_connect (const int *client_socket, struct sockaddr_in *server_address) {
    printf(MESSAGE_SOCKET_CONNECT);
    if (connect(*client_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) == -1) {
        printf(ERROR_MESSAGE_SOCKET_CONNECT);
        perror(ERROR_MESSAGE_SOCKET_CONNECT);
        PRINTLN;
        return ERROR_CONNECT;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return NORMAL_END;
}

int socket_check_connect(const int *socket) {
    command_frame command;
    unpack_command(*socket, &command);
    if (!(command.command == CONNECTED && command.parameter == NORMAL_END)) {
        close(*socket);
        return ERROR_CONNECT;
    }
    return NORMAL_END;
}

void socket_close(const int *socket) {
    command_frame command = {QUIT, NORMAL_END};
    send_command(*socket, &command);
    printf(MESSAGE_SOCKET_CLOSE);
    shutdown(*socket, SHUT_RDWR);
    close(*socket);
    PRINTLN;
}

int socket_set_name(const int *connect_socket, int *reuse) {
    printf(MESSAGE_SOCKET_SET_NAME);
    if (setsockopt(*connect_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) reuse, sizeof(int)) == -1) {
        printf(ERROR_MESSAGE_SOCKET_SET_NAME);
        perror(ERROR_MESSAGE_SOCKET_SET_NAME);
        PRINTLN;
        return ERROR_SOCKET_SET_NAME;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return NORMAL_END;
}

int socket_bind(const int *connect_socket, struct sockaddr_in *server_address) {
    printf(MESSAGE_SOCKET_BIND);
    if (bind(*connect_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) == -1) {
        printf(ERROR_MESSAGE_SOCKET_BIND);
        perror(ERROR_MESSAGE_SOCKET_BIND);
        PRINTLN;
        return ERROR_SOCKET_BIND;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return NORMAL_END;
}

int socket_listen(const int *connect_socket, int max_clients_amount) {
    printf(MESSAGE_SOCKET_LISTEN);
    if (listen(*connect_socket, max_clients_amount) == -1) {
        printf(ERROR_MESSAGE_SOCKET_LISTEN);
        perror(ERROR_MESSAGE_SOCKET_LISTEN);
        PRINTLN;
        return ERROR_SOCKET_LISTEN;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return NORMAL_END;
}

size_t init_connection(int *connect_socket, struct sockaddr_in *server_address, long port, int *reuse, int max_clients_amount) {
    int return_code = NORMAL_END;

    return_code = socket_open(connect_socket);
    if (return_code != NORMAL_END) return return_code;

    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);
    server_address->sin_addr.s_addr = htonl(INADDR_ANY);

    return_code = socket_set_name(connect_socket, reuse);
    if (return_code != NORMAL_END) return return_code;
    return_code = socket_bind(connect_socket, server_address);
    if (return_code != NORMAL_END) return return_code;
    return_code = socket_listen(connect_socket, max_clients_amount);
    if (return_code != NORMAL_END) return return_code;
    printf(MESSAGE_INIT_FINISH);
    PRINTLN;

    return NORMAL_END;
}

bool validate_ip(char * address) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, address, &(sa.sin_addr)) != 0;
}

bool validate_port(int port) {
    return port < 65535 && port > 1;
}