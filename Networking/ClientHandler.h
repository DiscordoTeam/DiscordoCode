//
// Created by jeuio on 06/02/2022.
//

#pragma once

#include <asio.hpp>
#include <deque>
#include <iostream>
#include <utility>
#include "Message.h"
#include "chrono"

class ClientHandler {

    bool initialised = false;

    RSA rsa;
    bool clientHasBiggerN;
    BigNum serverE, serverN;

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



    void encryptMessage(Message &m) {

        m.encrypt([&](BigNum a) {

            return rsa.encrypt(serverN, serverE, std::move(a));
        });
    }

    void decryptMessage(Message &c) {

        c.decrypt([&](BigNum a) {

            return rsa.decrypt(std::move(a));
        });
    }

    void process(Message message) {

        // Decrypt the message if there are more than 0 blocks present
        if (message.header.blocks > 0) {

            decryptMessage(message);
        }

        switch (message.header.id) {

            case MessageID::PING: {

                BigNum num;
                message >> num;
#if(DEBUG > 0)
                std::cout << num.toBaseString(16) << std::endl;
#endif
            }
            case MessageID::KEY_EXCHANGE_E: {

                message >> serverE;

                return;
            }
            case MessageID::KEY_EXCHANGE_N: {

                message >> serverN;

                clientHasBiggerN = rsa.n_ >= serverN;   // @todo set the amount of blocks based on this

#if(DEBUG > 0)
                std::cout << "Server key: " << serverE.toBaseString(16) + " - " << serverN.toBaseString(16) << std::endl;
#endif

                initialised = true;

                return;
            }
        }
    }





public:
    ClientHandler(asio::io_service &service)
            : service_(service), socket_(service) {
    }

    asio::ip::tcp::socket &socket() {

        return this->socket_;
    }



    void writeKeys(std::string path) {

#if(DEBUG > 0)
        std::cout << "Generating a new set of keys...";
#endif
        rsa.generateKeys(); // Generate a new set of keys
#if(DEBUG > 0)
        std::cout << " done!" << std::endl;
#endif

#if(DEBUG > 0)
        std::cout << "Storing keys...";
#endif
        std::ofstream stream(path);
        stream << "keyN:" << std::endl << rsa.n_.toBaseString(16) << std::endl;
        stream << "keyE:" << std::endl << rsa.e_.toBaseString(16) << std::endl;
        stream << "keyD:" << std::endl << rsa.d.toBaseString(16) << std::endl;
        stream.flush();
        stream.close();
#if(DEBUG > 0)
        std::cout << " done!" << std::endl;
#endif
    }

    void readKeys(std::string path) {

        std::ifstream stream(path);

        std::string line, keyN, keyE, keyD;
        while (stream >> line) {

            if (line == "keyN:") {

                stream >> keyN;
                rsa.n_ = "0x" + keyN;
            } else if (line == "keyE:") {

                stream >> keyE;
                rsa.e_ = "0x" + keyE;
            } else if (line == "keyD:") {

                stream >> keyD;
                rsa.d = "0x" + keyD;
            }
        }

        stream.close();
    }

    void keyExchange() {

#if(DEBUG > 0)
        std::cout << "Key: " << rsa.e_.toBaseString(16) << " - " << rsa.n_.toBaseString(16) << std::endl;
#endif

        // Send e-part
        Message e;
        e.header.id = MessageID::KEY_EXCHANGE_E;
        e << rsa.e_;
        send(e);

        // Send n-part
        Message n;
        n.header.id = MessageID::KEY_EXCHANGE_N;
        n << rsa.n_;
        send(n);
    }

    void addEntry(std::string name, const std::vector<std::string> &data) {

        Message nameMessage;
        nameMessage.header.id = MessageID::ADD_ENTRY_NAME;

        BigNum n;
        n.parseString(std::move(name));
        n = rsa.encrypt(serverN, serverE, n);
        nameMessage << n;
        send(nameMessage);

        for (std::string s: data) {

            Message dataMessage;
            dataMessage.header.id = MessageID::ADD_ENTRY_DATA;

            BigNum m;
            m.parseString(s);
            m = rsa.encrypt(rsa.n_, rsa.e_, m);
            dataMessage << m;
            send(dataMessage);
        }

        Message dataMessage;
        dataMessage.header.id = MessageID::ADD_ENTRY_DATA;
        send(dataMessage);
    }



    void start() {

        readHeader();
    }

    void send(Message &message) {

        service_.post([=] {

            queueMessage(message);
        });
    }

    void sendEncrypted(Message &message) {

        encryptMessage(message);

        send(message);
    }
};