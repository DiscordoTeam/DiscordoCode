//
// Created by root on 18.07.22.
//

#include "../DiscordoClient.h"

TextMessage::TextMessage(Message message) {

    message >> fromID;
    message >> targetID;
    message >> content;
}

TextMessage::TextMessage(uint64_t fromID, uint64_t targetID, std::string message) {

    this->fromID = fromID;
    this->targetID = targetID;
    this->content = std::move(message);
}

Message TextMessage::buildMessage() {

    Message message;
    message.header.id = TEXT_MESSAGE;
    message << content;
    message << targetID;
    message << fromID;

    return message;
}