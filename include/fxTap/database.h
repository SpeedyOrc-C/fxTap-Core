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
	double OverallDifficulty;
	uint8_t ColumnCount;
	uint16_t *ColumnSize;
} FXT_DatabaseRecord;

static constexpr FXT_DatabaseRecord FXT_DatabaseRecord_Null = {};

typedef struct FXT_Database
{
	char *key;
	FXT_DatabaseRecord value;
} *FXT_Database;

typedef enum FXT_DatabaseError
{
	FXT_DatabaseError_OK = 0,
	FXT_DatabaseError_MallocFailed,
	FXT_DatabaseError_CannotOpenDir,
	FXT_DatabaseError_CannotStartSavingGrades,
	FXT_DatabaseError_CannotSaveGrades,
} FXT_DatabaseError;

void FXT_Database_Init(FXT_Database *dst);

[[nodiscard]]
FXT_DatabaseError FXT_Database_Save(const FXT_Database *database);

void FXT_Database_FreeInner(FXT_Database *database);

void FXT_Database_Free(FXT_Database *database);

// If there's no grades, save. If new grades are higher, replace existing.
[[nodiscard]]
FXT_DatabaseError FXT_Database_UpdateGrades(FXT_Database *database, const char *path, const FXT_Grades *grades);

// Add new songs, remove deleted songs, and updating existing song metadata.
[[nodiscard]]
FXT_DatabaseError FXT_Database_SyncFromFileSystem(FXT_Database *database);

bool FXT_DatabaseRecord_IsNull(FXT_DatabaseRecord record);

int FXT_Database_Compare(const struct FXT_Database *a, const struct FXT_Database *b);

int FXT_Database_Compare_Reverse(const struct FXT_Database *a, const struct FXT_Database *b);

int FXT_Database_Compare_Void(const void *a, const void *b);

int FXT_Database_Compare_Reverse_Void(const void *a, const void *b);

[[nodiscard]]
FXT_DatabaseError FXT_SaveGradesAlongBeatmap(const char *beatmapPath, const FXT_Grades *grades);
