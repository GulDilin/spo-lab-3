#ifndef SPO_LAB3_EVENTS_H
#define SPO_LAB3_EVENTS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../net.h"

#define KEY_UP_1 259
#define KEY_DEL 127
#define KEY_NL 10

bool on_symbol(size_t *args);
bool on_KEY_UP(size_t *args);
bool on_KEY_DOWN(size_t *args);
bool on_KEY_F1(size_t *args);
bool on_KEY_F2(size_t *args);
bool on_KEY_F3(size_t *args);
bool on_KEY_F4(size_t *args);
bool on_KEY_F5(size_t *args);
bool on_KEY_F6(size_t *args);
bool on_KEY_F7(size_t *args);
bool on_KEY_F8(size_t *args);
bool on_KEY_F9(size_t *args);
bool on_KEY_F10(size_t *args);
bool on_KEY_MOUSE(size_t *args);

#endif
