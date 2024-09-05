cmake_minimum_required(VERSION 3.16)
project(fxTap-Core C)

set(CMAKE_C_STANDARD 11)

add_library(fxTap-Core
        src/config.c
        src/beatmap.c
        src/game.c
        src/endian-utility.c
        src/render.c
        src/beatmap-casiowin.c
        src/config-casiowin.c
        src/keymap-djmax.c
        src/keymap-beatmania.c
)

target_include_directories(fxTap-Core PUBLIC include)
