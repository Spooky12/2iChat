cmake_minimum_required(VERSION 3.9)
project(2iChat C)

set(CMAKE_C_STANDARD 99)

add_executable(Serveur2iChat serveur/serveur.c libs/include.h)
add_executable(Client2iChat client/client.c libs/include.h)



#For the Curses library to load:
SET(CURSES_USE_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

#add_executable(testNcurses testNcurses.c)
#target_link_libraries(testNcurses ${CURSES_LIBRARIES})
