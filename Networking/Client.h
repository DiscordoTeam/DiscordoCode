//
// Created by jeuio on 06/02/2022.
//

#pragma once

#include <utility>
#include <fstream>

#include "../Networking/ClientHandler.h"
#include "Message.h"

#define CONFIG_PATH "config"

class Client {

    asio::io_service service;
    ClientHandler handler = ClientHandler(service);

public:

    Client() {

        loadConfig();
    }

    void run() {

        keyExchange();
        service.run();
    }

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

    void send(Message message) {

        handler.send(message);
    }

    void loadConfig() {

#if(DEBUG > 0)
        std::cout << "Loading config file..." << std::endl;
#endif

        std::ifstream inStream(CONFIG_PATH);
        if (!inStream.good()) {

#if(DEBUG > 0)
            std::cout << "No config file found" << std::endl;
#endif

            inStream.close();

            // Create a new config file and ask the user where to store the keys
            std::string configPath;
            std::cout << "Please enter the path to a secure storage location, preferably to an external storage unit:" << std::endl;
            std::cin >> configPath;

            // @todo check whether the path is valid

            std::ofstream outStream(CONFIG_PATH);
            outStream << "keys:" << std::endl << configPath << std::endl;
            outStream.flush();
            outStream.close();

            handler.writeKeys(configPath);
        } else {

            std::string line;

            std::string keyPath;

            while (inStream >> line) {

#if(DEBUG > 1)
                std::cout << line << std::endl;
#endif

                if (line == "keys:") {

                    std::string keyLocation;
                    inStream >> keyLocation;
#if(DEBUG > 1)
                    std::cout << keyLocation << std::endl;
#endif
                    handler.readKeys(keyLocation);
                }
            }
        }
    }

    void keyExchange() {

        handler.keyExchange();
    }

    void register_(std::string username, std::string password) {

        BigNum u, p;
        u.parseString(username);
        p.parseString(password);
        p = Hashing::Scrypt::hash(p, 4, 8, 10, 50); // @todo put these parameters into macros

        Message un, pw;
        un.header.id = MessageID::USERNAME;
        un << u;
        pw.header.id = MessageID::REGISTER_PASSWORD;
        pw << p;

        handler.sendEncrypted(un);
        handler.sendEncrypted(pw);
    }

    void login(std::string username, std::string password) {

        BigNum u, p;
        u.parseString(std::move(username));
        p.parseString(std::move(password));
        p = Hashing::Scrypt::hash(p, 4, 8, 10, 50);

        Message un, pw;
        un.header.id = MessageID::USERNAME;
        un << u;
        pw.header.id = MessageID::LOGIN_PASSWORD;
        pw << p;

        handler.sendEncrypted(un);
        handler.sendEncrypted(pw);
    }

    void addEntry(std::string name, const std::vector<std::string> &data) {

        handler.addEntry(std::move(name), data);
    }
};