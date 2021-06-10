#ifndef SPO_LAB3_COMMON_H
#define SPO_LAB3_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_BOOKS_AMOUNT 500
#define MAX_BOOK_TITLE_LENGTH 256
#define MAX_BOOK_AUTHORS_AMOUNT 32
#define MAX_BOOK_ANNOTATION_LENGTH 512
#define MAX_BOOK_AUTHOR_NAME_LENGTH 32
#define MAX_BOOK_TAGS_AMOUNT 16
#define MAX_BOOK_TAG_LENGTH 32

typedef struct {
    size_t bookID;
    char title[MAX_BOOK_TITLE_LENGTH];
    char authors[MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH];
    char annotation[MAX_BOOK_ANNOTATION_LENGTH];
    char tags[MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH];
    char available;
} book;

book** generate_books(size_t length);
void free_books(book **books, const int books_amount);
book** read_books();
int get_books_amount(book **books);

#endif
