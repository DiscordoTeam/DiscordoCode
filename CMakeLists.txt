project(Discordo CXX)

cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)


#file(GLOB SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp" "Networking/*.h" "Networking/*.cpp" "Networking/Handlers/*.h" "Networking/Handlers/*.cpp")

set(SOURCES
        DiscordoClient.cpp
        TextChannel.cpp
        User.cpp
        DiscordoClient.cpp
        Networking/CustomMessages.cpp
        Networking/Handlers/Registration.cpp
        Networking/Handlers/TextHandler.cpp
)

if (WIN32)
    link_libraries(Networking Cryptography BigNum ws2_32 wsock32)
endif (WIN32)
if (UNIX)
    link_libraries(Networking Cryptography BigNum)
endif (UNIX)

add_library(DiscordoClient ${SOURCES})
set_target_properties(DiscordoClient PROPERTIES PUBLIC_HEADER DiscordoClient.h)

add_executable(Server ServerMain.cpp Networking/Handlers/Registration.cpp Networking/Handlers/TextHandler.cpp Networking/CustomMessages.cpp User.cpp)

list(GET CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES 0 LIB_DIR)
list(GET CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES 0 INC_DIR)

message(STATUS "Include directory: ${INC_DIR}")
message(STATUS "Library directory: ${LIB_DIR}")
install(TARGETS DiscordoClient DESTINATION ${LIB_DIR}
        PUBLIC_HEADER DESTINATION ${INC_DIR})