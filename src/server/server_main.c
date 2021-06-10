#include "./server_main.h"

connection_thread connections[MAX_CLIENTS_AMOUNT];
int server_status = ACTIVE;

void *main_connect_thread(void *args) {
    connection_thread * attrs = (connection_thread *) args;
    size_t connection_socket = attrs->socket;
    size_t * client_number = attrs->client_number;
    struct sockaddr *address = (struct sockaddr *)attrs->address;
    command_frame command;

    while (server_status == ACTIVE) {
        connection_thread * current = &connections[*client_number];
        current->socket = accept(connection_socket, address, attrs->socket_len);
        if (current->socket < 0) {
            perror(ERROR_MESSAGE_SOCKET_ACCEPT);
            exit(ERROR_END);
        }
        if (*client_number > MAX_CLIENTS_AMOUNT) {
            printf(ERROR_MESSAGE_CONNECT_NEW_CLIENT);
            command.command = QUIT;
            command.parameter = ERROR_CLIENTS_OVERFLOW;
            send_command(current->socket, &command);
            close(current->socket);
            current->connection_status = INACTIVE;
        }

        printf(MESSAGE_FORMAT_CLIENT_CONNECTED, *client_number);
        PRINTLN;
        command.command = CONNECTED;
        current->connection_status = ACTIVE;
        send_command(current->socket, &command);
        (*client_number)++;
        if (*client_number == MAX_CLIENTS_AMOUNT) printf(MESSAGE_CLIENTS_FULL);
    }
    pthread_exit(NORMAL_END);
}

void send_command_all(const int client_number, command_frame *command) {
    for (int i = 0; i < client_number; i++) {
        if(connections[i].socket < 0 || connections[i].connection_status != ACTIVE) continue;
        send_command(connections[i].socket, command);
    }
}

void deactivate_client(size_t client_number) {
    close(connections[client_number].socket);
    connections[client_number].connection_status = INACTIVE;
}

size_t default_command(event_info event) {
    fprintf(stderr, MESSAGE_FORMAT_CLIENT_PACKET_UNKNOWN, event.client_number);
    fprintf(stderr, "\n");
    deactivate_client(event.client_number);
    return NORMAL_END;
}

size_t list_book_command(event_info event) {
    book **books = (book **) event.books;
    printf(MESSAGE_FORMAT_CLIENT_BOOKS_LIST, event.client_number);
    PRINTLN;
    book_command_frame command;
    for (int i = 0; i < event.books_length; i++) {
        if (books[i]) {
            command.book = *(books[i]);
            command.command = RETRIEVE_BOOK;
        }
        if (i == event.books_length -1 || !books[i]){
            command.book = (book) {};
            command.command = EOF_BOOK;
        }
        send_book_command(connections[event.client_number].socket, &command);
    }

    return NORMAL_END;
}

size_t quit_command(event_info event) {
    printf(MESSAGE_FORMAT_CLIENT_QUIT, event.client_number);
    PRINTLN;
    deactivate_client(event.client_number);
    return NORMAL_END;
}

size_t update_book_command(event_info event) {
    printf(MESSAGE_FORMAT_CLIENT_BOOKS_UPDATE, event.client_number);
    book_command_frame book_command;
    connection_thread connection = connections[event.client_number];
    unpack_book_command(connection.socket, &book_command);
    for (int i = 0; i < event.books_length; i++) {
        if (event.books[i]->bookID != book_command.book.bookID) continue;
        memcpy(event.books[i], &(book_command.book), sizeof(book));
        break;
    }
    command_frame command;
    command.command = UPDATE_BOOK_INFO;
    send_command_all(*event.client_number_src, &command);
    return NORMAL_END;
}

size_t parse_command(size_t command, function * parsed_function) {
    switch(command) {
        case QUIT:
            *parsed_function = (function) &quit_command;
            return NORMAL_END;
        case LIST_BOOK:
            *parsed_function = (function) &list_book_command;
            return NORMAL_END;
        case UPDATE_BOOK_MODIFY:
            *parsed_function = (function) &update_book_command;
            return  NORMAL_END;
        default:
            *parsed_function = (function) &default_command;
            return NORMAL_END;
    }
}

size_t serve_event(size_t command, event_info event) {
    function parsed_function;
    parse_command(command, &parsed_function);
    return parsed_function(event);
}

void serve(const size_t * client_number, book **books, const size_t books_length) {
    command_frame command;
    fd_set read_fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    event_info event;
    event.client_number = 0;
    event.client_number_src = (size_t *) client_number;
    event.books = books;
    event.books_length = (size_t) books_length;

    while (server_status == ACTIVE) {
        FD_ZERO(&read_fds);
        for (int i = 0; i < *client_number; i++) {
            connection_thread * current = &connections[i];
            if (current->socket <= 0 || current->connection_status != ACTIVE) continue;
            FD_SET(current->socket, &read_fds);
            int return_code = select((int) current->socket + 1, &read_fds, NULL, NULL, &tv);
            if (return_code < 0) perror(ERROR_MESSAGE_UNDEFINED);
            else if (return_code == 0) continue;
            else {
                unpack_command(current->socket, &command);
                event.client_number = i;
                serve_event(command.command, event);
            }
        }
    }
}

void quit_server() {
    command_frame command = {QUIT, NORMAL_END};
    for (size_t i = 0; i < MAX_CLIENTS_AMOUNT; i++) {
        connection_thread conn = connections[i];
        if (conn.socket < 0 || conn.connection_status == INACTIVE) continue;
        send_command(conn.socket, &command);
        close(conn.socket);
    }
    server_status = INACTIVE;
}

void signal_quit_server() {
    quit_server();
    exit(SIGINT);
}

int run_server(long port) {
    signal(SIGINT, signal_quit_server);

    struct sockaddr_in server_address;
    int server_socket = 0;
    int reuse = 1;
    size_t return_code = init_connection(&server_socket, &server_address, port, &reuse, MAX_CLIENTS_AMOUNT);
    if (return_code != NORMAL_END) return return_code;

    struct sockaddr_in address;
    printf("Before books read\n;");
    book ** books = read_books();
    printf(MESSAGE_BOOKS_READ);
    size_t client_number = 0;
    size_t books_length = get_books_amount(books);
    printf("\nGot books amount\n;");


    connection_thread server_connection;
    socklen_t socket_len = sizeof(address);
    server_connection.socket = server_socket;
    printf("Create Accept thread\n;");
    server_connection.address = &address;
    server_connection.socket_len = &socket_len;
    server_connection.connection_status = (size_t) INACTIVE;
    server_connection.client_number =(size_t *) &client_number;

    pthread_create(&(server_connection.thread_id), NULL, main_connect_thread, &server_connection);
    printf("Accept thread created\n;");


    serve(&client_number, books, books_length);
    quit_server();

    return NORMAL_END;
}
