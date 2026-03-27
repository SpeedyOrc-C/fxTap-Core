#pragma once

#include <stdint.h>
#include <fxTap/beatmap.h>

static constexpr auto FXT_EndOfColumn = -1;

typedef int32_t FXT_TimeMs;

/*
Difference between actual tap time and note time.
Negative: Early
Positive: Late
*/
typedef int32_t FXT_DeltaMs;

typedef enum FXT_GradeLevel
{
	FXT_GradeLevel_Null,
	FXT_GradeLevel_Miss,
	FXT_GradeLevel_Meh,
	FXT_GradeLevel_Ok,
	FXT_GradeLevel_Good,
	FXT_GradeLevel_Great,
	FXT_GradeLevel_Perfect,
} FXT_GradeLevel;

typedef struct FXT_Grade
{
	FXT_GradeLevel Level;
	FXT_DeltaMs Delta;
} FXT_Grade;

typedef struct FXT_HoldGrade
{
	FXT_GradeLevel Level;
	FXT_DeltaMs HeadDelta, TailDelta;
} FXT_HoldGrade;

typedef struct FXT_HoldState
{
	bool HeadIsValid;
	bool TailIsValid;
	int32_t HeadDelta;
	int32_t TailDelta;
} FXT_HoldState;

typedef struct FXT_ColumnState
{
	FXT_TimeMs AccumulatedTime;
	int32_t FocusedNoteNo;
	FXT_HoldState HoldState;
} FXT_ColumnState;

static constexpr size_t FXT_TimingDistributionOneSideColumnCount = 20;

typedef uint16_t FXT_TimingDistribution[
	FXT_TimingDistributionOneSideColumnCount
	+ 1
	+ FXT_TimingDistributionOneSideColumnCount
];

typedef struct FXT_Game
{
	const FXT_Beatmap *Beatmap;
	FXT_Tolerance Tolerance;
	uint16_t ColumnOffset[FXT_MaxColumnCount];
	FXT_ColumnState ColumnsStates[FXT_MaxColumnCount];
	FXT_TimeMs LastUpdateTime;
	bool LastUpdatePressedColumn[FXT_MaxColumnCount];
	FXT_Grades Grades;
	FXT_TimingDistribution TimingDistribution;
	uint16_t Combo;
} FXT_Game;

typedef enum FXT_GameUpdateResult
{
	FXT_GameUpdateResult_Ended,
	FXT_GameUpdateResult_OK,
	FXT_GameUpdateResult_RewoundTimeError,
} FXT_GameUpdateResult;

void FXT_Game_Init(FXT_Game *game, const FXT_Beatmap *beatmap);

FXT_GameUpdateResult FXT_Game_Update(
	FXT_Game *game,
	FXT_TimeMs timeNow,
	const bool isPressingColumn[FXT_MaxColumnCount]
);

FXT_TimeMs FXT_Game_FirstNoteStartTime(const FXT_Game *game);

FXT_TimeMs FXT_Game_LastNoteEndTime(const FXT_Game *game);
