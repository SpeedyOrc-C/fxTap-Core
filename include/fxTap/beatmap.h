#pragma once

#include <stdint.h>
#include <fxTap/lib-config.h>

#define FXT_MaxColumnCount 16
#define BASE_TOLERANCE_PERFECT 16
#define BASE_TOLERANCE_GREAT 64
#define BASE_TOLERANCE_GOOD 97
#define BASE_TOLERANCE_OK 127
#define BASE_TOLERANCE_MEH 151
#define BASE_TOLERANCE_MISS 188

typedef struct Metadata
{
	char Header[16];
	char Title[32];
	char Artist[32];
	uint16_t SizeOfColumn[FXT_MaxColumnCount];
	double OverallDifficulty;
	int64_t Blank;
} Metadata;

typedef struct Tolerance
{
	int32_t Perfect;
	int32_t Great;
	int32_t Good;
	int32_t Ok;
	int32_t Meh;
	int32_t Miss;
} Tolerance, Grades;

typedef struct Note
{
	uint16_t AccumulatedStartTime;
	uint16_t Duration;
} Note;

typedef struct Beatmap
{
	Metadata Metadata;
	Tolerance Tolerance;
	// An 2D array: Notes[column_index][note_index].
	Note *Notes[FXT_MaxColumnCount];
} Beatmap;

typedef struct BeatmapFindEntry
{
	char *FileName;
	Metadata Metadata;
} BeatmapFindEntry;

typedef struct BeatmapFindEntries
{
	int Count;
	BeatmapFindEntry *Entries;
} BeatmapFindEntries;

typedef enum BeatmapError
{
	BeatmapError_OK,
	BeatmapError_MallocFailed,
	BeatmapError_FileNotFound,
	BeatmapError_CannotCloseFile,
	BeatmapError_ReadMetadataFailed,
	BeatmapError_ReadNotesFailed,
} BeatmapError;

typedef enum FindError
{
	FindError_OK,
	FindError_MallocFailed,
	FindError_FxtapFolderNotFound,
	FindError_BadFile,
} FindError;

// Load a beatmap from a file path.
// If failed, return null and set the error code.
[[nodiscard]]
Beatmap *Beatmap_New_LoadFromPath(const char *path, BeatmapError *error);

void Beatmap_Free(Beatmap *beatmap);

int Beatmap_ColumnCount(const Beatmap *beatmap);

int Beatmap_NoteCount(const Beatmap *beatmap);

Tolerance Tolerance_FromOverallDifficulty(double overallDifficulty);

#ifdef FXTAP_CORE_HAS_DIRENT

// Find all beatmaps under a directory, but subdirectories are not searched.
// If failed, return null and set the error code.
[[nodiscard]]
BeatmapFindEntries *BeatmapFindEntries_New_InsideDirectory(const char *path, FindError *error);

#endif //FXTAP_CORE_HAS_DIRENT

void BeatmapFindEntries_Free(BeatmapFindEntries *entries);
