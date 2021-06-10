#ifndef SPO_LAB3_UI_H
#define SPO_LAB3_UI_H

#include <curses.h>
#include <stdio.h>
#include <form.h>

#include "../common.h"

#define CHECKBOX_FILTER_BY_TITLE 0
#define CHECKBOX_FILTER_BY_AUTHOR 1
#define CHECKBOX_FILTER_BY_ANNOTATION 2
#define CHECKBOX_FILTER_BY_TAG 3

#define EDIT_BOX_TITLE 0
#define EDIT_BOX_AUTHOR 1
#define EDIT_BOX_ANNOTATION 2
#define EDIT_BOX_TAG 3
#define EDIT_BOX_NONE -1

struct border {
    WINDOW *borderBookListW;
    WINDOW *borderBookInfoW;
};

struct window_edit {
    WINDOW *top_button_window;
    WINDOW *list_book_window;
    WINDOW *info_book_window;
    WINDOW *bottom_button_window;
};

struct window_main {
    WINDOW *search_window;
    WINDOW *top_button_window;
    WINDOW *list_book_window;
    WINDOW *info_book_window;
    WINDOW *bottom_button_window;
    int book_lines;
    int book_name_length;
};

struct text_area {
    struct window_main window_main;
    struct window_edit window_edit;
};

struct form {
    WINDOW *window;
    FORM *form;
    FIELD **fields;
};

struct forms {
    struct form search;
    struct form edit;
};

struct console {
    struct border border;
    struct text_area text_area;
    struct forms forms;
};

int init_ui(struct console *cons);
void close_ui(struct console *cons);
void update_ui(size_t *args);
void clear_all_window(size_t *args);

#endif




