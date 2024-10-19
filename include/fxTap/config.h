#ifndef FXTAP_CORE_CONFIG_H
#define FXTAP_CORE_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <fxTap/beatmap.h>

#define FXTAP_CONFIG_PATH "fxTap.cfg"
#define FXTAP_CONFIG_VERSION "fxTap-config-v01"
#define CONFIG_VERSION_LEN 16
#define MAX_KEY_COUNT 10

typedef enum KeyMappingStyle
{
    KeyMappingStyle_DJMAX,
    KeyMappingStyle_BeatmaniaIIDX,
    KeyMappingStyle_Coop,
} KeyMappingStyle;

typedef enum Language
{
    EN_US, ZH_CN,
} Language;

typedef struct Config
{
    int8_t Version[CONFIG_VERSION_LEN];
    uint8_t PhysicalKeyOfFxTapKey[MAX_COLUMN_COUNT];
    int16_t NotesFallingTime;
    KeyMappingStyle KeyMappingStyle;
    Language Language;
} Config;

typedef enum ConfigError
{
    ConfigError_OK = 0,
    ConfigError_MallocFailed,
    ConfigError_CannotCreateFile,
    ConfigError_CannotWriteFile,
    ConfigError_CannotCloseFile,
    ConfigError_CannotReadFile,
    ConfigError_CannotOpenFile,
    ConfigError_CannotOpenNewFileJustCreated,
} ConfigError;

__attribute__ ((malloc))
Config *Config_New_LoadFromDisk(ConfigError *error);

void Config_SetDefault(Config *config);

ConfigError Config_SaveToDisk(Config *config);

void Config_ChangeEndian(Config *config);

#endif //FXTAP_CORE_CONFIG_H
