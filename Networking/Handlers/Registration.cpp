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

void Registration::onConnected() {


}

void Registration::onMessageReceived(Message message) {

    message >> name;
    message >> mail;
    message >> password;

    User user;
    user.uinitialization(name, mail, password);
}