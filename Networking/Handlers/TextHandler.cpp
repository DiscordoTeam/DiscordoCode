//
// Created by pietb on 18.06.2022.
//

#include "../Handlers.h"
#include "../CustomMessages.h"

TextHandler::TextHandler() {

    startingId = IDENTIFY;
    endingId = TEXT_MESSAGE;
}

MessageHandler *TextHandler::clone() const {

    return new TextHandler(*this);
}

void TextHandler::blockingOnConnected() {

    if (clientHandler != nullptr) {
        uint64_t fromID;

        std::cout << "Please enter the id you identify with: ";
        std::cin >> fromID;

        Message mes;
        mes.header.id = IDENTIFY;
        mes << fromID;
        sendMessage(mes);

        for (;;) {

            std::cout << "Enter target user id: ";
            uint64_t targetID;
            std::cin >> targetID;

            std::cout << "Enter message:" << std::endl;

            std::string messageString;

            std::cin.ignore();
            for (;;) {
                std::getline(std::cin, input);
                if (input.empty()) {
                    break;
                }

                messageString += input + "\r\n";
            }

            TextMessage m(fromID, targetID, messageString);

            sendMessage(m.buildMessage());
        }
    }
}

void TextHandler::onConnected() {}

void TextHandler::onMessageReceived(Message message) {



    std::cout << "Message received" << std::endl;
    switch (message.header.id) {

        case IDENTIFY:

            uint64_t id;
            message >> id;
            users->insert( { id, this } );
            break;

        case TEXT_MESSAGE:

            TextMessage tm(message);
            if (clientHandler != nullptr) {

                std::cout << "Received message from id " << tm.fromID << ":" << std::endl;
                std::cout << tm.content << std::endl;

            } else {
                std::cout << "Received message from id " << tm.fromID << ":" << std::endl;
                std::cout << "Target id: " << tm.targetID << std::endl;
                std::cout << "Content:" << tm.content << std::endl;
                if(users->find(tm.targetID) != users->end()){

                    users->find(tm.targetID)->second->sendMessage(message);
                }else{
                    std::cout << "User non-existent" << std::endl;
                }
            }
            break;
    }
}