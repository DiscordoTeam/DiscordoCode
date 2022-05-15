//
// Created by jeuio on 14/05/2022.
//

#include "Client.h"


int main() {

    std::cout << "Connecting to server..." << std::endl;
    Client client;
    client.connectToIp(50, "127.0.0.1");

    for(;;);
}