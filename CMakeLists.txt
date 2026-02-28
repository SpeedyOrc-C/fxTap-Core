cmake_minimum_required(VERSION 3.16)
project(fxTap-Core C)

set(CMAKE_C_STANDARD 23)

add_library(fxTap-Core
        src/config.c
        src/config-casiowin.c
        src/beatmap.c
        src/beatmap-casiowin.c
        src/game.c
        src/endian-utility.c
        src/render.c
        src/keymap.c
        src/keymap-djmax.c
        src/keymap-beatmania.c
)

target_include_directories(fxTap-Core PUBLIC include)

enable_testing()

add_executable(fxTap-Core-Test EXCLUDE_FROM_ALL test/main.c)
target_link_libraries(fxTap-Core-Test PRIVATE fxTap-Core)
target_include_directories(fxTap-Core-Test PRIVATE include)

add_test(NAME Test_All COMMAND $<TARGET_FILE:fxTap-Core-Test>)
