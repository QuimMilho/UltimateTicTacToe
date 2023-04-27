cmake_minimum_required(VERSION 3.20)
project(trabalhoCamilo C)

set(CMAKE_C_STANDARD 99)

add_executable(trabalhoCamilo main.c libs/utils.c libs/utils.h componentes/structs.h jogo/jogo.c jogo/jogo.h componentes/menu.c componentes/menu.h componentes/consts.h ficheiros/files.c ficheiros/files.h)
