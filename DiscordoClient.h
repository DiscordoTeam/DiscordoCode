//
// Created by jeuio on 20/07/2022.
//

#pragma once

#include <Networking.h>
#include <iostream>
#include <vector>
#include <array>
#include "json.hpp"
#include <fstream>

#pragma once

#ifdef _WIN32
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601
#endif

struct User {

    std::string name;
    std::string bio;
    std::string password;
    std::string mail;

    uint16_t idFrontEnd;
    uint64_t idBackEnd;

    uint32_t phone;

    bool online;
    bool afk;
    bool dnd;
    bool invisible;

    std::vector<uint32_t> servers;
    std::vector<uint32_t> friends;
    std::vector<uint32_t> groups;

    void udelete(uint64_t userID);
    void unoActivity();
    void udnd();
    void uinvisible();
    void udisconnect();
    uint64_t uinitialization(std::string name, std::string email, std::string password);
    uint64_t uinitializationIntoFreeSpot(std::string name, std::string email, std::string password, uint64_t freeID);

    enum a {

        PrivacyPower, WritingPower, SpeakingPower
    };

    uint8_t powerLevelList[3];
};

struct TextChannel {

    std::string name;
    std::string bio;

    uint32_t mother;

    uint64_t allowedDataSize;

    uint8_t requiredPrivacyPower;
    uint8_t requiredWritingPower;
    uint8_t requiredEditingPower;
    uint8_t requiredTransferPower;

    bool dataRestriciton = false;

    std::vector<uint32_t> members;
};

enum MessageIDs {

    NONE = 0,
    ECC_POINT_X, ECC_POINT_Y,
    IDENTIFY, TEXT_MESSAGE,
    REGISTER, LOG_IN, LOG_IN_FINAL
};

extern std::map<uint64_t, MessageHandler*>* users;

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

    uint64_t clientId;
    std::string input = "";

    void blockingOnConnected() override;

    void onConnected() override;

    void onMessageReceived(Message message) override;

public:
    [[nodiscard]] MessageHandler* clone() const override;

    ~TextHandler() override;

    TextHandler();
};

struct TextMessage {

    uint64_t fromID, targetID;
    std::string content;

    explicit TextMessage(Message message);

    TextMessage(uint64_t fromID, uint64_t targetID, std::string message);

    Message buildMessage();
};