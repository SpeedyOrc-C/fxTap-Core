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

typedef struct FXT_Metadata
{
	char Header[16];
	char Title[32];
	char Artist[32];
	uint16_t SizeOfColumn[FXT_MaxColumnCount];
	double OverallDifficulty;
} FXT_Metadata;

typedef struct FXT_Tolerance
{
	int32_t Perfect;
	int32_t Great;
	int32_t Good;
	int32_t Ok;
	int32_t Meh;
	int32_t Miss;
} FXT_Tolerance, FXT_Grades;

typedef struct FXT_Note
{
	uint16_t AccumulatedStartTime;
	uint16_t Duration;
} FXT_Note;

typedef struct FXT_Beatmap
{
	FXT_Metadata Metadata;
	FXT_Tolerance Tolerance;
	// An 2D array: Notes[column_index][note_index].
	FXT_Note *Notes[FXT_MaxColumnCount];
} FXT_Beatmap;

typedef struct FXT_BeatmapFindEntry
{
	char *FileName;
	FXT_Metadata Metadata;
} FXT_BeatmapFindEntry;

typedef struct FXT_BeatmapFindEntries
{
	int Count;
	FXT_BeatmapFindEntry *Entries;
} FXT_BeatmapFindEntries;

typedef enum FXT_BeatmapError
{
	FXT_BeatmapError_OK,
	FXT_BeatmapError_MallocFailed,
	FXT_BeatmapError_FileNotFound,
	FXT_BeatmapError_CannotCloseFile,
	FXT_BeatmapError_ReadMetadataFailed,
	FXT_BeatmapError_ReadNotesFailed,
} FXT_BeatmapError;

typedef enum FXT_FindError
{
	FXT_FindError_OK,
	FXT_FindError_MallocFailed,
	FXT_FindError_FxtapFolderNotFound,
	FXT_FindError_BadFile,
} FXT_FindError;

[[nodiscard]]
FXT_BeatmapError FXT_Beatmap_Load(FXT_Beatmap *dst, const char *path);

void FXT_Beatmap_Free(FXT_Beatmap *beatmap);

int FXT_Beatmap_ColumnCount(const FXT_Beatmap *beatmap);

int FXT_Beatmap_NoteCount(const FXT_Beatmap *beatmap);

FXT_Tolerance FXT_Tolerance_FromOverallDifficulty(double overallDifficulty);

#ifdef FXTAP_CORE_USE_CASIOWIN

[[nodiscard]]
FXT_BeatmapError FXT_Beatmap_Load_BFile(FXT_Beatmap *dst, const char *path);

#endif

#ifdef FXTAP_CORE_HAS_DIRENT

// Find all beatmaps under a directory, but subdirectories are not searched.
// If failed, return null and set the error code.
[[nodiscard]]
FXT_BeatmapFindEntries *BeatmapFindEntries_New_InsideDirectory(const char *path, FXT_FindError *error);

#endif //FXTAP_CORE_HAS_DIRENT

void FXT_BeatmapFindEntries_Free(FXT_BeatmapFindEntries *entries);
