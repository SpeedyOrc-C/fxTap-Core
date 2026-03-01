#pragma once

#include <stddef.h>
#include <stdint.h>
#include <fxTap/lib-config.h>

static constexpr size_t FXT_MaxColumnCount = 16;

typedef struct FXT_Tolerance
{
	int32_t Perfect, Great, Good, Ok, Meh, Miss;
} FXT_Tolerance, FXT_Grades;

typedef struct FXT_Note
{
	uint16_t AccumulatedStartTime;
	uint16_t Duration;
} FXT_Note;

typedef struct FXT_Beatmap
{
	char *Title;
	char *Artist;
	double OverallDifficulty;
	uint8_t ColumnCount;
	uint16_t *ColumnSize;
	FXT_Note *Notes;
} FXT_Beatmap;

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

void FXT_Beatmap_FreeInner(const FXT_Beatmap *beatmap);

void FXT_Beatmap_Free(FXT_Beatmap *beatmap);

int FXT_Beatmap_NoteCount(const FXT_Beatmap *beatmap);

FXT_Tolerance FXT_Tolerance_FromOverallDifficulty(double overallDifficulty);

#ifdef FXTAP_CORE_USE_CASIOWIN

[[nodiscard]]
FXT_BeatmapError FXT_Beatmap_Load_BFile(FXT_Beatmap *dst, const char *path);

#endif
