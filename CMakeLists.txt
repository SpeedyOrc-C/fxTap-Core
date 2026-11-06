cmake_minimum_required(VERSION 3.28)
project(fxTap-Core CXX)

set(CMAKE_CXX_STANDARD 23)

add_library(fxTap-Core)
target_sources(fxTap-Core PUBLIC FILE_SET CXX_MODULES FILES
        src/lib.ixx
        src/beatmap.ixx
        src/config.ixx
        src/grading.ixx
        src/keymap.ixx
        src/keymapper.ixx
)

if (DEFINED GINT_PATH)
    target_compile_definitions(fxTap-Core PUBLIC FXTAP_CORE_USE_GINT)
endif ()

add_executable(fxTap-Core-Test test/main.cpp)
target_link_libraries(fxTap-Core-Test PRIVATE fxTap-Core)
