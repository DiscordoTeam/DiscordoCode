project(Discordo)

cmake_minimum_required(VERSION 3.21)

link_libraries(ws2_32 wsock32)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

add_executable(DiscordoClient main.cpp)