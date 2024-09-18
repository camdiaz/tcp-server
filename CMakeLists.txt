cmake_minimum_required(VERSION 3.10)

project(TcpServer)

# Establece el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agrega el ejecutable
add_executable(tcp_server src/main.cpp)
