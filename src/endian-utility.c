#include <assert.h>
#include <stdint.h>
#include <fxTap/endian-utility.h>

void SwapBytesOfSize(void *bytes, size_t bytesCount)
{
	assert(bytesCount > 0);

	int8_t *i8Bytes = bytes;

	for (size_t lowIndex = 0, highIndex = bytesCount - 1; highIndex > lowIndex; lowIndex++, highIndex--)
	{
		const int8_t tmp = i8Bytes[lowIndex];
		i8Bytes[lowIndex] = i8Bytes[highIndex];
		i8Bytes[highIndex] = tmp;
	}
}

bool EnvironmentIsBigEndian()
{
	constexpr int16_t One = 1;
	return *(int8_t *) &One != 1;
}
