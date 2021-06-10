#include "./client_main.h"

int client_socket;

int connect_server(char *ip, long port, int *socket) {
    int return_code = NORMAL_END;
    return_code = socket_open(socket);
    if (return_code != NORMAL_END) return return_code;

    struct sockaddr_in server_address = {.sin_family = AF_INET, .sin_port = htons(port)};
    struct in_addr serv_address;
    if (inet_pton(AF_INET, ip, &serv_address) != 0) {
        server_address.sin_addr = serv_address;
    } else {
        server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }

    return_code = socket_connect(socket, &server_address);
    if (return_code != NORMAL_END) return return_code;

    return_code = socket_check_connect(socket);
    return return_code;
}

void list_books_command(const int *socket, book ***books, int *length) {
    command_frame command;
    command.command = LIST_BOOK;
    int old_length = *length;
    (*length) = 0;
    send_command(*socket, &command);

    book_command_frame book_command;
    while (1) {
        unpack_book_command(*socket, &book_command);
        if (book_command.command == EOF_BOOK) {
            (*books)[(*length)] = NULL;
            break;
        } else {
            if ((*books)[(*length)] == NULL) {
                (*books)[(*length)] = malloc(sizeof(book));
            }
            memcpy((*books)[(*length)], &(book_command.book), sizeof(book));
            (*length)++;
            if (old_length <= *length)
                (*books) = realloc((*books), ((*length) + 1) * sizeof(book *));
        }
    }
}

size_t parse_command_client(size_t command, function_client * parsed_function) {
    switch(command) {
        case KEY_UP_1:
            *parsed_function = (function_client) on_KEY_UP;
            return ACTIVE;
        case KEY_DOWN:
            *parsed_function = (function_client) on_KEY_DOWN;
            return ACTIVE;
        case KEY_F(1):
            *parsed_function = (function_client) on_KEY_F1;
            return ACTIVE;
        case KEY_F(2):
            *parsed_function = (function_client) on_KEY_F2;
            return ACTIVE;
        case KEY_F(3):
            *parsed_function = (function_client) on_KEY_F3;
            return ACTIVE;
        case KEY_F(4):
            *parsed_function = (function_client) on_KEY_F4;
            return ACTIVE;
        case KEY_F(5):
            *parsed_function = (function_client) on_KEY_F5;
            return ACTIVE;
        case KEY_F(6):
            *parsed_function = (function_client) on_KEY_F6;
            return ACTIVE;
        case KEY_F(7):
            *parsed_function = (function_client) on_KEY_F7;
            return ACTIVE;
        case KEY_F(8):
            *parsed_function = (function_client) on_KEY_F8;
            return ACTIVE;
        case KEY_F(9):
            *parsed_function = (function_client) on_KEY_F9;
            return ACTIVE;
        case KEY_F(10):
            *parsed_function = (function_client) on_KEY_F10;
            return ACTIVE;
        case KEY_MOUSE:
            *parsed_function = (function_client) on_KEY_MOUSE;
            return ACTIVE;
        default:
            *parsed_function = (function_client) on_symbol;
            return ACTIVE;
    }
}

size_t serve_event_client(size_t command, size_t * args) {
    function_client parsed_function;
    parse_command_client(command, &parsed_function);
    return parsed_function(args);
}

void *event_update(void *args) {
    event_info_client *arg = args;
    int *command = arg->command;
    size_t *active_status = arg->active_status;
    bool *need_update = arg->need_update;
    size_t *argsA = arg->args;
    while(*active_status == ACTIVE) {
        *active_status = serve_event_client(*command = getch(), argsA);
        *need_update = true;
    }
    return NULL;
}

int ui_work(book **books, const int *book_amount, bool *work, bool *need_update) {
    struct console console;
    printf("UI init");
    PRINTLN;
    if (init_ui(&console)) return ERROR_TERMINAL;
    printf("UI inited");
    int selectedBook = 0;
    int selectedPage = 0;
    int command = 0;
    bool open_edit_form = false;
    bool checkboxFilter[4] = {false};
    int editField = EDIT_BOX_NONE;

    size_t *args = malloc(11 * sizeof(size_t));
    args[0] = (size_t) &console;
    args[1] = (size_t) books;
    args[2] = (size_t) &selectedPage;
    args[3] = (size_t) &selectedBook;
    args[4] = (size_t) book_amount;
    args[5] = (size_t) &open_edit_form;
    args[6] = (size_t) checkboxFilter;
    args[7] = (size_t) &command;
    args[8] = (size_t) &editField;
    args[9] = (size_t) &client_socket;
    args[10] = (size_t) need_update;

    size_t active_status = ACTIVE;

    pthread_t threadUpdateEvent;
    event_info_client event;
    event.active_status = &active_status;
    event.command = &command;
    event.args = args;
    event.need_update = need_update;

    printf("Create thread to event updating");
    pthread_create(&threadUpdateEvent, NULL, event_update, &event);

    do {
        if (*need_update)  { update_ui(args); *need_update = false; }
        usleep(1);
    } while (active_status == ACTIVE && *work);
    free(args);
    close_ui(&console);
    return NORMAL_END;
}

void interrupt_close_client() {
    socket_close(&client_socket);
    exit(SIGINT);
}

void *update_book_thread(void *args) {
    client_thread *arg = args;
    command_frame command;
    while(*(arg->connect)) {
        unpack_command(arg->socket, &command);
        if(command.command == UPDATE_BOOK_INFO) {
            list_books_command(&(arg->socket), &(arg->books), arg->length);
            *(arg->need_update) = false;
        }else if(command.command == QUIT) {
            *(arg->connect) = false;
        }
    }
    pthread_exit(NORMAL_END);
}

int run_client(char *ip, long port) {
    signal(SIGINT, interrupt_close_client);

    int return_code = connect_server(ip, port, &client_socket);
    if (return_code > 0) return return_code;

    int amount = 0;
    book **books = malloc(sizeof(book *));
    list_books_command(&client_socket, &books, &amount);

    bool work = true;
    bool need_update = true;

    client_thread thread;
    thread.socket = client_socket;
    thread.books = books;
    thread.length = &amount;
    thread.connect = &work;
    thread.need_update = &need_update;

    printf("Create update thread \n");
    pthread_create(&thread.thread_id, NULL, update_book_thread, &thread);
    printf("Update thread created\n");
    return_code = ui_work(books, &amount, &work, &need_update);
    if(return_code != NORMAL_END) return return_code;
    socket_close(&client_socket);
    free_books(books, amount);
    return NORMAL_END;
}