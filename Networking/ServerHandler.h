//
// Created by jeuio on 06/02/2022.
//

#pragma once

#include <memory>
#include <asio.hpp>
#include <deque>
#include <fstream>
#include <thread>

#include "../RSA.h"
#include "Message.h"

class ServerHandler
        : public std::enable_shared_from_this<ServerHandler> {

    bool initialised = false;
    bool authorised = false;

    bool receivingEntry = false;
    std::ofstream entryStream;

    RSA *rsa;
    bool serverHasBiggerN;
    BigNum clientE, clientN;

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

    ServerHandler(asio::io_service &service, RSA &rsa)
            : service_(service), socket_(service), write_strand_(service) {

        this->rsa = &rsa;
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

    void encryptMessage(Message &m) {

        m.encrypt([&](BigNum a) {

            return rsa->encrypt(rsa->n_, rsa->e_, std::move(a));
        });
    }

    void decryptMessage(Message &c) {

        c.decrypt([&](BigNum a) {

            return rsa->decrypt(std::move(a));
        });
    }

    void process(Message message) {

        // Decrypt the message if there are more than 0 blocks present
        if (message.header.blocks > 0) {

            decryptMessage(message);
        }

        /// The assumption for the following code is made, that the message has already been decrypted as needed
        switch (message.header.id) {

            case MessageID::PING: {     // Send the message back, just as is was received

                BigNum num;
                message >> num;
#if(DEBUG > 0)
                std::cout << rsa->decrypt(num).toBaseString(16) << std::endl;
                std::cout << num.toBaseString(16) << std::endl;
#endif
                send(message);
                return;
            }
            case MessageID::KEY_EXCHANGE_E: {   // Store the received e-part of the clients public key and send the own

                message >> clientE;

                Message reply;
                reply.header.id = MessageID::KEY_EXCHANGE_E;
                reply << rsa->e_;
                send(reply);

                return;
            }
            case MessageID::KEY_EXCHANGE_N: {   // Store the received n-part of the clients public key and send the own

                message >> clientN;

                serverHasBiggerN = rsa->n_ >= clientN;  // @todo set the amount of blocks based on this

                Message reply;
                reply.header.id = MessageID::KEY_EXCHANGE_N;
                reply << rsa->n_;
                send(reply);

#if(DEBUG > 0)
                std::cout << "Key: " << rsa->e_.toBaseString(16) << " - " << rsa->n_.toBaseString(16) << std::endl;
                std::cout << "Client key: " << clientE.toBaseString(16) << " - " << clientN.toBaseString(16) << std::endl;
#endif

                initialised = true;

                return;
            }
            case MessageID::USERNAME: {     // Receive the users username and store it in memory for later use

                BigNum num;
                message >> num;

                username = num.toString();

#if(DEBUG > 0)
                std::cout << "Encrypted username: " << num.toBaseString(16) << std::endl;
                std::cout << "Username: " << rsa->decrypt(num).toBaseString(16) << std::endl;
#endif

                return;
            }
            case MessageID::REGISTER_PASSWORD: {    // Assume, that the username is already known and proceed to create a new user account, if none exists with the current username

                BigNum num;
                message >> num;

                std::ifstream checkStream("res\\Userdata\\" + username);
                if (checkStream.good()) { // If the user already exists, return an error

                    Message m;
                    m.header.id = MessageID::ERR;
                    send(m);
                    return;
                }

                std::ofstream stream("res\\Userdata\\" + username);

                // Create the user
                stream << num.toBaseString(16);
                stream.flush();
                stream.close();

                system(("mkdir res\\Users\\" + username).c_str());
                return;
            }
            case MessageID::LOGIN_PASSWORD: {   // Assume, that the username is already known and compare the hash of the received password with the one from the users file. Send an error back as needed @fixme the login is inconsistent between program instances

                std::ifstream stream("res\\Userdata\\" + username); // Open the file with the users name

                if (!stream.good()) {   // If the user does not exist, return an error

                    // @todo return error to the client

                    return;
                }

                BigNum localPassword;
                std::string passwordString;
                stream >> passwordString;
                stream.close();

                localPassword = "0x" + passwordString;
#if(DEBUG > 0)
                std::cout << "Local password: " << localPassword.toBaseString(16) << std::endl;
#endif

                BigNum messagePassword;
                message >> messagePassword;
                messagePassword = messagePassword;
#if(DEBUG > 0)
                std::cout << "Received password: " << messagePassword.toBaseString(16) << std::endl;
#endif

                if (messagePassword == localPassword) {

#if(DEBUG > 0)
                    std::cout << "Client received authorisation" << std::endl;
#endif
                    authorised = true;
                }

                return;
            }
            case MessageID::ADD_ENTRY_NAME: {

                if (!authorised) {

                    // @todo consider returning an error to the client

                    return;
                }

                BigNum m;
                message >> m;

                // @todo check if name already exists

                entryStream.open("res\\Users\\" + username + "\\" + m.toString());
                receivingEntry = true;

                return;
            }
            case MessageID::ADD_ENTRY_DATA: {

                if (!authorised) {

                    // @todo consider returning an error to the client

                    return;
                }

                if (message.body.empty()) {

                    receivingEntry = false;
                    entryStream.flush();
                    entryStream.close();
                    return;
                }

                BigNum m;
                message >> m;
#if(DEBUG == 0)
                std::cout << m.toString() << std::endl;
#endif

                entryStream << m.toString() << std::endl;

                return;
            }
        }
    }
};