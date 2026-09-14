cmake_minimum_required(VERSION 4.3)
project(TCPChatServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(server main.cpp)
add_executable(client client.cpp)