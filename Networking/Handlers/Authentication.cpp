//
// Created by jeuio on 15/06/2022.
//

#include "../Handlers.h"

Authentication::Authentication() {

    startingId = NONE;
    endingId = NONE;
}

MessageHandler *Authentication::clone() const {

    return new Authentication(*this);
}

void Authentication::onConnected() {

    sendPrivatePoint();
}

void Authentication::onMessageReceived(Message message) {

    switch (message.header.id) {

        case ECC_POINT_X:

            if (clientHandler != nullptr ? clientHandler->sessionKey.empty() : serverHandler->sessionKey.empty()) {

                message >> B.infinity;
                message >> B.odd;
                message >> B.x;
            }
            break;

        case ECC_POINT_Y:

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

    privateKey = ECC::DH::generatePrivateCoefficient();
    privatePoint = ECC::DH::calculatePrivatePoint(privateKey);
    privatePointGenerated = true;

    Message pointX, pointY;

    pointX.header.id = ECC_POINT_X;
    pointX << privatePoint.x;
    pointX << privatePoint.odd;
    pointX << privatePoint.infinity;

    pointX.header.id = ECC_POINT_Y;
    pointY << privatePoint.y;

    sendMessage(pointX);
    sendMessage(pointY);
}

void Authentication::deriveSessionKey() {

    if (clientHandler != nullptr) {

        clientHandler->sessionKey = ECC::DH::deriveKey(privateKey, B);
    } else {

        serverHandler->sessionKey = ECC::DH::deriveKey(privateKey, B);
    }

    privatePointGenerated = false;  // Not really necessary but resets the boolean just in case
}