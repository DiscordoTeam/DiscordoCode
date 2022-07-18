//
// Created by jeuio on 15/06/2022.
//

#pragma once

#include <Networking.h>

enum MessageIDs {

    NONE = 0,
    ECC_POINT_X, ECC_POINT_Y,
    TEXT_MESSAGE
};

class Authentication : public MessageHandler {

    mutable BigNum privateKey;
    mutable ECC::ECPoint privatePoint, B;
    mutable bool privatePointGenerated = false; // This is not needed until multiple threads can act on this instance

    void onConnected() override;

    void onMessageReceived(Message message) override;

    void sendPrivatePoint();

    void deriveSessionKey();

public:
    [[nodiscard]] MessageHandler* clone() const override;

    Authentication();
};

class TextHandler : public MessageHandler {

    std::string input = "";

    void onConnected() override;

    void onMessageReceived(Message message) override;

public:
    [[nodiscard]] MessageHandler* clone() const override;

    TextHandler();
};