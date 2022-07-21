//
// Created by jeuio on 21/07/2022.
//

#include "DiscordoClient.h"

DiscordoClient::DiscordoClient() {

    HandlerManager* hm = this->handler.getHandlerManager();

    registration = hm->handlers[0];
    textHandler = hm->handlers[1];
}