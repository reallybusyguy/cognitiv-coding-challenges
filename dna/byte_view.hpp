#pragma once

#include <cstddef>
#include "binary_traits.hpp"

using byte_view = std::basic_string_view<std::byte, detail::binary_traits>;
