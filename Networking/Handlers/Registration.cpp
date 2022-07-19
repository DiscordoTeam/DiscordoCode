//
// Created by pietb on 19.07.2022.
//

#include "../Handlers.h"
#include "../../User.h"

Registration::Registration() {

    startingId = REGISTER;
    endingId = LOG_IN_FINAL;
}

MessageHandler *Registration::clone() const {

    return new Registration(*this);
}

void Registration::blockingOnConnected() {

    if (clientHandler != nullptr) {

        std::string command;
        std::cin >> command;

        if (command == "register") {

            std::cout << "Please enter your desired Username: " << std::endl;
            std::cin >> name;

            std::cout << "Please enter your EMail-Address: " << std::endl;
            std::cin >> mail;

            std::cout << "Please enter your desired Password: " << std::endl;
            std::cin >> password;

            Message message;
            message.header.id = REGISTER;
            message << name;
            message << mail;
            message << password;

            name = "";
            mail = "";
            password = "";

            sendMessage(message);

        } else if (command == "login") {

            std::cout << "Please enter your EMail-Address: " << std::endl;
            std::cin >> mail;

            std::cout << "Please enter your Password: " << std::endl;
            std::cin >> password;

            Message message;
            message.header.id = LOG_IN;
            message << mail;
            message << password;

            mail = "";
            password = "";

            sendMessage(message);
        }
    }
}

void Registration::onConnected() {}

void Registration::onMessageReceived(Message message) {

    std::ifstream inputStream;
    bool success = false;

    User user;
    Message logInMessage;

    switch(message.header.id) {

        case REGISTER:
            message >> password;
            message >> mail;
            message >> name;

            user.uinitialization(name, mail, password);

            name = "";
            mail = "";
            password = "";

            break;

        case LOG_IN:

            message >> password;
            message >> mail;

            uint64_t idInt;

            inputStream.open("maxId");
            inputStream >> idInt;
            inputStream.close();

            for (uint64_t i = 1; i < idInt; ++i) {

                std::ifstream fs(std::to_string(i));
                nlohmann::json json;
                fs >> json;

                logInMessage.header.id = LOG_IN_FINAL;

                if (mail == json["email"] && password == json["password"]) {

                    logInMessage << true;
                    logInMessage << i;
                } else {

                    logInMessage << false;
                    logInMessage << 0;
                }

                users->insert( { i, this } );

                break;
            }

            mail = "";
            password = "";

            sendMessage(logInMessage);

            break;

        case LOG_IN_FINAL:

            uint64_t userID = 0;
            bool success = false;

            message >> userID;
            message >> success;

            if(success) {

                std::cout << "LogIn to User " << userID << " was successful!" << std::endl;
            } else {

                std::cout << "LogIn was unsuccessful. Check for typos and try again!" << std::endl;
            }

            break;
    }
}