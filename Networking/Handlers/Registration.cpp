//
// Created by pietb on 19.07.2022.
//

#include "../Handlers.h"
#include "../../User.h"

Registration::Registration() {

    startingId = REGISTER;
    endingId = REGISTER;
}

MessageHandler *Registration::clone() const {

    return new Registration(*this);
}

void Registration::blockingOnConnected() {

    if (clientHandler != nullptr) {

        std::cout << "Please enter your desired Username: " << std::endl;
        std::string username;
        std::cin >> username;

        std::cout << "Please enter your EMail-Address: " << std::endl;
        std::string email;
        std::cin >> email;

        std::cout << "Please enter your desired Password: " << std::endl;
        std::string password;
        std::cin >> password;

        Message message;
        message.header.id = REGISTER;
        message << username;
        message << email;
        message << password;

        sendMessage(message);
    }
}

void Registration::onConnected() {}

void Registration::onMessageReceived(Message message) {

    switch(message.header.id) {

        case REGISTER:
            message >> password;
            message >> mail;
            message >> name;

            User user;
            user.uinitialization(name, mail, password);
            break;
    }
}