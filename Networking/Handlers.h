//
// Created by jeuio on 15/06/2022.
//

#pragma once

#include <Networking.h>
#include <string>
#include <map>

enum MessageIDs {

    NONE = 0,
    ECC_POINT_X, ECC_POINT_Y,
    IDENTIFY, TEXT_MESSAGE,
    REGISTER, LOG_IN
};

class Registration : public MessageHandler {

    std::string name;
    std::string mail;
    std::string password;

    void blockingOnConnected() override;

    void onConnected() override;

    void onMessageReceived(Message message) override;

public:
    [[nodiscard]] MessageHandler *clone() const override;

    Registration();
};

class Authentication : public MessageHandler {

    mutable BigNum privateKey;
    mutable ECC::ECPoint privatePoint, B;
    mutable bool privatePointGenerated = false; // This is not needed until multiple threads can act on this instance

    void blockingOnConnected() override;

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

    std::map<uint64_t, MessageHandler*>* users = new std::map<uint64_t, MessageHandler*>();     // @todo Care about deletion

    void blockingOnConnected() override;

    void onConnected() override;

    void onMessageReceived(Message message) override;

public:
    [[nodiscard]] MessageHandler* clone() const override;

    TextHandler();
};