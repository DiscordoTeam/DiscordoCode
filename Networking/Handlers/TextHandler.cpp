//
// Created by pietb on 18.06.2022.
//

#include "../Handlers.h"

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

            users.find(1)->second->queueMessage(m.buildMessage());
        }
    }

}

void TextHandler::onMessageReceived(Message message) {

    std::cout << "Message received" << std::endl;
    switch (message.header.id) {

        case TEXT_MESSAGE:

            BigNum n;
            message >> n;

            std::cout << n.toString() << std::endl;
            break;
    }
}