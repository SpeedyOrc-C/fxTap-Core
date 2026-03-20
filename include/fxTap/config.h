#pragma once

#include <stdint.h>
#include <fxTap/beatmap.h>

static constexpr size_t FXT_MaxKeyCount = 10;

typedef enum FXT_KeyMapStyle
{
	FXT_KeyMapStyle_DJMAX,
	FXT_KeyMapStyle_Beatmania,
	FXT_KeyMapStyle_CoOp,
} FXT_KeyMapStyle;

typedef enum FXT_Language
{
	FXT_Language_EnUs = 0,
	FXT_Language_ZhCn = 1,
} FXT_Language;

typedef struct FXT_Config
{
	uint8_t PhysicalKeyOfFxTapKey[FXT_MaxColumnCount];
	int16_t NotesFallingTime;
	uint8_t ColumnWidth;
	uint8_t TapNoteHeight;
	uint8_t CustomOverallDifficulty10;
	FXT_Language Language:8;
	FXT_KeyMapStyle KeyMapStyle:8;
	bool OverrideDefaultOverDifficulty:1;
} FXT_Config;

bool FXT_Config_Equal(const FXT_Config *a, const FXT_Config *b);

typedef enum FXT_ConfigError
{
	FXT_ConfigError_OK = 0,
	FXT_ConfigError_MallocFailed,
	FXT_ConfigError_CannotCreateFile,
	FXT_ConfigError_CannotWriteFile,
	FXT_ConfigError_CannotReadFile,
	FXT_ConfigError_CannotOpenFile,
	FXT_ConfigError_CannotOpenNewFileJustCreated,
} FXT_ConfigError;

extern const FXT_Config FXT_Config_Default;

[[nodiscard]]
FXT_ConfigError FXT_Config_Load(FXT_Config *dst);

[[nodiscard]]
FXT_ConfigError FXT_Config_Save(const FXT_Config *config);

#ifdef FXTAP_CORE_USE_GINT

[[nodiscard]]
FXT_ConfigError FXT_Config_Load_BFile(FXT_Config *dst);

[[nodiscard]]
FXT_ConfigError FXT_Config_Save_BFile(const FXT_Config *config);

#endif
