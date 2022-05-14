//
// Created by jeuio on 19/12/2021.
//

#pragma once

#include <iostream>
#include <cassert>
#include <vector>

// @todo needs to be THOROUGHLY checked for errors
// @todo start to use templates

typedef struct {

    bool negative;
    std::vector<uint64_t> n;

} uint;

class BigNum {

public:

    std::string standard = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string extended = standard + "!\"§$%&/()=?ß+#<>.,@*+^°|;:_´`äüö";

    uint number;   // @todo make this private again eventually

    BigNum() { //wenn einfach nur BigNum erstellt

        this->number.negative = false;
        this->number.n.push_back(0);
    }

    explicit BigNum(const char *s) { //wenn spezifizierter Konstruktor

        BigNum a; //temporär
        a = s; //s ist so ziemlich n String, zeigt auf den Anfang von String
        *this = a;
    }

    std::string toString() {

        std::string result;

        for (uint64_t n: this->number.n) {

            for (uint8_t i = 0; i < 8; ++i) {

                uint8_t c = n >> ((7 - i) * 8);

                if (c > 0) {

                    result += c;
                }
            }
        }

        return result;
    }

    void parseString(std::string s) {

        BigNum result;
        result.number.n.clear();

        uint64_t i;
        for (i = 0; i < s.size() / 8; ++i) {

            uint64_t n = 0;
            for (uint8_t j = 0; j < 8; ++j) {

                n <<= 8;
                n |= s.at(i * 8 + j);
            }
            result.number.n.push_back(n);
        }

        if (s.size() % 8 > 0) {
            uint64_t n = 0;
            for (uint64_t j = 0; j < s.size() % 8; ++j) {

                n <<= 8;
                n |= s.at(i * 8 + j);
            }
            result.number.n.push_back(n);
        }

        *this = result;
    }

    uint64_t length() {

        BigNum a = *this; //this bezieht sich auf BigNum von dem das aufgerufen wird

        a.fit(); //alle unnötigen 0en werden entfernt
        if (a.number.n.size() == 1 && a.number.n[0] == 0) return 0;

        uint64_t length = (a.number.n.size() - 1) * 64;
        uint64_t mask = 1;
        for (uint8_t i = 0; i < 64; ++i) {

            length++;
            if (a.number.n.back() & mask) break; //Vergleich, ob der 1er Bit bei dem iterierten uint64 auf der selben Höhe ist, wie er erste von a
            mask <<= 1; //mask wird 1 nach links geshifted
        }

        return length;
    }

    // @todo test for errors
    BigNum subBits(uint64_t start, uint64_t end) {

        end = end > length() ? length() : end;



        uint64_t mask = UINT64_MAX;
        BigNum result, n = *this;

        if (end - start <= 64) { //@todo Prüfen i guess

            mask = mask << (64 - (end - start));
            mask = mask >> (64 - end);

            result.number.n[0] = n.number.n[start / 64] & mask;
            result = result << (start / 64 * 64);
            return result;
        }

        result.number.n.clear();

        result.number.n.push_back(n.number.n[start / 64] & (mask << (start % 64)));

        for (uint64_t i = start / 64 + 1; i < end / 64;  ++i) {

            result.number.n.push_back(n.number.n[i]);
        }

        if (end % 64 != 0) {

            result.number.n.push_back(n.number.n[end / 64] & (mask >> (64 - (end % 64))));
        }

        result.fit();

        return result;
    }

    std::string toBinString() {

        std::string out;

        BigNum a = *this;

        if (a.number.negative) {

            out += "-";
        }

        bool leadingZeros = true;
        for (uint64_t i = 0; i < this->number.n.size() * 64; ++i) {

            if (((a >> (this->number.n.size() * 64 - 1 - i)).number.n.at(0) & 1) == 1) {

                out += "1";
                leadingZeros = false;
            } else if (!leadingZeros) {

                out += "0";
            }
        }

        if (leadingZeros) {

            out += "0";
        }

        return out;
    }

