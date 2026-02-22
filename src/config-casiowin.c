#ifdef FXTAP_CORE_USE_CASIOWIN

#include <fxTap/bfile-interface.h>
#include <fxTap/config.h>

static const uint16_t *const ConfigPath = u"\\\\fls0\\FXTAP.cfg";

FXT_Config_Error FXT_Config_Load_BFile(FXT_Config *const dst)
{
	FXT_Config config = FXT_Config_Default;

	int file = BFile_Open(ConfigPath, BFile_ReadOnly);

	if (file < 0)
	{
		int size = sizeof(FXT_Config);

		if (BFile_Create(ConfigPath, BFile_File, &size))
			return FXT_ConfigError_CannotCreateFile;

		file = BFile_Open(ConfigPath, BFile_WriteOnly);

		if (file < 0)
			return FXT_ConfigError_CannotOpenNewFileJustCreated;

		if (sizeof(FXT_Config) > BFile_Write(file, &config, sizeof(FXT_Config)))
		{
			BFile_Close(file);
			return FXT_ConfigError_CannotWriteFile;
		}

		goto win;
	}

	if (sizeof(FXT_Config) > BFile_Read(file, &config, sizeof(FXT_Config), -1))
	{
		BFile_Close(file);
		return FXT_ConfigError_CannotReadFile;
	}

win:
	BFile_Close(file);
	*dst = config;
	return 0;
}

FXT_Config_Error FXT_Config_Save_BFile(const FXT_Config config)
{
	BFile_Remove(ConfigPath);

	int size = sizeof(FXT_Config);

	if (BFile_Create(ConfigPath, BFile_File, &size))
		return FXT_ConfigError_CannotCreateFile;

	const int file = BFile_Open(ConfigPath, BFile_WriteOnly);

	if (file < 0)
		return FXT_ConfigError_CannotOpenFile;

	if (sizeof(FXT_Config) > BFile_Write(file, &config, sizeof(FXT_Config)))
	{
		BFile_Close(file);
		return FXT_ConfigError_CannotWriteFile;
	}

	BFile_Close(file);
	return 0;
}

#endif
