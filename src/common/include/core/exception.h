#pragma once

#include <stdexcept>

#define CHECK(expr, message) \
    if (!(expr)) \
        throw std::runtime_error{ (message) };
