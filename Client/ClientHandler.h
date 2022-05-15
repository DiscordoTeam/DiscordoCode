//
// Created by jeuio on 06/02/2022.
//

#pragma once

#include <asio.hpp>
#include <deque>
#include <iostream>
#include <utility>

#include "../General/Networking/Message.h"
#include "chrono"





class ClientHandler {

    asio::io_service &service_;
    asio::ip::tcp::socket socket_;
    std::vector<uint8_t> headerBuffer, bodyBuffer;
    std::deque<Message> send_packet_queue;



    void queueMessage(Message message) {

        bool writing = !send_packet_queue.empty();
        send_packet_queue.push_back(std::move(message));

        if (!writing) {

            sendHeader();
        }
    }



    //////////////////////////////////////////////////



    void sendHeader() {

#if(DEBUG > 0)
        std::cout << "Sending header" << std::endl;
#endif
        asio::async_write(socket_, asio::buffer(send_packet_queue.front().headerData(), send_packet_queue.front().headerData().size()), [=](asio::error_code const &ec, std::size_t) {

            sendBody(ec);
        });
    }

    void sendBody(asio::error_code const &error) {

#if(DEBUG > 1)
        if (error) { std::cout << error.message() << std::endl; }
#endif
#if(DEBUG > 0)
        std::cout << "Sending body" << std::endl;
#endif
        asio::async_write(socket_, asio::buffer(send_packet_queue.front().body, send_packet_queue.front().body.size()), [=](asio::error_code const &ec, std::size_t) {

#if(DEBUG > 0)
            std::cout << "Done" << std::endl;
#endif
            packetSendDone(ec);
        });
    }

    void packetSendDone(asio::error_code const &error) {

#if(DEBUG > 0)
        if (error) {
            std::cout << error.message() << std::endl;
        }
#endif

        send_packet_queue.pop_front();
        if (!send_packet_queue.empty()) {

            sendHeader();
        }
    }



    //////////////////////////////////////////////////



    /**
     * @brief Reads the header of a message. The body of the message is read afterwards.
     */
    void readHeader() {

        headerBuffer.clear();
        headerBuffer.resize(9);
        asio::async_read(socket_, asio::buffer(headerBuffer, 9), [=](asio::error_code const &ec, std::size_t bytes) {

            readHeaderDone(ec, bytes);
        });
    }

    void readBody(asio::error_code const &error, std::size_t, Message message) {

        bodyBuffer.clear();
        bodyBuffer.resize(message.header.length);
        asio::async_read(socket_, asio::buffer(bodyBuffer, message.header.length), [=](asio::error_code const &ec, std::size_t bytes) {

            readBodyDone(ec, bytes, message);
        });
    }

    void readHeaderDone(asio::error_code error, std::size_t bytes) {

        if (error) {

#if(DEBUG > 1)
            std::cout << "Error while reading header: " << error.message() << std::endl;
#endif
            return;
        }

#if(DEBUG > 1)
        std::cout << "Bytes read:" << std::endl;
#endif

#if(DEBUG > 1)
        for (uint8_t i: headerBuffer) {

            std::cout << (int) i << std::endl;
        }
#endif

        Message message;
        message.header.id = headerBuffer[0];

        // Read the encryption blockSize of the messsage
        message.header.blocks |= headerBuffer[1];
        message.header.blocks = (message.header.blocks << 8) | headerBuffer[2];

        for (uint8_t i = 0; i < 8; ++i) {

            uint64_t n = headerBuffer[8 - i];
            message.header.length |= n << (8 * i);
        }


#if(DEBUG > 0)
        std::cout << "Id: " << (int) message.header.id << std::endl;
        std::cout << "Lenght: " << message.header.length << std::endl;
#endif

        if (message.header.length >= 0) {

            readBody(error, bytes, message);
        } else {

            service_.post([=] { readHeader(); });
        }
    }

    void readBodyDone(asio::error_code const &error, std::size_t bytes, Message message) {

        if (error) {

#if(DEBUG > 1)
            std::cout << "Error while reading body: " << error.message() << std::endl;
#endif
            return;
        }

#if(DEBUG > 1)
        std::cout << "Bytes read: " << bytes << std::endl;
        std::cout << "Read data: " << std::endl;
#endif

#if(DEBUG > 1)
        for (uint8_t i: bodyBuffer) {

            std::cout << (int) i << std::endl;
        }
#endif

        message.body.assign(bodyBuffer.begin(), bodyBuffer.end());

        process(message);

        service_.post([=] { readHeader(); });
    }



    //////////////////////////////////////////////////



    void process(Message message) {

        switch (message.header.id) {

        }
    }

public:

    ClientHandler(asio::io_service &service)
            : service_(service), socket_(service) {
    }



    asio::ip::tcp::socket &socket() {

        return this->socket_;
    }



    void start() {

        readHeader();
    }



    //////////////////////////////////////////////////



    void send(Message &message) {

        service_.post([=] {

            queueMessage(message);
        });
    }
};