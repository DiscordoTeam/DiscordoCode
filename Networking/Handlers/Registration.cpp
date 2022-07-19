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

        std::cout << "Enter 'register' for creating a new account or 'login' to use an existing one" << std::endl;

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
    bool valid = false;

    User user;
    Message logInMessage;

    switch(message.header.id) {

        case REGISTER:
            message >> password;
            message >> mail;
            message >> name;

            uint64_t iDInt;

            inputStream.open("maxId");
            inputStream >> iDInt;
            inputStream.close();

            for (uint64_t i = 1; i <= iDInt; ++i) {

                std::ifstream fs(std::to_string(i));
                nlohmann::json json;
                fs >> json;
                fs.close();

                if (mail == json["email"] && password == json["password"]) {

                    valid = true;

                    users->insert( {i, this } );

                    logInMessage.header.id = LOG_IN_FINAL;
                    logInMessage << true;
                    logInMessage << true;
                    logInMessage << i;
                    sendMessage(logInMessage);
                    break;

                }
            }
            if (!valid) {

                users->insert( { user.uinitialization(name, mail, password) , this } );
            }

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

            idInt++;

            for (uint64_t i = 1; i < idInt; ++i) {

                std::ifstream fs(std::to_string(i));
                nlohmann::json json;
                fs >> json;
                fs.close();

                logInMessage.header.id = LOG_IN_FINAL;

                if (mail == json["email"] && password == json["password"]) {

                    valid = true;
                    logInMessage << false;
                    logInMessage << true;
                    logInMessage << i;
                    users->insert( {i, this } );
                    break;
                }
            }

            if (!valid) {

                uint64_t filler;
                logInMessage << false;
                logInMessage << false;
                logInMessage << filler;
            }

            mail = "";
            password = "";

            sendMessage(logInMessage);

            break;

        case LOG_IN_FINAL:

            uint64_t userID = 0;
            bool success2;
            bool cameFromRegister = false;

            message >> userID;
            message >> success2;
            message >> cameFromRegister;

            if(success2) {

                if(cameFromRegister) {

                    std::cout << "This user already exists. You will be logged in to that account now!" << std::endl;
                }
                std::cout << "LogIn to User " << userID << " was successful!" << std::endl;
            } else {

                std::cout << "LogIn was unsuccessful. Check for typos and try again!" << std::endl;
            }

            break;
    }
}