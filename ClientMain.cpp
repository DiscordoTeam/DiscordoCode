//
// Created by jeuio on 14/05/2022.
//

#include "CONFIG.h"

#include <Networking.h>
#include "Networking/Handlers.h"

#define JEUIO_PORT "38832"
#define JEREMY_PORT "38833"

int main() {

    std::cout << "Connecting to server..." << std::endl;
    Client client;

    client.addHandler(new Registration);
    client.addHandler(new TextHandler);

    client.connectToIp(38832, "127.0.0.1");

    std::thread clientThread([&] { client.run(); });

    for(;;);

    return 0;
}