    std::string toHexString() {

        std::string hexChars = "0123456789ABCDEF"; //std::string = ""; ist äquivalent zu String = "";
        std::string out;

        BigNum a = *this;

        bool leadingZeros = true;
        for (uint64_t i = 0; i < this->number.n.size() * 16; ++i) {

            uint64_t num = a.number.n.front() & 0xF; //ganz rechten 4 Bits
            a = a >> 4;

            if (num > 0) {

                out += hexChars.at(num); //at(num); ist äquivalent zu .charAt(num);
                leadingZeros = false;
            } else if (!leadingZeros) {

                out += "0";
            }
        }

        if (leadingZeros) {

            out += "0";
        }

        std::string temp = out;
        out = a.number.negative ? "-" : "";
        for (uint64_t i = 0; i < temp.length(); ++i) {

            out += temp.at(temp.length() - 1 - i);
        }

        return out;
    }

    std::string toBaseString(uint32_t base) {

        std::string symbols = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::string result;
        std::string x = this->abs().toBinString();
        if (x.at(0) == '-') {

            x.erase(x.cbegin());
        }
        for (;;) {
            if (x.empty()) {

                break;
            }
            uint64_t r = 0;
            std::string temp;
            for (uint32_t i = 0; i < x.length(); ++i) {

                r = (r * 2) + (x.at(i) - '0');
                if (r >= base) {

                    temp += "1";
                    r = r - base;
                } else {

                    temp += "0";
                }
            }

            result += symbols.at(r);

            std::string temp2;
            for (uint32_t i = temp.find_first_not_of('0'); i < temp.length(); ++i) {

                temp2 += temp.at(i);
            }
            x = temp2;
        }

        std::string temp = result;
        result = this->number.negative ? "-" : "";
        for (uint32_t i = 0; i < temp.length(); ++i) {

            result += temp.at(temp.length() - 1 - i);
        }

        return result;
    }

    bool isPrime() {

        /*
         *  Reference: https://en.wikipedia.org/wiki/Baillie–PSW_primality_test
         *
         *  n = Tested number
         *
         *  1. Do a few trial divisions
         *
         *  2. Perform "strong probable prime test"     https://en.wikipedia.org/wiki/Strong_pseudoprime
         *      if
         *      2^(n - 1) % n = 1 v -1
         *      then n might be a prime. Otherwise, n is definitely not a prime
         *
         *      Continue with halving the exponent and check if the formula holds true until the exponent is odd. Inclusively check the odd exponent
         *
         *  3. Find the first D in the sequence 5, −7, 9, −11, 13, −15, ... for which the Jacobi symbol (D/n) is −1. Set P = 1 and Q = (1 − D) / 4.
         *      implement https://en.wikipedia.org/wiki/Jacobi_symbol
         *
         *  4. Perform a strong Lucas probable prime test on n using parameters D, P, and Q. If n is not a strong Lucas probable prime, then n is composite. Otherwise, n is almost certainly prime.
         *      implement https://en.wikipedia.org/wiki/Lucas_pseudoprime#Strong_Lucas_pseudoprimes
         */

        BigNum n = *this;

        uint64_t primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 47, 53, 59, 67, 71, 83, 89, 101, 107, 109, 113, 127, 131, 137, 139, 149, 157, 167, 179, 181, 191, 197, 199, 211, 227, 233, 239, 251, 257, 263, 269, 281, 293, 307, 311, 317, 337, 347, 353, 359, 379, 389, 401, 409};

        if ((n == 0) || (n == 1)) {

            return false;
        }

        for (uint64_t p: primes) {

            if (n % p == 0) {

                return n == p;
            }
        }

        ////////////////////////////////////////////////////////////

        // Step 2:
        {
            BigNum nc = n - 1;

            // Find s and d, such that 2^s * d + 1 = n and d is odd
            BigNum s("0b1"), d;
            //s = s << nc.length(); // Uncomment
            for (;;) {

                if (nc % s == 0) {   // If n is divisible by a power of 2 and the resulting d would be odd

                    d = nc / s;
                    if (d % 2 == 1) {

                        break;
                    }

                }
                s = s << 1; // s = s >> 1;
            }

            BigNum x = BigNum("0b10").modPow(d, n);

            if (x != 1 && x != (n - 1)) {

                bool composite = true;
                for (BigNum i; i < s.length(); i = i + 1) {

                    x = (x * x) % n;
                    if (x == (n - 1)) {

                        composite = false;
                        break;
                    }
                }
                if (composite) {

                    return false;   // n is composite
                }
            }

            std::cout << "(" << n.toBaseString(10) << ") ";

            /*
            BigNum exponent = n - 1;


            BigNum a("0b10");
            for (;;) {

                if (a.modPow(exponent, n) != 1) {

                    return false;   // n is not a probable prime base 2
                }

                if (exponent % 2 == 1) {

                    break;
                }

                exponent = exponent / 2;
            }
            */
        }
        ////////////////////////////////////////////////////////////

