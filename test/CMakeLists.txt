cmake_minimum_required(VERSION 3.16)
project(fxTap-Core-Test C)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME} main.c)

# You can see where the source code and the binary are in this repo,
# but when you do your own project, please set according to their actual locations.
set(FXTAP_CORE_SOURCE "../lib")
set(FXTAP_CORE_BINARY "../build-lib")

# Tell where's library's source code and binary
add_subdirectory(${FXTAP_CORE_SOURCE} ${FXTAP_CORE_BINARY})
# Allows #include<fxTap/blah-blah> in your C/C++ files
target_include_directories(${PROJECT_NAME} PUBLIC ../lib/include)
# Link this fxTap's core
target_link_libraries(${PROJECT_NAME} PUBLIC fxTap-Core)
