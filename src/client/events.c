#include "./events.h"
#include "./ui.h"
#include "./client_main.h"

#include <malloc.h>

char *ltrim(char *str, const char *sep) {
    if (sep == NULL) {
        sep = "\t\n\v\f\r ";
    }
    size_t totrim = strspn(str, sep);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        } else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }
    return str;
}

char *rtrim(char *str, const char *sep) {
    if (sep == NULL) {
        sep = "\t\n\v\f\r ";
    }
    size_t i = strlen(str) - 1;
    while (i >= 0 && strchr(sep, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}

char *trim(char *str) {
    return ltrim(rtrim(str, NULL), NULL);
}

int seek_substring_KMP(char *source, char *find) {
    size_t N = strlen(source);
    size_t M = strlen(find);

    int succ = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (source[i] == find[j]) {
                succ = 1;
                i++;
            } else {
                succ = 0;
                i += j - 1;
                break;
            }
        }
        if(succ) return succ;
    }
    return -1;
}

void update_book(int socket, book * book) {
    command_frame command= {UPDATE_BOOK_MODIFY};
    book_command_frame book_command = {UPDATE_BOOK_MODIFY, *book};
    send_command(socket, &command);
    send_book_command(socket, &book_command);
}

void search_text(size_t *args) {
    struct console *cons = (struct console *) args[0];
    book **books = (book **) args[1];
    int *current_page = ((int *) args[2]);
    int *current_book = ((int *) args[3]);
    int *book_length = (int *) args[4];
    bool *checkbox = (bool *) args[6];

    form_driver(cons->forms.search.form, REQ_PREV_FIELD);
    form_driver(cons->forms.search.form, REQ_NEXT_FIELD);
    char *field_buffer_value = trim(field_buffer(cons->forms.search.fields[0], 0));

    for (int i = 0; i < *book_length; i++) {
        book *book = books[i];
        int t1 = checkbox[CHECKBOX_FILTER_BY_TITLE] ? seek_substring_KMP(book->title, field_buffer_value) : -1;
        int t2 = checkbox[CHECKBOX_FILTER_BY_TAG] ? seek_substring_KMP(book->tags, field_buffer_value) : -1;
        int a1 = checkbox[CHECKBOX_FILTER_BY_AUTHOR] ? seek_substring_KMP(book->authors, field_buffer_value) : -1;
        int a2 = checkbox[CHECKBOX_FILTER_BY_ANNOTATION] ? seek_substring_KMP(book->annotation, field_buffer_value)
                                                         : -1;

        if (t1 == -1 && t2 == -1 && a1 == -1 && a2 == -1) {
            continue;
        } else {
            if (i >= cons->text_area.window_main.book_lines) {
                *current_page = i / cons->text_area.window_main.book_lines;
            }
            *current_book = i % cons->text_area.window_main.book_lines;
            break;
        }

    }
}

bool on_symbol(size_t *args) {
    struct console *cons = (struct console *) args[0];
    bool *open_edit_from = (bool *) args[5];
    int *ch = (int *) args[7];
    int *editField = (int *) args[8];
    FORM *form = *open_edit_from ? cons->forms.edit.form : cons->forms.search.form;
    switch (*ch) {
        case KEY_BACKSPACE:
        case KEY_DEL:
            form_driver(form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(form, REQ_DEL_CHAR);
            break;
        case KEY_LEFT:
            form_driver(form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(form, REQ_NEXT_CHAR);
            break;
        case KEY_NL:
        case KEY_ENTER:
            if (!(*open_edit_from)) {
                search_text(args);
                *(bool *)(args[10]) = true;
            }
            break;
        default:
            if (*open_edit_from && *editField == EDIT_BOX_NONE) break;
            form_driver(form, *ch);
            break;
    }
    return true;
}

bool on_KEY_MOUSE(size_t *args) {
    MEVENT event;
    if (getmouse(&event) == OK) {}
    return true;
}

bool on_KEY_UP(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!(*open_edit_form)) {
        struct console *cons = (struct console *) args[0];
        int lengthArea = cons->text_area.window_main.book_lines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);

        (*selectedBook)--;

        if ((*selectedBook) < 0) {
            (*selectedPage)--;
            if ((*selectedPage) < 0) {
                (*selectedPage) = 0;
                (*selectedBook) = 0;
            } else {
                (*selectedBook) = lengthArea - 1;
            }
        }

        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        if (selectBook <= 0) {
            (*selectedBook) = 0;
        }
    }
    return true;
}

