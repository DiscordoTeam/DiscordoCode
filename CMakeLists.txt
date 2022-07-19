project(Discordo)

cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

file(GLOB SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp" "Networking/*.h" "Networking/*.cpp" "Networking/Handlers/*.h" "Networking/Handlers/*.cpp")

if(WIN32)
    link_libraries(Networking Cryptography BigNum ws2_32 wsock32)
endif(WIN32)
if(UNIX)
    link_libraries(Networking Cryptography BigNum)
endif(UNIX)

add_executable(DiscordoServer ServerMain.cpp Networking/Handlers/TextHandler.cpp Networking/Handlers/Registration.cpp Networking/CustomMessages.cpp Networking/CustomMessages.h Networking/CustomMessages.cpp User.cpp Networking/Handlers/LogIn.cpp)
add_executable(DiscordoClient ClientMain.cpp Networking/Handlers/TextHandler.cpp Networking/Handlers/Registration.cpp Networking/CustomMessages.cpp Networking/CustomMessages.h Networking/CustomMessages.cpp User.cpp Networking/Handlers/LogIn.cpp)

#add_executable(Test ${SOURCES})