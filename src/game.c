#include <stdlib.h>
#include <fxTap/game.h>

void FXT_HoldState_SetDefault(FXT_HoldState *holdState)
{
	holdState->HeadIsValid = false;
	holdState->TailIsValid = false;
}

void FXT_Game_Init(FXT_Game *game, const FXT_Beatmap *beatmap)
{
	game->Beatmap = beatmap;
	game->LastUpdateTime = 0;
	game->ColumnCount = FXT_Beatmap_ColumnCount(beatmap);
	game->Combo = 0;

	game->Grades.Perfect = 0;
	game->Grades.Great = 0;
	game->Grades.Good = 0;
	game->Grades.Ok = 0;
	game->Grades.Meh = 0;
	game->Grades.Miss = 0;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
	{
		game->LastUpdatePressedColumn[column] = false;
		game->ColumnsStates[column].AccumulatedTime = 0;

		if (beatmap->Metadata.SizeOfColumn[column] > 0)
		{
			game->ColumnsStates[column].FocusedNoteNo = 0;
			FXT_HoldState_SetDefault(&game->ColumnsStates[column].HoldState);
		}
		else
		{
			game->ColumnsStates[column].FocusedNoteNo = FXT_EndOfColumn;
		}
	}
}

FXT_Grade GradeTapNote(
	const FXT_Tolerance *tolerance,
	const FXT_TimeMs timeNow,
	const bool keyIsDown,
	const FXT_TimeMs noteStart)
{
	// Negative: early
	// Positive: late
	const FXT_TimeMs delta = timeNow - noteStart;

	// After the grading area, MISS!
	if (delta > tolerance->Ok) return FXT_Grade_Miss;

	// Before the grading area and it's idle
	if (!keyIsDown || delta < -tolerance->Miss) return FXT_Grade_Null;

	// Give a grade
	if (delta < -tolerance->Meh) return FXT_Grade_Miss;
	if (delta < -tolerance->Ok) return FXT_Grade_Meh;
	if (delta < -tolerance->Good) return FXT_Grade_Ok;
	if (delta < -tolerance->Great) return FXT_Grade_Good;
	if (delta < -tolerance->Perfect) return FXT_Grade_Great;
	if (delta <= tolerance->Perfect) return FXT_Grade_Perfect;
	if (delta <= tolerance->Great) return FXT_Grade_Great;
	if (delta <= tolerance->Good) return FXT_Grade_Good;
	if (delta <= tolerance->Ok) return FXT_Grade_Ok;
	return FXT_Grade_Miss;
}

FXT_Grade GradeHoldNoteDefinite(const FXT_Tolerance *tolerance, const FXT_HoldState *holdState)
{
	const auto headError = abs(holdState->HeadDelta);
	const auto tailError = abs(holdState->TailDelta);
	const auto totalError = headError + tailError;

	if (headError <= tolerance->Perfect * 1.2 && totalError <= tolerance->Perfect * 2.4)
		return FXT_Grade_Perfect;
	if (headError <= tolerance->Great * 1.1 && totalError <= tolerance->Great * 2.2)
		return FXT_Grade_Great;
	if (headError <= tolerance->Good && totalError <= tolerance->Good * 2)
		return FXT_Grade_Good;
	if (headError <= tolerance->Ok && totalError <= tolerance->Ok * 2)
		return FXT_Grade_Ok;

	return FXT_Grade_Meh;
}

FXT_Grade GradeHoldNote(
	const FXT_Tolerance *tolerance,
	const FXT_TimeMs timeNow,
	const bool keyIsDown,
	const bool keyIsUp,
	FXT_HoldState *holdState,
	const FXT_TimeMs noteStart,
	const FXT_TimeMs noteEnd)
{
	// Before the grading area
	if (timeNow < -tolerance->Miss + noteStart)
		return FXT_Grade_Null;

	// After the grading area
	if (noteEnd + tolerance->Meh < timeNow)
	{
		// There's a valid hold, stop!
		if (holdState->HeadIsValid &&
		    holdState->TailIsValid &&
		    holdState->TailDelta >= -tolerance->Meh)
		{
			return GradeHoldNoteDefinite(tolerance, holdState);
		}

		// There's not a valid hold
		return FXT_Grade_Miss;
	}

	// Inside grading area

	if (keyIsDown)
	{
		// Record the head delta
		holdState->HeadDelta = timeNow - noteStart;
		holdState->HeadIsValid = true;
		holdState->TailIsValid = false;
		return FXT_Grade_Null;
	}

	if (keyIsUp && holdState->HeadIsValid)
	{
		// Record the tail delta
		holdState->TailDelta = timeNow - noteEnd;

		// Inside the confirmation area, stop!
		if (-tolerance->Meh + noteEnd <= timeNow)
			return GradeHoldNoteDefinite(tolerance, holdState);

		// Outside the confirmation area
		holdState->TailIsValid = true;
		return FXT_Grade_Null;
	}

	return FXT_Grade_Null;
}

