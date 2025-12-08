#include <fxTap/config.h>
#include <fxTap/beatmap.h>
#include <fxTap/endian-utility.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config *Config_New_LoadFromDisk(ConfigError *error)
{
	Config *config = malloc(sizeof(Config));

	if (config == NULL)
	{
		*error = ConfigError_MallocFailed;
		return NULL;
	}

	FILE *file = fopen(FXTAP_CONFIG_PATH, "rb");

	if (file == NULL) // Create a new config if not exists
	{
		file = fopen(FXTAP_CONFIG_PATH, "wb");

		if (file == NULL)
		{
			free(config);
			*error = ConfigError_CannotCreateFile;
			return NULL;
		}

		Config_SetDefault(config);

		if (EnvironmentIsBigEndian())
			Config_ChangeEndian(config);

		if (1 > fwrite(config, sizeof(Config), 1, file))
		{
			free(config);
			fclose(file);
			*error = ConfigError_CannotWriteFile;
			return NULL;
		}

		if (fclose(file) != 0)
		{
			free(config);
			*error = ConfigError_CannotCloseFile;
			return NULL;
		}
	} else // Load the config if exists
	{
		if (1 > fread(config, sizeof(Config), 1, file))
		{
			free(config);
			fclose(file);
			*error = ConfigError_CannotReadFile;
			return NULL;
		}

		if (fclose(file) != 0)
		{
			free(config);
			*error = ConfigError_CannotCloseFile;
			return NULL;
		}

		if (EnvironmentIsBigEndian())
			Config_ChangeEndian(config);
	}

	*error = ConfigError_OK;
	return config;
}

void Config_SetDefault(Config *config)
{
	memcpy(config->Version, FXTAP_CONFIG_VERSION, CONFIG_VERSION_LEN);

	for (char i = 0; i < MAX_COLUMN_COUNT; i += 1)
		config->PhysicalKeyOfFxTapKey[i] = 0;

	config->NotesFallingTime = 1000;
	config->KeyMappingStyle = KeyMappingStyle_DJMAX;
	config->Language = EN_US;
}

ConfigError Config_SaveToDisk(const Config *config)
{
	FILE *file = fopen(FXTAP_CONFIG_PATH, "wb");

	if (file == NULL)
		return ConfigError_CannotOpenFile;

	if (EnvironmentIsBigEndian())
	{
		Config configLittleEndian;

		memcpy(&configLittleEndian, config, sizeof(Config));
		Config_ChangeEndian(&configLittleEndian);

		if (1 > fwrite(&configLittleEndian, sizeof(Config), 1, file))
		{
			fclose(file);
			return ConfigError_CannotWriteFile;
		}
	} else
	{
		if (1 > fwrite(config, sizeof(Config), 1, file))
		{
			fclose(file);
			return ConfigError_CannotWriteFile;
		}
	}

	if (fclose(file) != 0)
		return ConfigError_CannotCloseFile;

	return ConfigError_OK;
}

void Config_ChangeEndian(Config *config)
{
	SwapBytes(config->NotesFallingTime);
}
