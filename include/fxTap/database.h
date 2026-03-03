#pragma once

#include <stddef.h>
#include <fxTap/beatmap.h>

typedef struct FXT_Database_Record
{
	char *Path;
	FXT_Grades *LastGrades; // nullptr: Player hasn't played this

	// Same as beatmap's but without its notes
	char *Title;
	char *Artist;
	double OverallDifficulty;
	uint8_t ColumnCount;
	uint16_t *ColumnSize;
} FXT_Database_Record;

typedef struct FXT_Database
{
	size_t Size;
	FXT_Database_Record *Records[];
} FXT_Database;

typedef enum FXT_DatabaseError
{
	FXT_DatabaseError_OK = 0,
} FXT_DatabaseError;

[[nodiscard]]
FXT_DatabaseError FXT_Database_Load(FXT_Database *dst);

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
