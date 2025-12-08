#include <fxTap/endian-utility.h>
#include <stdint.h>
#include <assert.h>

void SwapBytesOfSize(void *bytes, size_t bytesCount)
{
	assert(bytesCount > 0);

	int8_t *i8Bytes = bytes;

	for (size_t lowIndex = 0, highIndex = bytesCount - 1; highIndex > lowIndex; lowIndex++, highIndex--)
	{
		int8_t tmp = i8Bytes[lowIndex];
		i8Bytes[lowIndex] = i8Bytes[highIndex];
		i8Bytes[highIndex] = tmp;
	}
}

bool EnvironmentIsBigEndian()
{
	const int16_t One = 1;
	return *(int8_t *) &One != 1;
}
