#pragma once

#include <cstdlib>

#ifdef _DEBUG
#   define APP_DEBUG
#endif

#if defined(APP_DEBUG)
#   define ASSERT(expr) if (!(expr)) std::abort();
#else
#   define ASSERT(expr)
#endif
