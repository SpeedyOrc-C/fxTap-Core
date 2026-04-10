#pragma once

#include <fxTap/beatmap.h>

typedef struct FXT_DatabaseRecord
{
	struct
	{
		bool Exist;
		FXT_Grades Value;
	} LastGrades;

	// Same as beatmap's but without its notes
	char *Title;
	char *Artist;
	char *Version;
	double OverallDifficulty;
	uint8_t ColumnCount;
	uint16_t *ColumnSize;
} FXT_DatabaseRecord;

static constexpr FXT_DatabaseRecord FXT_DatabaseRecord_Null = {};

typedef struct
{
	char *key;
	FXT_DatabaseRecord value;
} *FXT_Database, FXT_DatabaseKV;

typedef enum FXT_DatabaseError
{
	FXT_DatabaseError_OK = 0,
	FXT_DatabaseError_MallocFailed,
	FXT_DatabaseError_CannotOpenDir,
	FXT_DatabaseError_CannotStartSavingGrades,
	FXT_DatabaseError_CannotSaveGrades,
	FXT_DatabaseError_BrokenBFileSearch,
	FXT_DatabaseError_View_MallocFailed,
} FXT_DatabaseError;

void FXT_Database_Init(FXT_Database *dst);

void FXT_Database_FreeInner(FXT_Database *database);

void FXT_Database_Free(FXT_Database *database);

// Find all beatmaps in the root directory, read their metadata, and put them into the database.
[[nodiscard]]
FXT_DatabaseError FXT_Database_SyncFromFileSystem(FXT_Database *database);

bool FXT_DatabaseRecord_IsNull(FXT_DatabaseRecord record);

[[nodiscard]]
FXT_DatabaseError FXT_SaveGradesAlongBeatmap(const char *beatmapPath, const FXT_Grades *grades);

#ifdef FXTAP_CORE_USE_GINT

[[nodiscard]]
FXT_DatabaseError FXT_Database_SyncFromFileSystem_BFile(FXT_Database *database);

[[nodiscard]]
FXT_DatabaseError FXT_SaveGradesAlongBeatmap_BFile(const char *beatmapPath, const FXT_Grades *grades);

#endif
