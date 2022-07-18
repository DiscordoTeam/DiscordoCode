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

    std::cin >> input;

    BigNum n;
    n.parseString(input);

    Message m;

    m.header.id = TEXT_MESSAGE;
    m << n;

    sendEncryptedMessage(m);
}

void TextHandler::onMessageReceived(Message message) {

    switch (message.header.id) {

        case TEXT_MESSAGE:

            BigNum n;
            message >> n;

            std::cout << n.toString() << std::endl;
            break;
    }
}