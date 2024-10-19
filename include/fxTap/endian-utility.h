#ifndef FXTAP_CORE_TEST_ENDIAN_UTILITY_H
#define FXTAP_CORE_TEST_ENDIAN_UTILITY_H

#include <stddef.h>
#include <stdbool.h>

void SwapBytesOfSize(void *bytes, size_t bytesCount);

#define SwapBytes(x) SwapBytesOfSize(&x, sizeof(x))

bool EnvironmentIsBigEndian();

#endif //FXTAP_CORE_TEST_ENDIAN_UTILITY_H
