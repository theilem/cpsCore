//
// Created by Mirco Theile on 10/6/25.
//

#include "cpsCore/Utilities/DataPresentation/hamming74.h"

namespace Hamming74
{
uint8_t
encode(uint8_t data4)
{
    uint8_t d1 = getBit(data4, 0);
    uint8_t d2 = getBit(data4, 1);
    uint8_t d3 = getBit(data4, 2);
    uint8_t d4 = getBit(data4, 3);

    uint8_t p1 = d1 ^ d2 ^ d4;
    uint8_t p2 = d1 ^ d3 ^ d4;
    uint8_t p3 = d2 ^ d3 ^ d4;

    uint8_t codeword = 0;
    setBit(codeword, 0, p1);  // bit 1
    setBit(codeword, 1, p2);  // bit 2
    setBit(codeword, 2, d1);  // bit 3
    setBit(codeword, 3, p3);  // bit 4
    setBit(codeword, 4, d2);  // bit 5
    setBit(codeword, 5, d3);  // bit 6
    setBit(codeword, 6, d4);  // bit 7

    return codeword;
}

uint8_t
decode(uint8_t codeword, bool& corrected)
{
    bool p1 = getBit(codeword, 0);
    bool p2 = getBit(codeword, 1);
    bool d1 = getBit(codeword, 2);
    bool p3 = getBit(codeword, 3);
    bool d2 = getBit(codeword, 4);
    bool d3 = getBit(codeword, 5);
    bool d4 = getBit(codeword, 6);

    // Recalculate parities
    bool s1 = p1 ^ d1 ^ d2 ^ d4;  // P1: bits 1,3,5,7
    bool s2 = p2 ^ d1 ^ d3 ^ d4;  // P2: bits 2,3,6,7
    bool s3 = p3 ^ d2 ^ d3 ^ d4;  // P3: bits 4,5,6,7

    uint8_t syndrome = (s3 << 2) | (s2 << 1) | s1;

    corrected = false;
    if (syndrome != 0 && syndrome <= 7) {
        codeword ^= (1 << (syndrome - 1));  // Flip the erroneous bit
        corrected = true;
    }

    // Extract data bits (positions 3,5,6,7)
    uint8_t decoded = 0;
    setBit(decoded, 0, getBit(codeword, 2));
    setBit(decoded, 1, getBit(codeword, 4));
    setBit(decoded, 2, getBit(codeword, 5));
    setBit(decoded, 3, getBit(codeword, 6));
    return decoded;
}

std::string
encode(const std::string& data)
{
    std::string result;
    for (const auto& byte : data) {
        uint8_t lsb = (static_cast<uint8_t>(byte) & 0x0F);  // Lower 4 bits
        uint8_t msb = (static_cast<uint8_t>(byte) >> 4);  // Upper 4 bits
        result.push_back(static_cast<char>(encode(lsb)));  // Encode lower 4 bits
        result.push_back(static_cast<char>(encode(msb)));
    }
    return result;
}

std::string
decode(const std::string& codewords, std::vector<bool>& corrected)
{
    std::vector<uint8_t> result;
    corrected.clear();
    for (const auto& codeword : codewords) {
        bool isCorrected = false;
        auto decoded = decode(codeword, isCorrected);
        result.push_back(decoded);
        corrected.push_back(isCorrected);
    }
    // Merge the decoded bytes back into original 8-bit values
    std::string merged;
    for (size_t i = 0; i < result.size(); i += 2) {
        if (i + 1 < result.size()) {
            merged.push_back(static_cast<char>((result[i] & 0x0F) | ((result[i + 1] & 0x0F) << 4)));
        } else {
            merged.push_back(static_cast<char>(result[i] & 0x0F));  // Last byte might be incomplete
        }
    }
    return merged;
}

}
