# system
cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_STANDARD 11)

# project
project(server C)

# sources
set(source_files
        main.c
        )

# build
add_executable(server main.c)
target_link_libraries(server ws2_32)
