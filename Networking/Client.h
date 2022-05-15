//
// Created by jeuio on 06/02/2022.
//

#pragma once

#include <utility>
#include <fstream>

#include "../Networking/ClientHandler.h"
#include "Message.h"





class Client {

    asio::io_service service;
    ClientHandler handler = ClientHandler(service);

public:

    Client() = default;



    void run() {

        service.run();
    }



    //////////////////////////////////////////////////



    void connectToDomain(std::string port, std::string hostname) {

        asio::ip::tcp::resolver::query resolverQuery(hostname, port, asio::ip::tcp::resolver::query::numeric_service);

        asio::ip::tcp::resolver resolver(service);

        asio::error_code error;
        asio::ip::tcp::resolver::iterator it = resolver.resolve(resolverQuery, error);
        if (error) {
#if(DEBUG > 0)
            std::cout << "Error while resolving hostname\n";
#endif
            return;
        }

        handler.socket().connect(it->endpoint());

        handler.start();
    }



    void connectToIp(uint16_t port, std::string ip) {

        asio::error_code error;
        asio::ip::address ipAddress = asio::ip::address::from_string(ip, error);

        asio::ip::tcp::endpoint endpoint(ipAddress, port);

        handler.socket().connect(endpoint, error);

        if (error) {

#if(DEBUG > 0)
            std::cout << "An error occurred while trying to connect to the server\n";
#endif
            return;
        }

        handler.start();

#if(DEBUG > 0)
        std::cout << "Connected to server" << std::endl;
#endif
    }



    //////////////////////////////////////////////////



    void send(Message message) {

        handler.send(message);
    }
};