        // Step 3:
        BigNum d("0b101");

        for (;;) {

            if (jacobiSymbol(d, n) == -1) {

                break;
            }

            bool negative = d.number.negative;
            d = d.abs() + 2;
            d.number.negative = !negative;
        }

        ////////////////////////////////////////////////////////////

        // Step 4:
        BigNum P("0b1");
        BigNum tempD = d - 1;
        tempD.number.negative = !d.number.negative;

        BigNum nc = n;
        nc = nc + 1;


        BigNum two("0b10");
        BigNum U("0b1");   // (U1 = 1)
        BigNum V("0b1");   // (V1 = P = 1)
        BigNum Q_prime = tempD / 4;
        BigNum Q = Q_prime;

        uint64_t length = nc.length();

        for (uint64_t i = 2; i <= length; ++i) {

            // Double the subscript
            BigNum U_prime = U, V_prime = V;

            U = (U_prime * V_prime) % n;

            V = V_prime.modPow(two, n) + (d * U_prime.modPow(two, n));
            V = V % 2 == 0 ? V : V + n;
            V = (V / 2) % n;

            //Q = (Q * Q) % n;

            // Check if the bit at position 'i' is a one
            if (((nc >> (length - i)).number.n.at(0) & 1) == 1) {

                // Raise the subscript by one
                U_prime = U, V_prime = V;

                U = U_prime + V_prime;
                U = U % 2 == 0 ? U : U + n;
                U = (U / 2) % n;

                V = (V_prime + (d * U_prime));
                V = V % 2 == 0 ? V : V + n;
                V = (V / 2) % n;

                //Q = (Q * Q_prime) % n;
            }
        }

        if (U % n == 0) {   // @fixme this is sometimes false, when it shouldn't e.g. when checking if 419 is prime

            return true;
        }

        // @todo consider implementing the other congruence relations

