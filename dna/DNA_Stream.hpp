#pragma once

#include <cstddef>
#include <string_view>
#include <vector>
#include <atomic>
#include "sequence_buffer.hpp"
#include "byte_view.hpp"

namespace dna
{
    class DNA_Stream
    {
        std::vector<std::byte> data_;
        std::size_t chunksize_;
        std::atomic<size_t> offset_;
    public:

        DNA_Stream();
        DNA_Stream(const DNA_Stream& other);
        DNA_Stream(DNA_Stream&& other) noexcept;
        DNA_Stream(std::vector<std::byte> data, std::size_t chunksize = 512);

        DNA_Stream& operator=(const DNA_Stream& other);
        DNA_Stream& operator=(DNA_Stream&& other) noexcept;

        void seek(size_t offset);
        size_t size() const;
        sequence_buffer<byte_view> read();

        bool atEnd() const;
        void advanceToEnd();
    };
}
