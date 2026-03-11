#ifdef FXTAP_CORE_USE_CASIOWIN

#include <fxTap/bfile-interface.h>
#include <fxTap/config.h>

static const uint16_t *const ConfigPath = u"\\\\fls0\\fxTap.cfg";

FXT_ConfigError FXT_Config_Load_BFile(FXT_Config *const dst)
{
	FXT_ConfigError error = 0;
	FXT_Config config = FXT_Config_Default;

	int file = BFile_Open(ConfigPath, BFile_ReadOnly);

	if (file < 0)
	{
		int size = sizeof(FXT_Config);

		if (BFile_Create(ConfigPath, BFile_File, &size))
		{
			error = FXT_ConfigError_CannotCreateFile;
			goto done;
		}

		file = BFile_Open(ConfigPath, BFile_WriteOnly);

		if (file < 0)
		{
			error = FXT_ConfigError_CannotOpenNewFileJustCreated;
			goto done;
		}

		if (sizeof(FXT_Config) > BFile_Write(file, &config, sizeof(FXT_Config)))
		{
			error = FXT_ConfigError_CannotWriteFile;
			goto done;
		}
	}
	else if (sizeof(FXT_Config) > BFile_Read(file, &config, sizeof(FXT_Config), -1))
	{
		error = FXT_ConfigError_CannotReadFile;
		goto done;
	}

	*dst = config;

done:
	if (file >= 0) BFile_Close(file);
	return error;
}

FXT_ConfigError FXT_Config_Save_BFile(const FXT_Config *config)
{
	BFile_Remove(ConfigPath);

	int size = sizeof(FXT_Config);

	if (BFile_Create(ConfigPath, BFile_File, &size))
		return FXT_ConfigError_CannotCreateFile;

	const int file = BFile_Open(ConfigPath, BFile_WriteOnly);

	if (file < 0)
		return FXT_ConfigError_CannotOpenFile;

	if (sizeof(FXT_Config) > BFile_Write(file, config, sizeof(FXT_Config)))
	{
		BFile_Close(file);
		return FXT_ConfigError_CannotWriteFile;
	}

	BFile_Close(file);
	return 0;
}

#endif
