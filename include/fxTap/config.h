#pragma once

#include <stdint.h>
#include <fxTap/beatmap.h>

#define FXT_MaxKeyCount 10

typedef enum FXT_KeyMapStyle
{
	FXT_KeyMapStyle_DJMAX,
	FXT_KeyMapStyle_BeatmaniaIIDX,
	FXT_KeyMapStyle_CoOp,
} FXT_KeyMapStyle;

typedef enum FXT_Language
{
	FXT_Language_EnUs,
	FXT_Language_ZhCn,
} FXT_Language;

typedef struct config
{
	int16_t NotesFallingTime;
	uint8_t PhysicalKeyOfFxTapKey[FXT_MaxColumnCount];
	FXT_KeyMapStyle KeyMapStyle;
	FXT_Language Language;
} FXT_Config;

typedef enum FXT_ConfigError
{
	FXT_ConfigError_OK = 0,
	FXT_ConfigError_MallocFailed,
	FXT_ConfigError_CannotCreateFile,
	FXT_ConfigError_CannotWriteFile,
	FXT_ConfigError_CannotReadFile,
	FXT_ConfigError_CannotOpenFile,
	FXT_ConfigError_CannotOpenNewFileJustCreated,
} FXT_Config_Error;

extern const FXT_Config FXT_Config_Default;

[[nodiscard]]
FXT_Config_Error FXT_Config_Load(FXT_Config *dst);

[[nodiscard]]
FXT_Config_Error FXT_Config_Save(FXT_Config config);

#ifdef FXTAP_CORE_USE_CASIOWIN

[[nodiscard]]
FXT_Config_Error FXT_Config_Load_BFile(FXT_Config *dst);

[[nodiscard]]
FXT_Config_Error FXT_Config_Save_BFile(FXT_Config config);

#endif
