//
// Created by jeuio on 15/06/2022.
//

#pragma once

#include "Networking.h"
#include <string>

enum MessageIDs {

    NONE = 0,
    ECC_POINT_X, ECC_POINT_Y, TEXT_MESSAGE, REGISTER, LOG_IN
};

class Registration : public MessageHandler {

    std::string name;
    std::string mail;
    std::string password;

    void onConnected() override;

    void onMessageReceived(Message message) override;

public:
    [[nodiscard]] MessageHandler *clone() const override;

    Registration();
};

class Authentication : public MessageHandler {

    BigNum privateKey;
    ECC::ECPoint privatePoint, B;
    bool privatePointGenerated = false; // This is not needed until multiple threads can act on this instance

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
    [[nodiscard]] MessageHandler *clone() const override;

    TextHandler();
};