//
// Created by jeuio on 05/02/2022.
//

#pragma once

#include <iostream>
#include <vector>

// @todo in order to make this into a usable kind of library, this needs to only be declared here and the user will need to initialise this in their own implementation
/**
 * @brief For giving IDs to messages in order for them to be recognized in some sort of protocol
 * @note For security reasons, it is advised to transmit all except for the key exchange messages in encrypted form
 */
enum MessageID {

    ERR, PING, KEY_EXCHANGE_E, KEY_EXCHANGE_N, USERNAME, REGISTER_PASSWORD, LOGIN_PASSWORD, ADD_ENTRY_NAME, ADD_ENTRY_DATA
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



    /**
     * @brief Encrypts a message bades on the number of blocks taht make up the message
     * @param encryptionFunction<BigNum(BigNum)> the function that shall be used for encryption. RSA is recommended
     */
    void encrypt(const std::function<BigNum(BigNum)>& encryptionFunction) {

        BigNum m, c;
        *this >> m;

        uint64_t blockSize = c.length() / this->header.blocks;

        for (uint64_t i = 0; i < this->header.blocks; ++i) {

            c = c | (encryptionFunction(m.subBits(0, blockSize)) << blockSize * i);

            c = c << (blockSize * i);
            m = m >> (blockSize * i);
        }

        *this << c;
    }

    /**
     * @brief Decrypts a message based on the number of blocks that make up the message
     * @param decryptionFunction<BigNum(BigNum)> function that shall be used for decryption. RSA is recommended
     */
    void decrypt(const std::function<BigNum(BigNum)>& decryptionFunction) {

        BigNum c, m;
        *this >> c;

        uint64_t blockSize = c.length() / this->header.blocks;

        for (uint64_t i = 0; i < this->header.blocks; ++i) {

            m = m | (decryptionFunction(c.subBits(0, blockSize)) << blockSize * i);

            m = m << (blockSize * i);
            c = c >> (blockSize * i);
        }

        *this << m;
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