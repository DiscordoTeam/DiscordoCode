//
// Created by pietb on 18.06.2022.
//

#include "../Handlers.h"
#include "../CustomMessages.h"

TextHandler::TextHandler() {

    startingId = TEXT_MESSAGE;
    endingId = TEXT_MESSAGE;
}

MessageHandler *TextHandler::clone() const {

    return new TextHandler(*this);
}

void TextHandler::onConnected() {

    if (clientHandler != nullptr) {
        uint64_t fromID;

        std::cout << "Please enter the id you identify with" << std::endl;
        std::cin >> fromID;


        for (;;) {

            std::cout << "Enter target user id:" << std::endl;
            uint64_t targetID;
            std::cin >> targetID;

            std::cout << "Enter message:" << std::endl;
            std::string input;
            std::getline(std::cin, input);

            BigNum n;
            n.parseString(input);



            TextMessage m(fromID, targetID , n);

            sendMessage(m.buildMessage());
        }
    }
}

void TextHandler::onMessageReceived(Message message) {



    std::cout << "Message received" << std::endl;
    switch (message.header.id) {

        case IDENTIFY:

            uint64_t id;
            message >> id;
            users.at(id) = this;
            break;

        case TEXT_MESSAGE:

            TextMessage tm(message);
            if (clientHandler != nullptr) {

                std::cout << "Received message from id " << tm.fromID << ":" << std::endl;
                std::cout << tm.content.toString() << std::endl;
            } else {

                users.find(tm.targetID)->second->sendMessage(message);
            }
            break;
    }
}