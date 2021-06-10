#include "./common.h"

book** generate_books(size_t length) {
    printf("Start generating books\n");
    book **books = malloc((length+1) * sizeof(book *));
    printf("Memory allocated for all\n");

    for (size_t i = 0; i < length; i++) {
        books[i] = malloc(sizeof(book));
        books[i]->bookID = i;
        books[i]->available = 10;
        snprintf(books[i]->title, MAX_BOOK_TITLE_LENGTH, "Example %zu", i);
        snprintf(books[i]->authors, MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH, "Ivan, Igor");
        snprintf(books[i]->tags, MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH, "Bestseller");
        snprintf(books[i]->annotation, MAX_BOOK_ANNOTATION_LENGTH, "Is is absolutely adorable book");
    }
    books[length] = NULL;
    return books;
}

int get_books_amount(book **books) {
    for(int i = 1; ; i++) if (books[i] == NULL) return i;
}

void free_books(book **books, const int books_amount) {
    for (int i = 0; i < books_amount; i++) if(books[i]) free(books[i]);
    free(books);
}

book ** read_books() {
    return generate_books(50);
}