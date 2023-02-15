#include "DNA_Stream.hpp"

namespace dna
{
    DNA_Stream::DNA_Stream() : offset_(0), chunksize_(1) {
    }

    DNA_Stream::DNA_Stream(const DNA_Stream& other) {
        data_ = other.data_;
        chunksize_ = other.chunksize_;
        offset_ = other.offset_.load();
    }

    DNA_Stream::DNA_Stream(DNA_Stream&& other) noexcept {
        data_ = std::move(other.data_);
        chunksize_ = other.chunksize_;
        offset_ = other.offset_.exchange(0);
    }

    DNA_Stream::DNA_Stream(std::vector<std::byte> data, std::size_t chunksize) {
        data_ = std::move(data);
        chunksize_ = chunksize;
        offset_ = 0;
    }

    DNA_Stream& DNA_Stream::operator=(const DNA_Stream& other) {
        if (&other != this) {
            data_ = other.data_;
            chunksize_ = other.chunksize_;
            offset_ = other.offset_.load();
        }
        return *this;
    }

    DNA_Stream& DNA_Stream::operator=(DNA_Stream&& other) noexcept {
        if (&other != this) {
            data_ = std::move(other.data_);
            chunksize_ = other.chunksize_;
            offset_ = other.offset_.exchange(0);
        }
        return *this;
    }

    // Set the offset position in the stream to the given offset position.
    // NB: This offset is an absolute position, not relative to the current position.
    void DNA_Stream::seek(size_t offset) {
        offset_.store(std::min(std::max(offset, size_t(0)), data_.size()));
    }

    size_t DNA_Stream::size() const {
        return data_.size();
    }

    sequence_buffer<byte_view> DNA_Stream::read() {
        auto offset = offset_.load(std::memory_order_consume);
        while (true)
        {
            auto len = std::min(chunksize_, data_.size() - offset_);
            if (len == 0)
                return byte_view(nullptr, 0);

            if (offset_.compare_exchange_weak(offset, offset + len, std::memory_order_release))
                return byte_view(data_.data() + offset, len);
        }
    }

    bool DNA_Stream::atEnd() const
    {
        return offset_ == data_.size();
    }

    void DNA_Stream::advanceToEnd()
    {
        seek(size());
    }
}

