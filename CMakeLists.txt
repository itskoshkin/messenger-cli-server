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
SET(CMAKE_CXX_FLAGS -pthread)
add_executable(server source/Init.c source/Services/ServerService.c source/Services/ServerService.h source/Services/TimeService.c source/Services/TimeService.h source/Services/DatabaseService.c source/Services/DatabaseService.h source/Services/AuthService.c source/Services/AuthService.h source/Services/ClientStruct.h source/Services/ClientStruct.c)
target_link_libraries(${PROJECT_NAME} ws2_32)
target_link_libraries(${PROJECT_NAME} pthread)
