project(Discordo)

cmake_minimum_required(VERSION 3.21)

link_libraries(ws2_32 wsock32)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

file(GLOB GENERAL "General/*.cpp" "General/*.h" "General/BigNum/*.h" "General/Networking/*.h")
file(GLOB CLIENT "Client/*.cpp" "Client/*.h")
file(GLOB SERVER "Server/*.cpp" "Server/*.h")

add_executable(DiscordoClient ${GENERAL} ${CLIENT})
add_executable(DiscordoServer ${GENERAL} ${SERVER})