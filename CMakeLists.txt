cmake_minimum_required(VERSION 3.9)
project(2iChat C)

set(CMAKE_C_STANDARD 99)

add_executable(2iChatServeur serveur.c include.h)
add_executable(2iChatClient client.c include.h)