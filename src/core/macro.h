#pragma once

#ifdef _MSC_VER
#define malloc32(ptr, size) ptr = _aligned_malloc(size, 32)
#define free32(ptr) _aligned_free(ptr)
#elif defined(__GNUC__)
#define malloc32(ptr, size) posix_memalign((void **)&ptr, 32, size)
#define free32(ptr) free(ptr)
#else
#error "Unsupported compiler"
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif