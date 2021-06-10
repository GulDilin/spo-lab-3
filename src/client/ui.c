#include <string.h>
#include "./ui.h"

int init_curses() {
    printf("INIT curses started\n");
    if (!initscr()) {
        fprintf(stderr, "Error initialising ncurses.\n");
        return 1;
    }
    printf("INIT  cbrake\n");
    noecho();
    cbreak();

    keypad(stdscr, TRUE);
    printf("INIT curses finished\n");

    return 0;
}

void border_book(struct console *cons) {
    wborder(cons->border.border_book_list_window, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(cons->border.border_book_info_window, '|', '|', '-', '-', '+', '+', '+', '+');
}

void init_form_search(struct console *cons, int colsBookList) {
    cons->forms.search.window = derwin(cons->text_area.window_main.search_window, 1, colsBookList, 0, 0);
    cons->forms.search.fields = malloc(3 * sizeof(FIELD *));
    cons->forms.search.fields[0] = new_field(1, colsBookList-2, 0, 2, 2, 0);
    cons->forms.search.fields[1] = new_field(1, 2, 0, 0, 0, 0);
    cons->forms.search.fields[2] = NULL;

    set_field_buffer(cons->forms.search.fields[0], 0, "");
    set_field_buffer(cons->forms.search.fields[1], 0, " >");
    set_field_opts(cons->forms.search.fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_opts(cons->forms.search.fields[1], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

    cons->forms.search.form = new_form(cons->forms.search.fields);
    set_form_win(cons->forms.search.form, cons->forms.search.window);
    post_form(cons->forms.search.form);
}

void init_form_edit(struct console *cons, int rowBookInfo, int colsBookInfo, int colsBookList) {
    cons->forms.edit.window = derwin(cons->text_area.window_edit.info_book_window, rowBookInfo - 2, colsBookInfo, 1, 1);
    cons->forms.edit.fields = malloc(2 * sizeof(FIELD *));
    cons->forms.edit.fields[0] = new_field(5, colsBookInfo-4, 2, colsBookList+2, 5, 0);
    cons->forms.edit.fields[1] = NULL;

    set_field_buffer(cons->forms.edit.fields[0], 0, "");
    set_field_opts(cons->forms.edit.fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_back(cons->forms.edit.fields[0], A_UNDERLINE);

    cons->forms.edit.form = new_form(cons->forms.edit.fields);
    set_form_win(cons->forms.edit.form, cons->forms.edit.window);
}

int init_ui(struct console *cons) {
    if (init_curses()) {
        printf("INIT CURSES ERROR");
        return 1;
    }
    printf("UI preparings start ");


    int colsBookList = COLS * 2 / 10;
    int colsBookInfo = COLS * 8 / 10;

    int linesInBook = LINES - 3;

    cons->border.border_book_list_window = newwin(linesInBook, colsBookList, 1, 0);
    cons->border.border_book_info_window = newwin(linesInBook, colsBookInfo, 1, colsBookList);

    cons->text_area.window_main.list_book_window = newwin(linesInBook - 2, colsBookList - 4, 2, 2);
    cons->text_area.window_main.info_book_window = newwin(linesInBook - 2, colsBookInfo - 4, 2, colsBookList + 2);
    cons->text_area.window_main.book_lines = linesInBook - 2;
    cons->text_area.window_main.book_name_length = colsBookList - 8;
    cons->text_area.window_main.search_window = newwin(1, colsBookList, 0, 0);
    cons->text_area.window_main.top_button_window = newwin(1, colsBookInfo - 2, 0, colsBookList + 2);
    cons->text_area.window_main.bottom_button_window = newwin(1, COLS, LINES - 2, 0);

    cons->text_area.window_edit.list_book_window = cons->text_area.window_main.list_book_window;
    cons->text_area.window_edit.info_book_window = cons->text_area.window_main.info_book_window;
    cons->text_area.window_edit.top_button_window = cons->text_area.window_main.top_button_window;
    cons->text_area.window_edit.bottom_button_window = cons->text_area.window_main.bottom_button_window;

    init_form_search(cons, colsBookList);
    init_form_edit(cons, linesInBook, colsBookInfo, colsBookList);

    return 0;
}

void refresh_border(struct console *cons) {
    border_book(cons);
    refresh();
    wrefresh(cons->border.border_book_list_window);
    wrefresh(cons->border.border_book_info_window);

}

void refresh_window_edit(struct console *cons) {
    wborder(cons->border.border_book_list_window, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(cons->forms.edit.window, '|', '|', '-', '-', '+', '+', '+', '+');
    refresh();
    wrefresh(cons->border.border_book_list_window);
    wrefresh(cons->forms.edit.window);
    wrefresh(cons->text_area.window_main.info_book_window);
    wrefresh(cons->text_area.window_main.list_book_window);
    wrefresh(cons->text_area.window_main.top_button_window);
    wrefresh(cons->text_area.window_main.bottom_button_window);
    refresh();
}

void refresh_window_main(struct console *cons){
    refresh_border(cons);
    wrefresh(cons->text_area.window_main.info_book_window);
    wrefresh(cons->text_area.window_main.list_book_window);
    wrefresh(cons->text_area.window_main.search_window);
    wrefresh(cons->text_area.window_main.top_button_window);
    wrefresh(cons->text_area.window_main.bottom_button_window);
    wrefresh(cons->forms.search.window);
    refresh();
}

void clear_window_text_area(struct text_area *textArea) {
    delwin(textArea->window_main.info_book_window);
    delwin(textArea->window_main.list_book_window);
    delwin(textArea->window_main.search_window);
    delwin(textArea->window_main.top_button_window);
    delwin(textArea->window_main.bottom_button_window);
}

void clear_window_box_area(struct border *box_area) {
    delwin(box_area->border_book_info_window);
    delwin(box_area->border_book_list_window);
}

void clear_form_edit(struct form *search) {
    delwin(search->window);
    free_form(search->form);
    free_field(search->fields[0]);
    free(search->fields);
}

void clear_form_search(struct form *search) {
    delwin(search->window);
    free_form(search->form);
    free_field(search->fields[0]);
    free_field(search->fields[1]);
    free(search->fields);
}

void clear_forms(struct forms *forms) {
    clear_form_search(&(forms->search));
    clear_form_edit(&(forms->edit));
}

void clear_window(struct console *cons) {
    clear_forms(&(cons->forms));
    clear_window_box_area(&(cons->border));
    clear_window_text_area(&(cons->text_area));
}

void close_ui(struct console *cons) {
    clear_window(cons);
    endwin();
}

void print_book_list(struct console *cons, book **books, int lenght, int selected_page, int selected_book) {
    wclear(cons->text_area.window_main.list_book_window);
    for (int i = 0; i < cons->text_area.window_main.book_lines; i++) {
        int index = cons->text_area.window_main.book_lines * selected_page + i;
        if (index >= lenght) { break; }
        book *bk = books[index];
        if (bk == NULL) continue;
        char *m = malloc((cons->text_area.window_main.book_name_length + 1) * sizeof(char));
        memcpy(m, bk->title, cons->text_area.window_main.book_name_length);
        if (i == selected_book) wprintw(cons->text_area.window_main.list_book_window, "> %s\n", m);
        else wprintw(cons->text_area.window_main.list_book_window, "%s\n", m);
        free(m);
    }
}

void print_book_info(struct console *cons, book *book) {
    wclear(cons->text_area.window_main.info_book_window);
    wprintw(cons->text_area.window_main.info_book_window, "Title: %s\n", book->title);
    wprintw(cons->text_area.window_main.info_book_window, "Authors: %s\n", book->authors);
    wprintw(cons->text_area.window_main.info_book_window, "Annotation: %s\n", book->annotation);
    wprintw(cons->text_area.window_main.info_book_window, "Tags: %s\n", book->tags);
    wprintw(cons->text_area.window_main.info_book_window, "Available: %d\n", book->available);
}

void print_top_menu(struct console *cons, const bool *open_edit_form) {
    wclear(cons->text_area.window_main.top_button_window);
    if(!(*open_edit_form)) {
        wprintw(cons->text_area.window_main.top_button_window, "[Get book ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F1");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");

        wprintw(cons->text_area.window_main.top_button_window, "[Return book ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F2");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");

        wprintw(cons->text_area.window_main.top_button_window, "[To Edit Book ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F5");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");
    } else {
        wprintw(cons->text_area.window_main.top_button_window, "[Title ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F1");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");

        wprintw(cons->text_area.window_main.top_button_window, "[Authors ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F2");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");

        wprintw(cons->text_area.window_main.top_button_window, "[Annotation ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F3");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");

        wprintw(cons->text_area.window_main.top_button_window, "[Tags ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F4");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");

        wprintw(cons->text_area.window_main.top_button_window, "[Return ");
        wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "F5");
        wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.top_button_window, "] ");
    }

    wprintw(cons->text_area.window_main.top_button_window, "[Exit ");
    wattron(cons->text_area.window_main.top_button_window, A_UNDERLINE);
    wprintw(cons->text_area.window_main.top_button_window, "F10");
    wattroff(cons->text_area.window_main.top_button_window, A_UNDERLINE);
    wprintw(cons->text_area.window_main.top_button_window, "]");
}

void print_botton_menu(struct console *cons, const bool *checkboxFilter, const bool *open_edit_form, const int *editField) {
    wclear(cons->text_area.window_main.bottom_button_window);
    if(!(*open_edit_form)) {
        wprintw(cons->text_area.window_main.bottom_button_window, "Filter by ");

        wprintw(cons->text_area.window_main.bottom_button_window, "%s Title (",
                checkboxFilter[CHECKBOX_FILTER_BY_TITLE] ? "[X]" : "[ ]");
        wattron(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, "F6");
        wattroff(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, "), ");

        wprintw(cons->text_area.window_main.bottom_button_window, "%s Author (",
                checkboxFilter[CHECKBOX_FILTER_BY_AUTHOR] ? "[X]" : "[ ]");
        wattron(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, "F7");
        wattroff(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, "), ");

        wprintw(cons->text_area.window_main.bottom_button_window, "%s Annotation (",
                checkboxFilter[CHECKBOX_FILTER_BY_ANNOTATION] ? "[X]" : "[ ]");
        wattron(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, "F8");
        wattroff(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, "), ");

        wprintw(cons->text_area.window_main.bottom_button_window, "%s Tag (",
                checkboxFilter[CHECKBOX_FILTER_BY_TAG] ? "[X]" : "[ ]");
        wattron(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, "F9");
        wattroff(cons->text_area.window_main.bottom_button_window, A_UNDERLINE);
        wprintw(cons->text_area.window_main.bottom_button_window, ") ");
    }else{
        wprintw(cons->text_area.window_main.bottom_button_window, "Edit Book ");
        switch (*editField) {
            case EDIT_BOX_TITLE:
                wprintw(cons->text_area.window_main.bottom_button_window, "Title");
                break;
            case EDIT_BOX_AUTHOR:
                wprintw(cons->text_area.window_main.bottom_button_window, "Author");
                break;
            case EDIT_BOX_ANNOTATION:
                wprintw(cons->text_area.window_main.bottom_button_window, "Annotation");
                break;
            case EDIT_BOX_TAG:
                wprintw(cons->text_area.window_main.bottom_button_window, "Tag");
                break;
            case EDIT_BOX_NONE:
            default:
                break;
        }
    }
}

void print_selected_book(struct console *cons, book **books, int selectBook, const int *editField) {
    book *bk = books[selectBook];
    if(*editField == EDIT_BOX_NONE){
        print_book_info(cons, bk);
    }
}

void update_ui(size_t *args) {
    struct console *cons = (struct console *) args[0];
    book **books= (book **) args[1];
    int selected_page = *((int *)args[2]);
    int selected_book = *((int *)args[3]);
    int count_book = *((int *)args[4]);
    bool *open_edit_field = (bool *) args[5];
    bool *checkboxFilter = (bool *) args[6];
    int *editField = (int *) args[8];

    int selectBook = selected_page * cons->text_area.window_main.book_lines + selected_book;

    print_top_menu(cons, open_edit_field);
    print_botton_menu(cons, checkboxFilter, open_edit_field, editField);
    print_book_list(cons, books, count_book, selected_page, selected_book);

    if(*open_edit_field) {
        print_selected_book(cons, books, selectBook, editField);
        refresh_window_edit(cons);
    }else {
        print_book_info(cons, books[selectBook]);
        refresh_window_main(cons);
    }
}

void clear_all_window(size_t *args) {
    struct console *cons = (struct console *) args[0];
    wclear(cons->text_area.window_main.info_book_window);
    wclear(cons->border.border_book_info_window);
    wclear(cons->border.border_book_list_window);
    wclear(cons->forms.edit.window);
}