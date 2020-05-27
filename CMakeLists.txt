# system
cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_STANDARD 11)

# project
project(server C)

# sources
set(source_files
        source/Init.c
        )

# build
add_executable(server source/Init.c source/ServerService.c source/ServerService.h source/TimeService.c source/TimeService.h)
target_link_libraries(server ws2_32)
