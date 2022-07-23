//
// Created by jeuio on 21/07/2022.
//

#include "DiscordoClient.h"

int main() {

    std::cout << "Waiting for connections..." << std::endl;

    Server server;

    HandlerManager hm;
    hm.addHandler(new Registration);
    hm.addHandler(new TextHandler);

    server.setHandlerManager(&hm);

    server.start<ServerHandler>(38832);

    for(;;);
    return 0;
}