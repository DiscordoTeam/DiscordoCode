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

    client.connectToDomain("38832", "big.jeuio.net");

    std::thread clientThread([&] { client.run(); });

    for(;;);

    return 0;
}