FXT_GameUpdateResult FXT_Game_Update(
	FXT_Game *game,
	const FXT_TimeMs timeNow,
	const bool isPressingColumn[16])
{
	if (timeNow < game->LastUpdateTime)
		return FxTapUpdateResult_Error_RewoundTime;

	bool ended = true;

	for (int columnIndex = 0; columnIndex < game->ColumnCount; columnIndex += 1)
	{
		FXT_ColumnState *column = &game->ColumnsStates[columnIndex];

		if (column->FocusedNoteNo >= game->Beatmap->Metadata.SizeOfColumn[columnIndex])
			continue;

		if (column->FocusedNoteNo == FXT_EndOfColumn)
			continue;

		ended = false;

		const FXT_Note note = game->Beatmap->Notes[columnIndex][column->FocusedNoteNo];
		const FXT_TimeMs noteStart = column->AccumulatedTime + note.AccumulatedStartTime;
		const bool lastUpdatePressed = game->LastUpdatePressedColumn[columnIndex];
		const bool isPressing = isPressingColumn[columnIndex];
		const bool keyIsDown = !lastUpdatePressed && isPressing;
		const bool keyIsUp = lastUpdatePressed && !isPressing;

		const FXT_Grade grade =
				note.Duration == 0
					? GradeTapNote(&game->Beatmap->Tolerance, timeNow, keyIsDown, noteStart)
					: GradeHoldNote(&game->Beatmap->Tolerance, timeNow, keyIsDown, keyIsUp,
					                &column->HoldState, noteStart, noteStart + note.Duration);

		switch (grade)
		{
		case FXT_Grade_Null: continue;
		case FXT_Grade_Miss: game->Grades.Miss += 1;
			break;
		case FXT_Grade_Meh: game->Grades.Meh += 1;
			break;
		case FXT_Grade_Ok: game->Grades.Ok += 1;
			break;
		case FXT_Grade_Good: game->Grades.Good += 1;
			break;
		case FXT_Grade_Great: game->Grades.Great += 1;
			break;
		case FXT_Grade_Perfect: game->Grades.Perfect += 1;
			break;
		}

		if (grade != FXT_Grade_Miss)
			game->Combo += 1;
		else
			game->Combo = 0;

		column->FocusedNoteNo += 1;
		column->AccumulatedTime += note.AccumulatedStartTime;
		FXT_HoldState_SetDefault(&game->ColumnsStates[columnIndex].HoldState);
	}

	if (ended)
		return FxTapUpdateResult_Ended;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
		game->LastUpdatePressedColumn[column] = isPressingColumn[column];

	return FxTapUpdateResult_OK;
}

KeyMapper FXT_Game_FetchKeyMapper(const FXT_Game *game, const FXT_Config *config)
{
	switch (config->KeyMapStyle)
	{
	case FXT_KeyMapStyle_DJMAX:
		switch (game->ColumnCount)
		{
		case 1: return &KeyMapper_DJMAX_1K;
		case 2: return &KeyMapper_DJMAX_2K;
		case 3: return &KeyMapper_DJMAX_3K;
		case 4: return &KeyMapper_DJMAX_4K;
		case 5: return &KeyMapper_DJMAX_5K;
		case 6: return &KeyMapper_DJMAX_6K;
		case 7: return &KeyMapper_DJMAX_7K;
		case 8: return &KeyMapper_DJMAX_8K;
		case 9: return &KeyMapper_DJMAX_9K;
		default: return nullptr;
		}
	case FXT_KeyMapStyle_BeatmaniaIIDX:
		switch (game->ColumnCount)
		{
		case 4: return &KeyMapper_BeatmaniaIIDX_4K;
		case 5: return &KeyMapper_BeatmaniaIIDX_5K;
		case 6: return &KeyMapper_BeatmaniaIIDX_6K;
		case 7: return &KeyMapper_BeatmaniaIIDX_7K;
		case 8: return &KeyMapper_BeatmaniaIIDX_8K;
		default: return nullptr;
		}
	default:
		return nullptr;
	}
}
