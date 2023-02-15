#pragma once

#include <cstddef>
#include <string_view>
#include <vector>
#include <atomic>
#include "sequence_buffer.hpp"
#include "../byte_view.hpp"


class fake_stream
{
	std::vector<std::byte> data_;
	std::size_t chunksize_;
	std::atomic<long> offset_;
public:

	fake_stream();
	fake_stream(const fake_stream& other);
	fake_stream(fake_stream&& other) noexcept;
	fake_stream(std::vector<std::byte> data, std::size_t chunksize);

	fake_stream& operator=(const fake_stream& other);
	fake_stream& operator=(fake_stream&& other) noexcept;

	void seek(long offset);
	long size() const;
	dna::sequence_buffer<byte_view> read();
};