bool on_KEY_DOWN(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!(*open_edit_form)) {
        struct console *cons = (struct console *) args[0];
        int lengthArea = cons->text_area.window_main.book_lines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *length = (int *) args[4];
        int pages = (*length) / cons->text_area.window_main.book_lines + 1;

        (*selectedBook)++;

        if ((*selectedBook) >= lengthArea) {
            (*selectedPage)++;
            if ((*selectedPage) >= pages) {
                (*selectedPage) = pages - 1;
                (*selectedBook)--;
            } else {
                (*selectedBook) = 0;
            }
        }

        if ((*selectedBook) >= (*length)) {
            (*selectedBook) = (*length) - 1;
        }

        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        if (selectBook >= (*length)) {
            selectBook--;
            (*selectedBook)--;
        }
    }
    return true;
}

bool on_KEY_F1(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        book **books = (book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];

        int lengthArea = cons->text_area.window_main.book_lines;
        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }

        *editField = EDIT_BOX_TITLE;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->title);
    }else{
        struct console *cons = (struct console *) args[0];
        book **books = (book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int lengthArea = cons->text_area.window_main.book_lines;
        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        book *cur_book = books[selectBook];
        if(cur_book->available > 0) {
            cur_book->available--;
        }
        int *client_socket = (int *) args[9];
        update_book(*client_socket, cur_book);
    }

    return true;
}

bool on_KEY_F2(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        book **books = (book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];

        int lengthArea = cons->text_area.window_main.book_lines;
        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }
        *editField = EDIT_BOX_AUTHOR;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->authors);
    }else{
        struct console *cons = (struct console *) args[0];
        book **books = (book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int lengthArea = cons->text_area.window_main.book_lines;
        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        book *cur_book = books[selectBook];
        if(cur_book->available < 10) {
            cur_book->available++;
        }
        int *client_socket = (int *) args[9];
        update_book(*client_socket, cur_book);
    }
    return true;
}

bool on_KEY_F3(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        book **books = (book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];

        int lengthArea = cons->text_area.window_main.book_lines;
        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }

        *editField = EDIT_BOX_ANNOTATION;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->annotation);
    }

    return true;
}

bool on_KEY_F4(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        book **books = (book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];

        int lengthArea = cons->text_area.window_main.book_lines;
        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            default:
                break;
        }
        *editField = EDIT_BOX_TAG;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->tags);
    }
    return true;
}

bool on_KEY_F5(size_t *args) {
    struct console *cons = (struct console *) args[0];
    bool *open_edit_form = (bool *) args[5];
    *open_edit_form = !(*open_edit_form);
    if (*open_edit_form) {
        unpost_form(cons->forms.search.form);
        post_form(cons->forms.edit.form);
        set_current_field(cons->forms.edit.form, cons->forms.edit.fields[0]);
    } else {
        book **books = (book **) args[1];
        int lengthArea = cons->text_area.window_main.book_lines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int selectBook = (*selectedPage) * lengthArea + (*selectedBook);
        book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_VALIDATION);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        int *editField = (int *) args[8];
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }

        *editField = EDIT_BOX_NONE;

        unpost_form(cons->forms.edit.form);
        post_form(cons->forms.search.form);
        set_current_field(cons->forms.search.form, cons->forms.search.fields[0]);

        int *client_socket = (int *) args[9];
        update_book(*client_socket, cur_book);
    }
    clear_all_window(args);
    return true;
}

bool on_KEY_F6(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_TITLE] = !checkbox[CHECKBOX_FILTER_BY_TITLE];
    }
    return true;
}

bool on_KEY_F7(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_AUTHOR] = !checkbox[CHECKBOX_FILTER_BY_AUTHOR];
    }
    return true;
}

bool on_KEY_F8(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_ANNOTATION] = !checkbox[CHECKBOX_FILTER_BY_ANNOTATION];
    }
    return true;
}

bool on_KEY_F9(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_TAG] = !checkbox[CHECKBOX_FILTER_BY_TAG];
    }
    return true;
}

bool on_KEY_F10(size_t *args) {
    return false;
}