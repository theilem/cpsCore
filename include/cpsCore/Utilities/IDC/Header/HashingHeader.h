//
// Created by Mirco Theile on 29/5/25.
//

#ifndef HASHINGHEADER_H
#define HASHINGHEADER_H
#include <string>

inline uint32_t
hash_32_fnv1a(const std::string& str)
{
    uint32_t hash = 0x811c9dc5;
    uint32_t prime = 0x1000193;

    for (int i = 0; i < str.length(); ++i)
    {
        uint8_t value = str[i];
        hash = hash ^ value;
        hash *= prime;
    }

    return hash;
} //hash_32_fnv1a

inline uint64_t
hash_64_fnv1a(const std::string& str)
{
    uint64_t hash = 0xcbf29ce484222325;
    uint64_t prime = 0x100000001b3;

    for (int i = 0; i < str.length(); ++i)
    {
        uint8_t value = str[i];
        hash = hash ^ value;
        hash *= prime;
    }

    return hash;
} //hash_64_fnv1a

struct HashingHeader
{
    HashingHeader(): hashValue(0)
    {
    }

    explicit
    HashingHeader(const std::string& string):
        hashValue(hash_32_fnv1a(string))
    {
    }

    bool
    operator==(const std::string& string) const
    {
        return hashValue == hash_32_fnv1a(string);
    }

    uint32_t hashValue;
};

namespace dp
{
    template <typename Archive, typename>
    void
    serialize(Archive& ar, HashingHeader& h)
    {
        ar & h.hashValue;
    }
}

#endif //HASHINGHEADER_H
