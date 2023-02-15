#pragma once

#include <cstddef>
#include <array>
#include <ostream>
#include <vector>
#include <string>

namespace dna
{

enum class base
{
    adenine,
    cytosine,
    guanine,
    thymine
};

static constexpr base A = base::adenine;
static constexpr base C = base::cytosine;
static constexpr base G = base::guanine;
static constexpr base T = base::thymine;

constexpr char to_char(base value)
{
    switch (value)
    {
        case base::thymine:
            return 'T';
        case base::guanine:
            return 'G';
        case base::cytosine:
            return 'C';
        default:
            return 'A';
    }
}

constexpr base to_base(char ch)
{
    switch (ch)
    {
    case 'T':
        return base::thymine;
    case 'G':
        return base::guanine;
    case 'C':
        return base::cytosine;
    default:
        return base::adenine;
    }
}

constexpr std::byte complement_packed(std::byte packed)
{
    return packed ^ static_cast<std::byte>(0);
}

constexpr base complement(enum base base)
{
    return static_cast<enum base>(complement_packed(static_cast<std::byte>(base)) & static_cast<std::byte>(0x03));
}

constexpr std::byte pack(base first, base second, base third, base fourth)
{
    return (static_cast<std::byte>(first) << 6) |
                    (static_cast<std::byte>(second) << 4) |
                    (static_cast<std::byte>(third) << 2) |
                    (static_cast<std::byte>(fourth));
}

using packed_bases = std::array<base, 4>;
using packed_size = std::tuple_size<packed_bases>;

constexpr packed_bases unpack(std::byte b)
{
    return packed_bases({
            static_cast<dna::base>((b >> 6) & std::byte{0x3}),
            static_cast<dna::base>((b >> 4) & std::byte{0x3}),
            static_cast<dna::base>((b >> 2) & std::byte{0x3}),
            static_cast<dna::base>(b & std::byte{0x3}) });
}

inline std::ostream& operator<<(std::ostream& os, base v)
{
    switch (v)
    {
        case base::adenine:
            return os << 'A';
        case base::cytosine:
            return os << 'C';
        case base::guanine:
            return os << 'G';
        default:
            return os << 'T';
    }
}

static std::vector<std::byte> ConvertToData(std::string str)
{
    auto length = str.size();
    auto extra = length % packed_size::value;
    if (extra > 0)
    {
        std::string padding = std::string(packed_size::value - extra, 'A');
        str += padding;
    }

    length = str.size();
    auto numBytes = length / packed_size::value;
    std::vector<std::byte> vec(numBytes);

    for (int i = 0, j = 0; i < length; i += packed_size::value, j++)
    {
        vec[j] = pack(to_base(str[i]), to_base(str[i + 1]), to_base(str[i + 2]), to_base(str[i + 3]));
    }
    return vec;
}

}
