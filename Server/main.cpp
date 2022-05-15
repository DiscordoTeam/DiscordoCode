//
// Created by jeuio on 15/05/2022.
//

#include "Server.h"
#include "ServerHandler.h"

int main() {

    std::cout << "Waiting for connection..." << std::endl;
    Server<ServerHandler> server;
    server.start(50);

    for(;;);
}