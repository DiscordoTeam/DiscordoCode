project(Discordo)

cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

file(GLOB SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp")

link_libraries(Networking Cryptography BigNum ws2_32 wsock32)
add_executable(DiscordoServer ServerMain.cpp CONFIG.h TestHandler.h)
add_executable(DiscordoClient ClientMain.cpp CONFIG.h TestHandler.h)