//
// Created by jeremy on 18.07.22.
//

#pragma once

#include <Networking.h>

struct TextMessage {



    uint64_t fromID, targetID;
    std::string content;

    explicit TextMessage(Message message);

    TextMessage(uint64_t fromID, uint64_t targetID, std::string message);

    Message buildMessage();
};

