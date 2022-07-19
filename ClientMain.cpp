//
// Created by jeuio on 14/05/2022.
//

#include "CONFIG.h"

#include <Networking.h>
#include "Networking/Handlers.h"

int main() {

    std::cout << "Connecting to server..." << std::endl;
    Client client;
    client.connectToIp(10, "127.0.0.1");

    client.addHandler(new Authentication);
    client.addHandler(new TextHandler);

    std::thread clientThread([&] { client.run(); });

    for(;;);
}