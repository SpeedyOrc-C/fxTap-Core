#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fxTap/game.h>

static void FXT_HoldState_SetDefault(FXT_HoldState *holdState)
{
	holdState->HeadIsValid = false;
	holdState->TailIsValid = false;
}

void FXT_Game_Init(FXT_Game *game, const FXT_Beatmap *beatmap)
{
	game->Beatmap = beatmap;
	game->Tolerance = FXT_Tolerance_FromOverallDifficulty(beatmap->OverallDifficulty);
	game->LastUpdateTime = -1000;
	game->Combo = 0;

	game->Grades.Perfect = 0;
	game->Grades.Great = 0;
	game->Grades.Good = 0;
	game->Grades.Ok = 0;
	game->Grades.Meh = 0;
	game->Grades.Miss = 0;

	memset(game->TimingDistribution, 0, sizeof(game->TimingDistribution));

	int columnOffset = 0;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
	{
		game->ColumnOrder[column] = column;
		game->ColumnOffset[column] = columnOffset;
		game->LastUpdatePressedColumn[column] = false;
		game->ColumnsStates[column].AccumulatedTime = 0;

		columnOffset += beatmap->ColumnSize[column];

		if (beatmap->ColumnSize[column] > 0)
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

static size_t TimingDistributionIndexFromDelta(const FXT_DeltaMs delta)
{
	auto index = (int) round((double) delta / 10)
	             + (int) FXT_TimingDistributionOneSideColumnCount;

	if (index > 40)
		index = 40;
	else if (index < 0)
		index = 0;

	return (size_t) index;
}

static FXT_Grade GradeTapNote(
	const FXT_Tolerance *tolerance,
	const FXT_TimeMs timeNow,
	const bool keyIsDown,
	const FXT_TimeMs noteStart)
{
	const FXT_DeltaMs delta = timeNow - noteStart;

	// After the grading area, MISS!
	if (delta > tolerance->Ok)
		return (FXT_Grade){FXT_GradeLevel_Miss, delta};

	// Before the grading area and it's idle
	if (! keyIsDown || delta < -tolerance->Miss)
		return (FXT_Grade){FXT_GradeLevel_Null};

	// Give a grade
	if (delta < -tolerance->Meh)
		return (FXT_Grade){FXT_GradeLevel_Miss, delta};
	if (delta < -tolerance->Ok)
		return (FXT_Grade){FXT_GradeLevel_Meh, delta};
	if (delta < -tolerance->Good)
		return (FXT_Grade){FXT_GradeLevel_Ok, delta};
	if (delta < -tolerance->Great)
		return (FXT_Grade){FXT_GradeLevel_Good, delta};
	if (delta < -tolerance->Perfect)
		return (FXT_Grade){FXT_GradeLevel_Great, delta};
	if (delta <= tolerance->Perfect)
		return (FXT_Grade){FXT_GradeLevel_Perfect, delta};
	if (delta <= tolerance->Great)
		return (FXT_Grade){FXT_GradeLevel_Great, delta};
	if (delta <= tolerance->Good)
		return (FXT_Grade){FXT_GradeLevel_Good, delta};
	if (delta <= tolerance->Ok)
		return (FXT_Grade){FXT_GradeLevel_Ok, delta};

	return (FXT_Grade){FXT_GradeLevel_Miss};
}

static FXT_HoldGrade GradeHoldNoteDefinite(const FXT_Tolerance *tolerance, const FXT_HoldState *holdState)
{
	auto const head = holdState->HeadDelta;
	auto const tail = holdState->TailDelta;
	auto const absHead = abs(head);
	auto const absTail = abs(tail);
	auto const absTotal = absHead + absTail;

	if (absHead <= tolerance->Perfect * 1.2 && absTotal <= tolerance->Perfect * 2.4)
		return (FXT_HoldGrade){FXT_GradeLevel_Perfect, head, tail};
	if (absHead <= tolerance->Great * 1.1 && absTotal <= tolerance->Great * 2.2)
		return (FXT_HoldGrade){FXT_GradeLevel_Great, head, tail};
	if (absHead <= tolerance->Good && absTotal <= tolerance->Good * 2)
		return (FXT_HoldGrade){FXT_GradeLevel_Good, head, tail};
	if (absHead <= tolerance->Ok && absTotal <= tolerance->Ok * 2)
		return (FXT_HoldGrade){FXT_GradeLevel_Ok, head, tail};

	return (FXT_HoldGrade){FXT_GradeLevel_Meh, head, tail};
}

static FXT_HoldGrade GradeHoldNote(
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
		return (FXT_HoldGrade){FXT_GradeLevel_Null};

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
		return (FXT_HoldGrade){FXT_GradeLevel_Miss};
	}

	// Inside grading area

	if (keyIsDown)
	{
		// Record the head delta
		holdState->HeadDelta = timeNow - noteStart;
		holdState->HeadIsValid = true;
		holdState->TailIsValid = false;
		return (FXT_HoldGrade){FXT_GradeLevel_Null};
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
		return (FXT_HoldGrade){FXT_GradeLevel_Null};
	}

	return (FXT_HoldGrade){FXT_GradeLevel_Null};
}

FXT_GameUpdateResult FXT_Game_Update(
	FXT_Game *game,
	const FXT_TimeMs timeNow,
	const bool isPressingColumn[16])
{
	if (timeNow < game->LastUpdateTime)
		return FXT_GameUpdateResult_RewoundTimeError;

	bool ended = true;

	for (size_t column = 0; column < game->Beatmap->ColumnCount; column += 1)
	{
		FXT_ColumnState *state = &game->ColumnsStates[column];

		if (state->FocusedNoteNo >= game->Beatmap->ColumnSize[column])
			continue;

		if (state->FocusedNoteNo == FXT_EndOfColumn)
			continue;

		ended = false;

		const FXT_Note note = game->Beatmap->Notes[state->FocusedNoteNo + game->ColumnOffset[column]];
		const FXT_TimeMs noteStart = state->AccumulatedTime + note.AccumulatedStartTime;
		const bool lastUpdatePressed = game->LastUpdatePressedColumn[column];
		const bool isPressing = isPressingColumn[game->ColumnOrder[column]];
		const bool keyIsDown = ! lastUpdatePressed && isPressing;
		const bool keyIsUp = lastUpdatePressed && ! isPressing;

		FXT_GradeLevel level;

		if (note.Duration == 0)
		{
			auto const grade = GradeTapNote(&game->Tolerance, timeNow, keyIsDown, noteStart);

			level = grade.Level;

			if (level != FXT_GradeLevel_Null)
			{
				auto const i = TimingDistributionIndexFromDelta(grade.Delta);
				game->TimingDistribution[i] += 1;
			}
		}
		else
		{
			auto const grade = GradeHoldNote(
				&game->Tolerance, timeNow, keyIsDown, keyIsUp,
				&state->HoldState, noteStart, noteStart + note.Duration);

			level = grade.Level;

			if (level != FXT_GradeLevel_Null)
			{
				auto const iHead = TimingDistributionIndexFromDelta(grade.HeadDelta);
				auto const iTail = TimingDistributionIndexFromDelta(grade.TailDelta);
				game->TimingDistribution[iHead] += 1;
				game->TimingDistribution[iTail] += 1;
			}
		}

		switch (level)
		{
		case FXT_GradeLevel_Null: continue;
		case FXT_GradeLevel_Miss: game->Grades.Miss += 1;
			break;
		case FXT_GradeLevel_Meh: game->Grades.Meh += 1;
			break;
		case FXT_GradeLevel_Ok: game->Grades.Ok += 1;
			break;
		case FXT_GradeLevel_Good: game->Grades.Good += 1;
			break;
		case FXT_GradeLevel_Great: game->Grades.Great += 1;
			break;
		case FXT_GradeLevel_Perfect: game->Grades.Perfect += 1;
			break;
		}

		if (level != FXT_GradeLevel_Miss)
			game->Combo += 1;
		else
			game->Combo = 0;

		state->FocusedNoteNo += 1;
		state->AccumulatedTime += note.AccumulatedStartTime;
		FXT_HoldState_SetDefault(&game->ColumnsStates[column].HoldState);
	}

	if (ended)
		return FXT_GameUpdateResult_Ended;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
		game->LastUpdatePressedColumn[column] = isPressingColumn[column];

	return FXT_GameUpdateResult_OK;
}

FXT_TimeMs FXT_Game_FirstNoteStartTime(const FXT_Game *game)
{
	FXT_TimeMs time = INT32_MAX;

	auto const columnOffsets = game->ColumnOffset;
	auto const beatmap = game->Beatmap;
	auto const columnCount = beatmap->ColumnCount;
	auto const notes = beatmap->Notes;

	for (size_t column = 0; column < columnCount; column += 1)
	{
		auto const columnOffset = columnOffsets[column];
		auto const thisTime = notes[columnOffset].AccumulatedStartTime;

		if (thisTime < time)
			time = thisTime;
	}

	return time;
}

FXT_TimeMs FXT_Game_LastNoteEndTime(const FXT_Game *game)
{
	FXT_TimeMs time = 0;

	auto const beatmap = game->Beatmap;
	auto const notes = beatmap->Notes;
	auto const columnOffsets = game->ColumnOffset;
	auto const columnSizes = beatmap->ColumnSize;

	for (size_t column = 0; column < beatmap->ColumnCount; column += 1)
	{
		FXT_TimeMs thisTime = 0;

		for (size_t note = 0; note < columnSizes[column]; note += 1)
			thisTime += notes[columnOffsets[column] + note].AccumulatedStartTime;

		thisTime += notes[columnOffsets[column] + columnSizes[column] - 1].
				Duration;

		if (thisTime > time)
			time = thisTime;
	}

	return time;
}
