#include <stdio.h>
#include <fxTap/config.h>

static const auto ConfigPath = "fxTap.cfg";


const FXT_Config FXT_Config_Default = {
	.NotesFallingTime = 1000,
	.KeyMapStyle = FXT_KeyMapStyle_DJMAX,
	.Language = FXT_Language_EnUs,
	.PhysicalKeyOfFxTapKey = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

FXT_Config_Error FXT_Config_Load(FXT_Config *dst)
{
	FXT_Config_Error error = 0;
	FXT_Config config = FXT_Config_Default;

	FILE *file = fopen(ConfigPath, "rb");

	if (file == nullptr)
	{
		file = fopen(ConfigPath, "wb");

		if (file == nullptr)
		{
			error = FXT_ConfigError_CannotCreateFile;
			goto done;
		}

		if (!fwrite(&config, sizeof(FXT_Config), 1, file))
		{
			error = FXT_ConfigError_CannotWriteFile;
			goto done;
		}
	}
	else if (!fread(&config, sizeof(FXT_Config), 1, file))
	{
		error = FXT_ConfigError_CannotReadFile;
		goto done;
	}

	*dst = config;

done:
	if (file != nullptr) fclose(file);
	return error;
}

FXT_Config_Error FXT_Config_Save(const FXT_Config config)
{
	FILE *file = fopen(ConfigPath, "wb");

	if (file == nullptr)
		return FXT_ConfigError_CannotOpenFile;

	if (!fwrite(&config, sizeof(FXT_Config), 1, file))
	{
		fclose(file);
		return FXT_ConfigError_CannotWriteFile;
	}

	fclose(file);
	return 0;
}

bool FXT_Config_Equal(const FXT_Config *a, const FXT_Config *b)
{
	for (int i = 0; i < FXT_MaxColumnCount; i += 1)
		if (a->PhysicalKeyOfFxTapKey[i] != b->PhysicalKeyOfFxTapKey[i])
			return false;

	return a->NotesFallingTime == b->NotesFallingTime
	       && a->KeyMapStyle == b->KeyMapStyle
	       && a->Language == b->Language;
}
