#include <stdio.h>
#include <fxTap/config.h>

const auto ConfigPath = "fxTap.cfg";

// ReSharper disable once CppUseAuto
const FXT_Config FXT_Config_Default = (FXT_Config){
	.NotesFallingTime = 1000,
	.KeyMapStyle = FXT_KeyMapStyle_DJMAX,
	.Language = FXT_Language_EnUs,
	.PhysicalKeyOfFxTapKey = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

FXT_Config_Error FXT_Config_Load(FXT_Config *dst)
{
	FXT_Config_Error error = FXT_ConfigError_OK;
	FXT_Config config = FXT_Config_Default;

	FILE *file = fopen(ConfigPath, "rb");

	if (file == nullptr)
	{
		file = fopen(ConfigPath, "wb");

		if (file == nullptr)
		{
			error = FXT_ConfigError_CannotCreateFile;
			goto fail;
		}

		if (!fwrite(&config, sizeof(FXT_Config), 1, file))
		{
			error = FXT_ConfigError_CannotWriteFile;
			goto fail;
		}

		goto win;
	}

	if (!fread(&config, sizeof(FXT_Config), 1, file))
	{
		error = FXT_ConfigError_CannotReadFile;
		goto fail;
	}

win:
	fclose(file);
	*dst = config;
	return 0;

fail:
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