        return false;
    }

    char jacobiSymbol(BigNum a, BigNum n) {

        assert(n % 2 == 1 && n > 0);

        a = a % n;
        int8_t t = 1;
        while (a != 0) {

            while (a % 2 == 0) {

                a = a / 2;
                BigNum r = n % 8;
                if ((r == 3) || (r == 5)) {

                    t *= -1;
                }
            }
            BigNum temp = a;
            a = n;
            n = temp;
            if (((a % n) == 3) && ((n % 4) == 3)) {

                t *= -1;
            }
            a = a % n;
        }
        if (n == 1) {

            return t;
        }

        return 0;
    }

    BigNum toLength(uint64_t longs) {

        BigNum result;
        result.number.n.clear();    // Empty the number

        if (this->number.n.size() == longs) return *this;   // No work to do

        if (this->number.n.size() < longs) {    // Takes all entries of the previous number and appends leading zeros

            for (uint64_t i = 0; i < longs - this->number.n.size(); ++i) {

                result.number.n.push_back(0);
            }
            for (uint64_t i = 0; i < this->number.n.size(); ++i) {

                result.number.n.push_back(this->number.n[i]);
            }
        } else {    // Takes the most significant entries in the vector to form a new number

            for (uint64_t i = 0; i < longs; ++i) {

                result.number.n.push_back(this->number.n[this->number.n.size() - i - 1]);
            }
        }

        return result;
    }

    BigNum sqrt() {

        // @todo understand this
        // https://stackoverflow.com/questions/10866119/what-is-the-fastest-way-to-find-integer-square-root-using-bit-shifts

        BigNum n = *this;
        BigNum zero;
        BigNum res;
        BigNum bit;
        bit = "0b1";
        bit = bit << 638;

        // This loop seems to align the two numbers in a certain way, since it is always shifted two to the right
        while (bit > n) {

            bit = bit >> 2;
        }
        while (bit != zero) {

            if (n >= (res + bit)) {

                n = n - (res + bit);
                res = (res >> 1) + bit;
            } else {

                res = res >> 1;
            }
            bit = bit >> 2;
        }

        return res;
    }

    BigNum pow(BigNum n) {

        BigNum result;
        result = "0b1";
        BigNum null;
        null = "0b0";
        BigNum one;
        one = "0b1";

        while (n > null) {

            result = result * *this;
            n = n - one;
        }

        return result;
    }

    BigNum abs() const {

        BigNum a = *this;
        a.number.negative = false;
        return a;
    }

    BigNum modPow(BigNum e, BigNum m) {

        /*
        BigNum a = *this;
        a = a % m;
        BigNum result("0b1");

        while (e > 0) {

            if (e % 2 == 1) {

                result = (result * a) % m;
            }
            e = e / 2;
            a = (a * a) % m;
        }

        return result;
        */

        // @todo make this work for negative exponents

        BigNum result("0b1");
        BigNum a = *this;
        a = a % m;

        while (e > 0) {

            if (e.number.n.at(0) & 1) result = (result * a) % m;
            a = (a * a) % m;
            e = e >> 1;
        }

        return result;
    }

    BigNum &operator=(std::string s) {

        BigNum temp;
        temp.number.n.clear();
        unsigned int offset = 0;

        if (s.at(0) == '-') {

            temp.number.negative = true;
            offset = 1;
        }

        // @todo use switch case

        if (s.length() > 2 + offset && s.at(offset) == '0' && s.at(1 + offset) == 'x') { // Hex

            for (unsigned int i = 2 + offset; i < s.length(); ++i) {

                unsigned char v = 0;
                if (s.at(i) >= 'a') {

                    v = s.at(i) - 'a' + 10;

                } else if (s.at(i) >= 'A') {

                    v = s.at(i) - 'A' + 10;

                } else if (s.at(i) >= '0') {

                    v = s.at(i) - '0';
                }

                temp = temp << 4;
                temp.number.n.at(0) |= v;
            }

        } else if (s.length() > 2 + offset && s.at(offset) == '0' && s.at(1 + offset) == 'b') {  // Binary

            for (unsigned int i = 2 + offset; i < s.length(); ++i) {

                temp = temp << 1;
                temp.number.n.at(0) |= (s.at(i) - '0');
            }

        } else if (s.length() > 0) {    // Decimal

            // @todo implement

        } else {

            std::cout << "Invalid input" << std::endl;
        }

        *this = temp;
        return *this;
    }

    BigNum &operator=(uint64_t n) {

        this->number.n.clear();
        this->number.n.push_back(n);
        return *this;
    }

    BigNum operator+(BigNum n) const {

        uint operand1 = this->number;
        uint operand2 = n.number;
        BigNum result;

        if (operand1.negative == operand2.negative) {

            result.number.negative = operand1.negative;
        } else {

            result = this->abs() > n.abs() ? this->abs() - n.abs() : n.abs() - this->abs();
            result.number.negative = this->abs() > n.abs() ? this->number.negative : !this->number.negative;
            if (result == 0) {

                result.number.negative = false;
            }
            return result;
        }

        result.number.n.clear();
        bool carry = false;
        for (unsigned int i = 0; i < std::max(operand1.n.size(), operand2.n.size()); ++i) {

            uint64_t a = i < operand1.n.size() ? operand1.n.at(i) : 0;
            uint64_t b = i < operand2.n.size() ? operand2.n.at(i) : 0;
            uint64_t c = 0;

            uint64_t m = 1;
            for (unsigned char j = 0; j < 64; ++j) {

                bool temp1 = (m << j) & a;
                bool temp2 = (m << j) & b;

                uint64_t temp3 = (temp1 ^ temp2 ^ carry);

                c |= temp3 << j;

                carry = (temp1 && temp2) || (temp1 && carry) || (temp2 && carry); // @todo make this less ugly
            }

            result.number.n.push_back(c);
        }

        if (carry) {

            result.number.n.push_back(1);
        }


        result.fit();
        return result;
    }

    BigNum operator+(uint64_t n) const {

        BigNum a;
        a = n;
        return *this + a;
    }

    BigNum operator-(BigNum n) const {

        uint operand1 = this->number;
        uint operand2 = n.number;
        BigNum result;

        if (this->number.negative == n.number.negative) {

            operand1 = this->abs() < n.abs() ? n.number : this->number;
            operand2 = this->abs() < n.abs() ? this->number : n.number;
            result.number.negative = n > *this;
        } else if (!this->number.negative && n.number.negative) {

            return *this + n.abs();
        } else {

            result = this->abs() + n;
            result.number.negative = result != 0;
            return result;
        }

        result.number.n.clear();
        bool borrow = false;
        for (uint64_t i = 0; i < operand1.n.size(); ++i) {

            uint64_t a = operand1.n.at(i);
            uint64_t b = i < operand2.n.size() ? operand2.n.at(i) : 0;
            uint64_t c = 0;

            uint64_t m = 1;
            for (unsigned char j = 0; j < 64; ++j) {

                bool temp1 = (m << j) & a;
                bool temp2 = (m << j) & b;

                uint64_t temp3 = (temp1 ^ temp2 ^ borrow);

                c |= temp3 << j;

                borrow = (!temp1 && temp2) || (!temp1 && borrow) || (temp1 && temp2 && borrow);
            }

            result.number.n.push_back(c);
        }

        result.fit();
        return result;
    }

    BigNum operator-(uint64_t n) const {

        BigNum a;
        a = n;
        return *this - a;
    }

    BigNum operator*(BigNum n) const {

        BigNum a;
        a = *this;
        BigNum result;
        bool negative = this->number.negative != n.number.negative;
        a = a.abs();
        n = n.abs();

        for (uint64_t i = 0; i < a.number.n.size() * 64; ++i) {

            if (((a >> i).number.n.at(0) & 1) == 1) {

                BigNum temp = (n << i);
                result = result + temp;
            }
        }

        result.number.negative = negative;
        result.fit();
        return result;
    }

    BigNum operator*(uint64_t n) const {

        BigNum a;
        a = n;
        return *this * a;
    }

    BigNum operator/(BigNum n) const {

        BigNum a = this->abs();
        BigNum result;
        bool negative = this->number.negative != n.number.negative;

        if (*this == n) {

            //result = "0b1";
            //return result;
        }

        if (n.abs() > a) {

            result = "0b0";
            return result;
        }


        n = n.abs();

        uint64_t i;
        for (i = 0; i < a.number.n.size() * 64; ++i) {

            if ((a >> (a.number.n.size() * 64 - 1 - i)).number.n.at(0) & 1) {

                break;
            }
        }
        i = a.number.n.size() * 64 - 1 - i;

        uint64_t j;
        for (j = 0; j < n.number.n.size() * 64; ++j) {

            if ((n >> (n.number.n.size() * 64 - 1 - j)).number.n.at(0) & 1) {

                break;
            }
        }
        j = n.number.n.size() * 64 - 1 - j;

        unsigned short diff = i - j;
        n = n << diff;

        for (unsigned short k = 0; k < diff + 1; ++k) {

            result = result << 1;

            if (a >= n) {

                a = a - n;
                result.number.n.at(0) |= 1;
            }

            n = n >> 1;
        }

        result.number.negative = negative;
        result.fit();
        return result;
    }

    BigNum operator/(uint64_t n) const {

        BigNum a;
        a = n;
        return *this / a;
    }

    BigNum operator%(BigNum n) const {

        BigNum result;
        if (this->number.negative == n.number.negative) {

            result = this->abs() - ((this->abs() / n.abs()) * n.abs());
            result.number.negative = this->number.negative && result != 0;
        } else {

            result = n.abs() - (this->abs() - (this->abs() / n.abs() * n.abs()));
            result.number.negative = result != 0;
        }

        result.fit();
        return result;
    }

    BigNum operator%(uint64_t n) const {

        BigNum a;
        a = n;
        return *this % a;
    }

    bool operator>(BigNum n) const {


        BigNum a = *this;
        a.fit();
        n.fit();

        if (a.number.n.size() != n.number.n.size()) {

            return a.number.n.size() > n.number.n.size();
        }

        if (a.number.negative != n.number.negative) {

            return a.number.negative && !n.number.negative;
        }

        for (int64_t i = a.number.n.size() - 1; i >= 0; --i) {

            if (a.number.n.at(i) != n.number.n.at(i)) {

                if (a.number.n.at(i) > n.number.n.at(i)) {

                    return true;
                } else {

                    return false;
                }
            }
        }

        return false;
    }

    bool operator>(uint64_t n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() > 1) {

            return true;
        }

        if (a.number.negative != n < 0) {

            return !a.number.negative && n < 0;
        }

        return a.number.n.at(0) > n;
    }

    bool operator>=(BigNum n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() != n.number.n.size()) {

            return a.number.n.size() > n.number.n.size();
        }

        if (a.number.negative != n.number.negative) {

            return a.number.negative && !n.number.negative;
        }

        for (int64_t i = a.number.n.size() - 1; i >= 0; --i) {

            if (a.number.n.at(i) != n.number.n.at(i)) {

                if (a.number.n.at(i) > n.number.n.at(i)) {

                    return true;
                } else {

                    return false;
                }
            }
        }

        return true;
    }

    bool operator>=(uint64_t n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() > 1) {

            return true;
        }

        if (a.number.negative != n < 0) {

            return !a.number.negative && n < 0;
        }

        return a.number.n.at(0) >= n;
    }

    bool operator<(BigNum n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() != n.number.n.size()) {

            return a.number.n.size() < n.number.n.size();
        }

        if (a.number.negative != n.number.negative) {

            return a.number.negative && !n.number.negative;
        }

        for (int64_t i = a.number.n.size() - 1; i >= 0; --i) {

            if (a.number.n.at(i) != n.number.n.at(i)) {

                if (a.number.n.at(i) < n.number.n.at(i)) {

                    return true;
                } else {

                    return false;
                }
            }
        }

        return false;
    }

    bool operator<(uint64_t n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() > 1) {

            return false;
        }

        if (a.number.negative != n < 0) {

            return a.number.negative && n >= 0;
        }

        return a.number.n.at(0) < n;
    }

    bool operator<=(BigNum n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() != n.number.n.size()) {

            return a.number.n.size() < n.number.n.size();
        }

        if (a.number.negative != n.number.negative) {

            return a.number.negative && !n.number.negative;
        }

        for (int64_t i = a.number.n.size() - 1; i >= 0; --i) {

            if (a.number.n.at(i) != n.number.n.at(i)) {

                if (a.number.n.at(i) < n.number.n.at(i)) {

                    return true;
                } else {

                    return false;
                }
            }
        }

        return true;
    }

    bool operator==(BigNum n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() != n.number.n.size()) {

            return false;
        }

        if (a.number.negative != n.number.negative) {

            return false;
        }

        for (uint64_t i = 0; i < a.number.n.size(); ++i) {

            if (a.number.n.at(i) != n.number.n.at(i)) {

                return false;
            }
        }

        return true;
    }

    bool operator==(uint64_t n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() > 1) {

            return false;
        }

        if (a.number.negative != n < 0) {

            return false;
        }

        return a.number.n.at(0) == n;
    }

    bool operator!=(BigNum n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() != n.number.n.size()) {

            return true;
        }

        if (a.number.negative != n.number.negative) {

            return true;
        }

        for (uint64_t i = 0; i < a.number.n.size(); ++i) {

            if (a.number.n.at(i) != n.number.n.at(i)) {

                return true;
            }
        }

        return false;
    }

    bool operator!=(uint64_t n) const {

        BigNum a = *this;
        a.fit();

        if (a.number.n.size() > 1) {

            return true;
        }

        if (a.number.negative != n < 0) {

            return true;
        }

        return a.number.n.at(0) != n;
    }

    BigNum operator&(BigNum n) const {

        BigNum result = *this;

        for (uint64_t i = 0; i < std::min(result.number.n.size(), n.number.n.size()); ++i) {

            result.number.n.at(i) = result.number.n.at(i) & n.number.n.at(i);
        }

        result.fit();
        return result;
    }

    BigNum operator|(BigNum n) const {

        BigNum result = n.number.n.size() > this->number.n.size() ? n : *this;      // The operand with more entries is stored

        for (uint64_t i = 0; i < std::min(this->number.n.size(), n.number.n.size()); ++i) {        // Go through the entries, that the operands have in common

            result.number.n.at(i) = this->number.n.at(i) | n.number.n.at(i);       // Perform a regular OR-operation
        }

        result.fit();
        return result;
    }

    BigNum operator^(BigNum n) const {

        BigNum a = *this;
        BigNum result;
        result.number.n.erase(result.number.n.begin());
        for (uint64_t i = 0; i < std::min(n.number.n.size(), a.number.n.size()); ++i) {

            result.number.n.push_back(a.number.n[i] ^ n.number.n[i]);
        }

        for (uint64_t i = std::min(n.number.n.size(), a.number.n.size()); i < std::max(n.number.n.size(), a.number.n.size()); ++i) {

            result.number.n.push_back((i >= a.number.n.size() ? 0 : a.number.n[i]) ^ (i >= n.number.n.size() ? 0 : n.number.n[i]));
        }
        return result;
    }

    BigNum operator^(uint64_t n) const {

        BigNum a;
        a = n;
        return *this ^ a;
    }

    BigNum operator<<(unsigned short shifts) const {


        if (shifts == 0) {

            return *this;
        }

        BigNum result = *this;
        uint64_t mask = UINT64_MAX;



        /*
         * Make sure to have enough space in the vector
         */
        result.allocate(shifts / 64 + 1);


        /*
         * The following part is for shifting multiples of 64 i.e. a whole uint64_t for making the process faster.
         */
        if (shifts >= 64) {

            unsigned char segmentShifts = shifts / 64;
            for (int64_t i = result.number.n.size() - 1; i >= 0; --i) {

                result.number.n[i] = i - segmentShifts >= 0 ? result.number.n[i - segmentShifts] : 0;
            }
        }



        /*
         * For shifting the remainder of shifts % 64, i.e. the bits that were not shifted by the previous operations
         */
        shifts %= 64;
        if (shifts != 0) {
            uint64_t carry = 0;
            for (int64_t i = 0; i < result.number.n.size(); ++i) {

                uint64_t a = result.number.n.at(i);
                result.number.n.at(i) = (result.number.n.at(i) << shifts) | carry;
                carry = (mask & a) >> (64 - shifts);
            }
        }


        result.fit();
        return result;
    }

    BigNum operator>>(unsigned short shifts) const {

        /*
         * Return the number, if no shifts are performed;
         */
        if (shifts == 0) {

            return *this;
        }

        BigNum result = *this;
        uint64_t mask = UINT64_MAX;



        /*
         * The following part is for shifting multiples of 64 i.e. a whole uint64_t for making the process faster.
         */
        if (shifts >= 64) {

            unsigned char segmentShifts = shifts / 64;
            for (uint64_t i = 0; i < result.number.n.size(); ++i) {

                result.number.n[i] = i + segmentShifts < result.number.n.size() ? result.number.n[i + segmentShifts] : 0;
            }
        }
        result.fit();



        /*
         * For shifting the remainder of shifts % 64, i.e. the bits that were not shifted by the previous operations
         */
        shifts %= 64;
        if (shifts != 0) {
            uint64_t carry = 0;
            for (int64_t i = result.number.n.size() - 1; i >= 0; --i) {

                uint64_t a = result.number.n.at(i);
                result.number.n.at(i) = (result.number.n.at(i) >> shifts) | carry;
                carry = (mask & a) << (64 - shifts);
            }
        }


        result.fit();
        return result;
    }

    void fit() {

        while (!this->number.n.empty()) {

            if (this->number.n.back() != 0) {

                break;
            }
            this->number.n.pop_back();
        }

        if (this->number.n.empty()) {

            this->number.n.push_back(0);
        }

        this->number.n.shrink_to_fit(); // @todo determine if this is unnecessary
    }

    void allocate(uint64_t n) {

        if (n == 0) {
            return;
        }

        this->number.n.reserve(this->number.n.size() + n);
        for (uint64_t i = 0; i < n; ++i) {

            this->number.n.push_back(0);
        }

        this->number.n.shrink_to_fit(); // @todo determine if this is unnecessary
    }
};