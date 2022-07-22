//
// Created by jeuio on 21/07/2022.
//

#include "DiscordoClient.h"

void DiscordoClient::initialise() {

    HandlerManager* hm = this->handler.getHandlerManager();

    registration = dynamic_cast<Registration*>(hm->handlers[0]);
    textHandler = dynamic_cast<TextHandler*>(hm->handlers[1]);
}