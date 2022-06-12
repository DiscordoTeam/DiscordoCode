//
// Created by jeuio on 14/05/2022.
//

#include "CONFIG.h"

#include <Custom/Networking.h>
#include "TestHandler.h"

int main() {

    std::cout << "Connecting to server..." << std::endl;
    Client client;
    client.connectToIp(10, "127.0.0.1");

    client.addHandler(new TestHandler());

    std::thread clientThread([&] { client.run(); });

    Message message;
    message.header.id = 13;

    BigNum test("0xBEEF");
    message << test;

    client.handler.queueMessage(message);

    for(;;);
}