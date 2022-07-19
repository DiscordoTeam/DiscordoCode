project(Discordo)

cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

file(GLOB SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp" "Networking/*.h" "Networking/*.cpp" "Networking/Handlers/*.h" "Networking/Handlers/*.cpp")

link_libraries(Networking Cryptography BigNum ws2_32 wsock32)
add_executable(DiscordoServer ServerMain.cpp Networking/Handlers/Authentication.cpp Networking/Handlers/TextHandler.cpp CONFIG.h Networking/Handlers/Registration.cpp)
add_executable(DiscordoClient ClientMain.cpp Networking/Handlers/Authentication.cpp Networking/Handlers/TextHandler.cpp User.cpp User.h CONFIG.h TextChannel.cpp TextChannel.h Networking/Handlers/Registration.cpp)

add_executable(Test ${SOURCES})