#pragma once

#include <stdint.h>
#include <fxTap/beatmap.h>

static constexpr auto FXT_EndOfColumn = -1;

typedef int32_t FXT_TimeMs;

typedef enum FXT_Grade
{
	FXT_Grade_Null,
	FXT_Grade_Miss,
	FXT_Grade_Meh,
	FXT_Grade_Ok,
	FXT_Grade_Good,
	FXT_Grade_Great,
	FXT_Grade_Perfect,
} FXT_Grade;

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

typedef struct FXT_Game
{
	const FXT_Beatmap *Beatmap;
	FXT_Tolerance Tolerance;
	uint16_t ColumnOffset[FXT_MaxColumnCount];
	FXT_ColumnState ColumnsStates[FXT_MaxColumnCount];
	FXT_TimeMs LastUpdateTime;
	bool LastUpdatePressedColumn[FXT_MaxColumnCount];
	FXT_Grades Grades;
	uint16_t TimingDistribution[20 + 1 + 20];
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
