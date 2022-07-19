//
// Created by pietb on 18.07.2022.
//
#include <iostream>
#include <BigNum.h>
#include <vector>

#pragma once

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