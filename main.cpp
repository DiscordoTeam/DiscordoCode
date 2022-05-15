//
// Created by jeuio on 14/05/2022.
//

#include <iostream>
#include "Networking/Client.h"
#include "Networking/ClientHandler.h"


int main() {

    Client client;
    client.connectToIp(42, "127.0.0.1");

    for(;;);
}