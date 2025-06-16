//
// Created by Mirco Theile on 10/6/25.
//

#ifndef HAMMING74_H
#define HAMMING74_H
#include <cstdint>
#include <array>
#include <vector>
#include <string>

namespace Hamming74
{
    // Encode 4-bit data into 7-bit codeword
    uint8_t
    encode(uint8_t data4);

    // Decode 7-bit codeword, correct 1-bit errors, return 4-bit data
    uint8_t
    decode(uint8_t codeword, bool& corrected);

    std::string
    encode(const std::string& data);
    std::string
    decode(const std::string& codewords, std::vector<bool>& corrected);

    // Extract i-th bit (0 = LSB)
    inline bool
    getBit(uint8_t val, int i)
    {
        return (val >> i) & 1;
    }

    // Set i-th bit (0 = LSB)
    inline void
    setBit(uint8_t& val, int i, bool bit)
    {
        val = (val & ~(1 << i)) | (bit << i);
    }
} // namespace Hamming74
#endif //HAMMING74_H
