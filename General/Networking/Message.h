//
// Created by jeuio on 05/02/2022.
//

#pragma once

#include <iostream>
#include <vector>

#include "../BigNum/BigNum.h"

// @todo in order to make this into a usable kind of library, this needs to only be declared here and the user will need to initialise this in their own implementation
/**
 * @brief For giving IDs to messages in order for them to be recognized in some sort of protocol
 */
enum MessageID {


};

struct Message {

    /**
     * @brief The header of a message
     * @note the length will always be 11 bytes
     */
    typedef struct Header {

        uint8_t id = 0;
        uint16_t blocks = 1;
        uint64_t length = 0;
    } Header;

    Header header;
    mutable std::vector<uint8_t> body;

    /**
     * @brief Returns the header data
     * @return header data
     */
    std::vector<uint8_t> headerData() {

        std::vector<uint8_t> data;
        data.push_back(header.id);
        for (int8_t i = 7; i >= 0; --i) {
            data.push_back(header.length >> (i * 8));
        }

        return data;
    }

    /**
     * Return the complete data of the message
     * @return message data
     */
    std::vector<uint8_t> data() {

        std::vector<uint8_t> data;
        std::vector<uint8_t> hData = headerData();
        data.assign(hData.begin(), hData.end());
        data.insert(data.begin() + 9, body.begin(), body.end());

        return data;
    }



    /////////////////////////////////////////////////////////////////////////////////////////



    void operator<<(BigNum &v) {

        for (uint64_t n: v.number.n) {

            for (uint8_t i = 0; i < 8; ++i) {

                uint8_t a = n;
                n >>= 8;
                this->body.push_back(a);
            }
        }
        this->header.length = v.number.n.size() * 8;
    }

    void operator>>(BigNum &v) {

        v.number.n.clear();
        for (uint64_t i = 0; i < this->header.length / 8; ++i) {

            uint64_t n = 0;
            for (int8_t j = 7; j >= 0; --j) {

                n <<= 8;
                n |= this->body[i * 8 + j];
            }
            v.number.n.push_back(n);
        }

        this->body.clear();
        this->header.length = 0;
    }

    template<typename DataType>
    void operator<<(DataType value) {

        static_assert(std::is_standard_layout<DataType>());     // Check whether the provided data is a primitive

        uint8_t temp;
        for (uint64_t i = 0; i < sizeof(DataType); ++i) {

            temp = value;
            this->body.push_back(temp);
            value >>= 8;
        }

        this->header.length += sizeof(DataType);
    }

    template<typename DataType>
    void operator>>(DataType &value) {

        static_assert(std::is_standard_layout<DataType>());     // Check whether the provided data is a primitive

        value = 0;

        for (uint64_t i = 0; i < sizeof(DataType); ++i) {

            DataType d = this->body.back();
            d <<= ((sizeof(DataType) - i - 1) * 8);
            value |= d;
            this->body.erase(this->body.end() - 1);
        }

        this->header.length -= sizeof(DataType);
    }
};