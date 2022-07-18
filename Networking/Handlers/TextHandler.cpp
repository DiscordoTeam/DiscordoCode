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
    for(;;) {
        std::string input;
        std::getline(std::cin, input);

        BigNum n;
        n.parseString(input);

        Message m;
        m.header.id = TEXT_MESSAGE;
        m << n;

        sendMessage(m);
    }
}

void TextHandler::onMessageReceived(Message message) {

    std::cout << "received message" << std::endl;
    switch (message.header.id) {

        case TEXT_MESSAGE:

            BigNum n;
            message >> n;

            std::cout << n.toString() << std::endl;
            break;
    }
}