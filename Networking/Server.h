//
// Created by jeuio on 06/02/2022.
//

#pragma once

#include <iostream>
#include <asio.hpp>

template<typename ConnectionHandler>
class Server {

    RSA rsa;

    uint32_t threadCount;
    std::vector<std::thread> threadPool;
    asio::io_service service;
    asio::ip::tcp::acceptor acceptor;


    void handleNewConnection(std::shared_ptr<ConnectionHandler> handler, asio::error_code const &error) {

        if (error) return;

        std::cout << "Accepted Connection\n";

        handler->start();

        std::shared_ptr<ConnectionHandler> newHandler = std::make_shared<ConnectionHandler>(this->service, rsa);

        this->acceptor.async_accept(newHandler->socket(), [=](std::error_code ec) {

            handleNewConnection(newHandler, ec);
        });
    }


public:

    Server(uint32_t threadCount = 1)
            : threadCount(threadCount), acceptor(service) {

        rsa.generateKeys();
    }

    void start(uint16_t port) {

        std::shared_ptr<ConnectionHandler> handler = std::make_shared<ConnectionHandler>(this->service, rsa);

        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
        this->acceptor.open(endpoint.protocol());
        this->acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        this->acceptor.bind(endpoint);
        this->acceptor.listen();

        this->acceptor.async_accept(handler->socket(), [=](asio::error_code ec) {

            this->handleNewConnection(handler, ec);
        });

        for (uint32_t i = 0; i < this->threadCount; ++i) {

            this->threadPool.emplace_back([=] { service.run(); });
        }
    }
};