//
// Created by jeuio on 15/05/2022.
//

#include "CONFIG.h"

#include <Networking.h>
#include "Networking/Handlers.h"

int main() {

   std::cout << "Waiting for connection..." << std::endl;
    Server server;

    //server.addHandler(new Authentication);
    server.addHandler(new TextHandler);

    server.start<ServerHandler>(38832);

    for(;;);

    return 0;
}