//
// Created by jeuio on 15/05/2022.
//

#include "DiscordoClient.h"

int main() {

   std::cout << "Waiting for connection..." << std::endl;
    Server server;

    server.addHandler(new Registration);
    server.addHandler(new TextHandler);
    //server.addHandler(new Authentication);


    server.start<ServerHandler>(38832);


    for(;;);

    return 0;
}