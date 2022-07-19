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

    return new LogIn(*this);
}

void LogIn::blockingOnConnected() {

}