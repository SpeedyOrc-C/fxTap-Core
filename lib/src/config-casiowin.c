#include <fxTap/config-casiowin.h>
#include <fxTap/endian-utility.h>
#include <fxTap/bfile-extern.h>
#include <stdlib.h>
#include <assert.h>

Config *Config_New_LoadFromDisk_BFile(ConfigError *error)
{
    Config *config = malloc(sizeof(Config));

    if (config == NULL)
    {
        *error = ConfigError_MallocFailed;
        return NULL;
    }

    const int bfile = BFile_Open(FXTAP_CONFIG_BFILE_PATH, BFile_ReadOnly);

    if (bfile < 0) // Create a new config if not exists
    {
        int size = sizeof(Config);

        assert((size & 1) == 0);

        if (BFile_Create(FXTAP_CONFIG_BFILE_PATH, BFile_File, &size))
        {
            free(config);
            *error = ConfigError_CannotCreateFile;
            return NULL;
        }

        const int newBfile = BFile_Open(FXTAP_CONFIG_BFILE_PATH, BFile_WriteOnly);

        if (newBfile < 0)
        {
            free(config);
            *error = ConfigError_CannotOpenNewFileJustCreated;
            return NULL;
        }

        Config_SetDefault(config);

        Config littleEndianConfig = *config;
        Config_ChangeEndian(&littleEndianConfig);

        if (sizeof(Config) > BFile_Write(newBfile, &littleEndianConfig, sizeof(Config)))
        {
            free(config);
            BFile_Close(newBfile);
            *error = ConfigError_CannotWriteFile;
            return NULL;
        }

        BFile_Close(newBfile);

    } else // Load the config if exists
    {
        if (sizeof(Config) > BFile_Read(bfile, config, sizeof(Config), -1))
        {
            free(config);
            BFile_Close(bfile);
            *error = ConfigError_CannotReadFile;
            return NULL;
        }

        Config_ChangeEndian(config);

        BFile_Close(bfile);
    }

    *error = ConfigError_OK;
    return config;
}

ConfigError Config_SaveToDisk_BFile(Config *config)
{
    BFile_Remove(FXTAP_CONFIG_BFILE_PATH);

    int size = sizeof(Config);
    assert((size & 1) == 0);

    if (BFile_Create(FXTAP_CONFIG_BFILE_PATH, BFile_File, &size))
        return ConfigError_CannotCreateFile;

    const int bfile = BFile_Open(FXTAP_CONFIG_BFILE_PATH, BFile_WriteOnly);

    if (bfile < 0)
        return ConfigError_CannotOpenFile;

    Config littleEndianConfig = *config;

    Config_ChangeEndian(&littleEndianConfig);

    if (sizeof(Config) > BFile_Write(bfile, &littleEndianConfig, sizeof(Config)))
    {
        BFile_Close(bfile);
        return ConfigError_CannotWriteFile;
    }

    BFile_Close(bfile);

    return ConfigError_OK;
}
