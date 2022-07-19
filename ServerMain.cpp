//
// Created by jeuio on 15/05/2022.
//

#include "CONFIG.h"

#include "Networking.h"
#include "Networking/Handlers.h"
#include <fstream>

int main() {

    std::fstream fs("Fiole");


   std::cout << "Waiting for connection..." << std::endl;
    Server server;

    server.addHandler(new Registration);
    server.addHandler(new TextHandler);
    //server.addHandler(new Authentication);


    server.start<ServerHandler>(38832);


    for(;;);

    return 0;
}