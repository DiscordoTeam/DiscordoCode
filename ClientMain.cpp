//
// Created by jeuio on 14/05/2022.
//

#include "CONFIG.h"

#include <Networking.h>
#include "Networking/Handlers.h"

int main() {

    std::cout << "Connecting to server..." << std::endl;
    Client client;

    //client.addHandler(new Authentication);
    client.addHandler(new TextHandler);

    client.connectToIp(10, "192.168.178.52");

    std::thread clientThread([&] { client.run(); });

    for(;;);
}