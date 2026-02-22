#pragma once

#include <stddef.h>

void SwapBytesOfSize(void *bytes, size_t bytesCount);

#define SwapBytes(x) SwapBytesOfSize(&x, sizeof(x))

bool EnvironmentIsBigEndian();
