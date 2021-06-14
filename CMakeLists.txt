cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(ProiectPCD)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

message("C compiler flags: ${CMAKE_C_FLAGS}")

add_executable(server server.c)
add_executable(client ezxml.c car.c client.c)
add_executable(admin admin.c)
