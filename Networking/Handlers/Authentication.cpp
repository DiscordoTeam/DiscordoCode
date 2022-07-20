//
// Created by jeuio on 15/06/2022.
//

#include "../../DiscordoClient.h"

Authentication::Authentication() {

    startingId = ECC_POINT_X;
    endingId = ECC_POINT_Y;
}

MessageHandler *Authentication::clone() const {

    return new Authentication(*this);
}

void Authentication::onConnected() {

    std::cout << "Starting key exchange" << std::endl;
    sendPrivatePoint();
}

void Authentication::onMessageReceived(Message message) {

    // @fixme Problem with messages arriving simultaneously

    switch (message.header.id) {

        case ECC_POINT_X:

            std::cout << "Received x" << std::endl;
            if (clientHandler != nullptr ? clientHandler->sessionKey.empty() : serverHandler->sessionKey.empty()) {

                message >> B.infinity;
                message >> B.odd;
                message >> B.x;
            }
            break;

        case ECC_POINT_Y:

            std::cout << "Received y" << std::endl;
            if (clientHandler != nullptr ? clientHandler->sessionKey.empty() : serverHandler->sessionKey.empty()) {

                message >> B.y;

                if (!ECC::isOnCurve(B)) {

                    B = ECC::ECPoint();
                    std::cout << "Provided point is invalid" << std::endl;
                    break;
                }

                while (!privatePointGenerated);
                deriveSessionKey();
            }
            break;
    }
}

void Authentication::sendPrivatePoint() {

    std::cout << "Generating private coefficient... ";
    privateKey = ECC::DH::generatePrivateCoefficient();
    std::cout << "done" << std::endl;
    std::cout << "Calculating private point... ";
    std::cout << "Test" << std::endl;
    privatePoint = ECC::DH::calculatePrivatePoint(privateKey);
    std::cout << "done" << std::endl;
    privatePointGenerated = true;

    Message pointX, pointY;

    pointX.header.id = ECC_POINT_X;
    pointX << privatePoint.x;
    pointX << privatePoint.odd;
    pointX << privatePoint.infinity;

    pointX.header.id = ECC_POINT_Y;
    pointY << privatePoint.y;

    std::cout << "Sending x... ";
    sendMessage(pointX);
    std::cout << "done" << std::endl;
    std::cout << "Sending y... ";
    sendMessage(pointY);
    std::cout << "done" << std::endl;
}

void Authentication::deriveSessionKey() {

    if (clientHandler != nullptr) {

        clientHandler->sessionKey = ECC::DH::deriveKey(privateKey, B);
    } else {

        serverHandler->sessionKey = ECC::DH::deriveKey(privateKey, B);
    }

    privatePointGenerated = false;  // Not really necessary but resets the boolean just in case
}