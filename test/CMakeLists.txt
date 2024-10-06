cmake_minimum_required(VERSION 3.16)
project(fxTap-Core-Test C)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME} main.c)

# You can see where the source code and the binary are in this repo,
# but when you do your own project, please set according to their actual locations.
set(FXTAP_CORE_SOURCE "../lib")
# However, in most cases, you clone this library in your project as a subfolder,
# in this case you don't need to specify where the binary is.
set(FXTAP_CORE_BINARY "../build-lib")

add_subdirectory(${FXTAP_CORE_SOURCE} ${FXTAP_CORE_BINARY})
target_include_directories(${PROJECT_NAME} PUBLIC ../lib/include)
target_compile_definitions(fxTap-Core PUBLIC FXTAP_CORE_ON_MODERN_OS)
target_link_libraries(${PROJECT_NAME} PUBLIC fxTap-Core)
