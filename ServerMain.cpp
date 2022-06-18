//
// Created by jeuio on 15/05/2022.
//

#include "CONFIG.h"

#include <Custom/Networking.h>
#include "TestHandler.h"

int main() {

    std::cout << "Waiting for connection..." << std::endl;
    Server server;

    server.addHandler(new TestHandler);

    server.start<ServerHandler>(10);

    for(;;);
}