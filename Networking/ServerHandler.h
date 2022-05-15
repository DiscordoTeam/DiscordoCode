//
// Created by jeuio on 06/02/2022.
//

#pragma once

#include <memory>
#include <asio.hpp>
#include <deque>
#include <fstream>
#include <thread>

#include "Message.h"

class ServerHandler
        : public std::enable_shared_from_this<ServerHandler> {

    std::ofstream entryStream;

    asio::io_service &service_;
    asio::ip::tcp::socket socket_;
    asio::io_service::strand write_strand_;
    std::vector<uint8_t> headerBuffer, bodyBuffer;
    std::deque<Message> send_packet_queue;

    std::string username;

    void queueMessage(Message message) {

        bool writing = !send_packet_queue.empty();
        send_packet_queue.push_back(std::move(message));

        if (!writing) {

            sendHeader();
        }
    }

    void sendHeader() {

        asio::async_write(socket_, asio::buffer(send_packet_queue.front().headerData()), write_strand_.wrap([me = shared_from_this()](asio::error_code const &ec, std::size_t) {

            me->sendHeaderDone(ec);
        }));
    }

    void sendHeaderDone(asio::error_code const &error) {

        if (error) {
            std::cout << "Error while sending header: " << error.message() << std::endl;
            return;
        }

        if (send_packet_queue.front().header.length > 0) {

            sendBody();
        } else {

            sendMessageDone(error);
        }
    }

    void sendBody() {

        asio::async_write(socket_, asio::buffer(send_packet_queue.front().body), write_strand_.wrap([me = shared_from_this()](asio::error_code const &ec, std::size_t) {

            me->sendMessageDone(ec);
        }));
    }

    void sendMessageDone(asio::error_code const &error) {

        if (error) {
#if(DEBUG > 0)
            std::cout << "Error while sending message: " << error.message() << std::endl;
#endif
            return;
        }

        send_packet_queue.pop_front();
        if (!send_packet_queue.empty()) {

            sendHeader();
        }
    }


public:

    ServerHandler(asio::io_service &service)
            : service_(service), socket_(service), write_strand_(service) {

    };

    asio::ip::tcp::socket &socket() {

        return this->socket_;
    }

    void start() {

        readHeader();   // @todo pass this to the service. It will block the main thread otherwise
    }

    void send(Message &message) {

        service_.post(write_strand_.wrap([me = shared_from_this(), message]() {

            me->queueMessage(message);
        }));
    }

    void readHeader() {

#if(DEBUG > 0)
        std::cout << "Starting to read header... ";
#endif

        headerBuffer.clear();
        headerBuffer.resize(9);
        asio::async_read(socket_, asio::buffer(headerBuffer, 9), [me = shared_from_this()](asio::error_code const &ec, std::size_t bytes) {

            me->readHeaderDone(ec, bytes);
        });
    }

    void readHeaderDone(asio::error_code error, std::size_t bytes) {

        if (error) {
#if(DEBUG > 0)
            std::cout << error.message() << std::endl;
#endif
            return;
        }

#if(DEBUG > 0)
        std::cout << " - done!" << std::endl;
#endif

        // @fixme halts when message is smaller than 11 bytes or the length of the message is set incorrectly

#if(DEBUG > 1)
        std::cout << "Bytes read: " << bytes << std::endl;

        std::cout << "Read data:" << std::endl;
        for (unsigned char i: headerBuffer) {

            std::cout << (int) i << std::endl;
        }
#endif

        Message message;
        message.header.id = headerBuffer[0];

        for (uint8_t i = 0; i < 8; ++i) {

            uint64_t n = headerBuffer[8 - i];
            message.header.length |= n << (8 * i);
        }

#if(DEBUG > 1)
        std::cout << "Id: " << (int) message.header.id << std::endl;
        std::cout << "Length: " << message.header.length << std::endl;
#endif

        if (message.header.length > 0) {

            readBody(error, bytes, message);
        } else {

            process(message);

            service_.post([me = shared_from_this()]() {     // @annotation mistakes here?

                me->readHeader();
            });
        }
    }

    void readBody(asio::error_code const &error, std::size_t, const Message &message) {

#if(DEBUG > 0)
        std::cout << "Starting to read body... " << std::endl;
#endif

        bodyBuffer.clear();
        bodyBuffer.resize(message.header.length);
        asio::async_read(socket_, asio::buffer(bodyBuffer, message.header.length), [me = shared_from_this(), message](asio::error_code const &ec, std::size_t bytes) {

            me->readBodyDone(ec, bytes, message);
        });
    }

    void readBodyDone(asio::error_code error, std::size_t bytes, Message message) {

        if (error) {
#if(DEBUG > 0)
            std::cout << error.message() << std::endl;
#endif
            return;
        }

#if(DEBUG > 0)
        std::cout << " - done!" << std::endl;
#endif
#if(DEBUG > 1)
        std::cout << "Bytes read: " << bytes << std::endl;

        std::cout << "Read data:" << std::endl;
        for (unsigned char i: bodyBuffer) {

            std::cout << (int) i << std::endl;
        }
#endif

        message.body.assign(bodyBuffer.begin(), bodyBuffer.end());

        process(message);

        service_.post([me = shared_from_this()]() {     // @annotation mistakes here?

            me->readHeader();
        });
    }

    void process(Message message) {

        /// The assumption for the following code is made, that the message has already been decrypted as needed
        switch (message.header.id) {


        }
    }
};