#pragma once

#include <stdint.h>
#include <fxTap/beatmap.h>
#include <fxTap/config.h>
#include <fxTap/keymap.h>

#define END_OF_COLUMN (-1)

// Unit: millisecond (ms)
typedef int32_t FxtapTime;

typedef enum Grade
{
	Grade_Null,
	Grade_Miss,
	Grade_Meh,
	Grade_Ok,
	Grade_Good,
	Grade_Great,
	Grade_Perfect,
} Grade;

typedef struct HoldState
{
	bool HeadIsValid;
	bool TailIsValid;
	int32_t HeadDelta;
	int32_t TailDelta;
} HoldState;

void HoldState_SetDefault(HoldState *holdState);

typedef struct ColumnState
{
	FxtapTime AccumulatedTimeMs;
	int32_t FocusedNoteNo;
	HoldState HoldState;
} ColumnState;

typedef struct FxTap
{
	const FXT_Beatmap *Beatmap;

	int ColumnCount;
	ColumnState ColumnsStates[FXT_MaxColumnCount];
	int32_t LastUpdateTimeMs;
	bool LastUpdatePressedColumn[FXT_MaxColumnCount];
	FXT_Grades Grades;
	unsigned int Combo;
} FxTap;

typedef enum FxTapUpdateResult
{
	FxTapUpdateResult_Ended,
	FxTapUpdateResult_OK,
	FxTapUpdateResult_Error_RewoundTime,
} FxTapUpdateResult;

void FxTap_Init(FxTap *fxTap, const FXT_Beatmap *beatmap);

FxTapUpdateResult FxTap_Update(FxTap *fxTap, FxtapTime timeNowMs, const bool isPressingColumn[FXT_MaxColumnCount]);

KeyMapper FxTap_FetchKeyMapper(const FxTap *fxTap, const FXT_Config *config);
