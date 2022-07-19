//
// Created by pietb on 19.07.2022.
//

#include "../Handlers.h"

Registration::Registration() {

    startingId = REGISTER;
    endingId = REGISTER;
}

MessageHandler *Registration::clone() const {

    return new Registration(*this);
}

void Registration::onConnected() {


}

void Registration::onMessageReceived(Message message) {


}