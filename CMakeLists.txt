cmake_minimum_required(VERSION 3.10)
project(spo_lab3 C)

set(CMAKE_C_STANDARD 99)

add_executable(spo_lab3 src/main.c src/server/server_main.c src/net.c src/client/client_main.c src/client/ui.c src/client/ui.h src/common.c src/client/events.c src/client/events.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
if (CURSES_FOUND AND CURSES_HAVE_NCURSES_H)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(spo_lab3 PRIVATE Threads::Threads ${CURSES_LIBRARIES} form)
endif ()