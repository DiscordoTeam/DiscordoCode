//
// Created by jeuio on 12/06/2022.
//

#pragma once

#include <Custom/Networking.h>

class TestHandler : public MessageHandler {


    void onMessageReceived(Message message) override {

        std::cout << "Message received" << std::endl;
    }

public:

    [[nodiscard]] MessageHandler* clone() const override {

        return new TestHandler(*this);
    }

    TestHandler() {

        startingId = 0;
        endingId = 255;
    }
};