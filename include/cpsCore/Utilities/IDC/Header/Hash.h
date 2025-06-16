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

    for (std::size_t i = 0; i < str.length(); ++i)
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

    for (std::size_t i = 0; i < str.length(); ++i)
    {
        uint8_t value = str[i];
        hash = hash ^ value;
        hash *= prime;
    }

    return hash;
} //hash_64_fnv1a

struct Hash
{
    Hash(): hashValue(0)
    {
    }

    Hash(const std::string& string):
        hashValue(hash_32_fnv1a(string))
    {
    }

    Hash(const char* string):
        hashValue(hash_32_fnv1a(std::string(string)))
    {
    }

    auto operator<(const Hash& other) const
    {
        return hashValue < other.hashValue;
    }

    bool
    operator==(const Hash& other) const
    {
        return hashValue == other.hashValue;
    }

    explicit operator uint32_t() const
    {
        return hashValue;
    }

    explicit operator int() const
    {
        return static_cast<int>(hashValue);
    }

    uint32_t hashValue;
};

namespace dp
{
    template <typename Archive, typename>
    void
    serialize(Archive& ar, Hash& h)
    {
        ar & h.hashValue;
    }
}

#endif //HASHINGHEADER_H
