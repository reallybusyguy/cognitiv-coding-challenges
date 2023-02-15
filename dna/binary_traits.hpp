#pragma once

namespace detail
{
	class binary_traits
	{
	public:
		using char_type = std::byte;

		static constexpr std::byte to_upper(std::byte c) noexcept {
			return c;
		}

		static std::size_t length(const std::byte* s) {
			throw std::logic_error("length of a binary string can't be determined. It must be explicitly supplied.");
		}
	};
}
