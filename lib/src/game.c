#include <fxTap/game.h>
#include <stdlib.h>

void HoldState_SetDefault(HoldState *holdState)
{
    holdState->HeadIsValid = false;
    holdState->TailIsValid = false;
}

void FxTap_Init(FxTap *fxTap, Beatmap *beatmap)
{
    fxTap->Beatmap = beatmap;
    fxTap->LastUpdateTimeMs = 0;
    fxTap->ColumnCount = Beatmap_ColumnCount(beatmap);

    fxTap->Grades.Perfect = 0;
    fxTap->Grades.Great = 0;
    fxTap->Grades.Good = 0;
    fxTap->Grades.Ok = 0;
    fxTap->Grades.Meh = 0;
    fxTap->Grades.Miss = 0;

    for (char i = 0; i < MAX_COLUMN_COUNT; i += 1)
    {
        fxTap->LastUpdatePressedColumn[i] = false;
        fxTap->Columns[i].AccumulatedTimeMs = 0;

        if (beatmap->Metadata.SizeOfColumn[i] > 0)
        {
            fxTap->Columns[i].FocusedNoteNo = 0;
            HoldState_SetDefault(&fxTap->Columns[i].HoldState);
        } else
        {
            fxTap->Columns[i].FocusedNoteNo = END_OF_COLUMN;
        }
    }
}

Grade GradeTapNote(Tolerance *tolerance, int32_t timeNowMs, bool keyIsDown, int32_t noteStartMs)
{
    // Negative: early
    // Positive: late
    const int32_t delta = timeNowMs - noteStartMs;

    // After the grading area, MISS!
    if (delta > tolerance->Ok) return Grade_Miss;

    // Before the grading area and it's idle
    if (!keyIsDown || delta < -tolerance->Miss) return Grade_Null;

    // Give a grade
    if (delta < -tolerance->Meh) return Grade_Miss;
    if (delta < -tolerance->Ok) return Grade_Meh;
    if (delta < -tolerance->Good) return Grade_Ok;
    if (delta < -tolerance->Great) return Grade_Good;
    if (delta < -tolerance->Perfect) return Grade_Great;
    if (delta <= tolerance->Perfect) return Grade_Perfect;
    if (delta <= tolerance->Great) return Grade_Great;
    if (delta <= tolerance->Good) return Grade_Good;
    if (delta <= tolerance->Ok) return Grade_Ok;
    return Grade_Miss;
}

Grade GradeHoldNoteDefinite(Tolerance *tolerance, HoldState *holdState)
{
    const int32_t headError = abs(holdState->HeadDelta);
    const int32_t tailError = abs(holdState->TailDelta);
    const int32_t totalError = headError + tailError;

    if (headError <= tolerance->Perfect * 1.2 && totalError <= tolerance->Perfect * 2.4)
        return Grade_Perfect;
    if (headError <= tolerance->Great * 1.1 && totalError <= tolerance->Great * 2.2)
        return Grade_Great;
    if (headError <= tolerance->Good && totalError <= tolerance->Good * 2)
        return Grade_Good;
    if (headError <= tolerance->Ok && totalError <= tolerance->Ok * 2)
        return Grade_Ok;

    return Grade_Meh;
}

Grade GradeHoldNote(Tolerance *tolerance, int32_t timeNowMs, bool keyIsDown, bool keyIsUp, HoldState *holdState,
                    int32_t noteStartMs, int32_t noteEndMs)
{
    // Before the grading area
    if (timeNowMs < -tolerance->Miss + noteStartMs)
        return Grade_Null;

    // After the grading area
    if (noteEndMs + tolerance->Meh < timeNowMs)
    {
        // There's a valid hold, stop!
        if (holdState->HeadIsValid &&
            holdState->TailIsValid &&
            holdState->TailDelta >= -tolerance->Meh)
        {
            return GradeHoldNoteDefinite(tolerance, holdState);
        }

        // There's not a valid hold
        return Grade_Miss;
    }

    // Inside grading area

    if (keyIsDown)
    {
        // Record the head delta
        holdState->HeadDelta = timeNowMs - noteStartMs;
        holdState->HeadIsValid = true;
        holdState->TailIsValid = false;
        return Grade_Null;
    }

    if (keyIsUp && holdState->HeadIsValid)
    {
        // Record the tail delta
        holdState->TailDelta = timeNowMs - noteEndMs;

        // Inside the confirmation area, stop!
        if (-tolerance->Meh + noteEndMs <= timeNowMs)
            return GradeHoldNoteDefinite(tolerance, holdState);

        // Outside the confirmation area
        holdState->TailIsValid = true;
        return Grade_Null;
    }

    return Grade_Null;
}

GameUpdateResult FxTap_Update(FxTap *fxTap, FxtapTime timeNowMs, const bool isPressingColumn[16])
{
    if (timeNowMs < fxTap->LastUpdateTimeMs)
        return GameUpdateResult_Error_RewoundTime;

    for (char columnIndex = 0; columnIndex < fxTap->ColumnCount; columnIndex += 1)
    {
        Column *column = &fxTap->Columns[columnIndex];

        if (column->FocusedNoteNo >= fxTap->Beatmap->Metadata.SizeOfColumn[columnIndex])
            continue;

        if (column->FocusedNoteNo == END_OF_COLUMN)
            continue;

        const Note note = fxTap->Beatmap->Notes[columnIndex][column->FocusedNoteNo];
        const int32_t noteStartMs = column->AccumulatedTimeMs + note.AccumulatedStartTime;
        const bool lastUpdatePressed = fxTap->LastUpdatePressedColumn[columnIndex];
        const bool isPressing = isPressingColumn[columnIndex];
        const bool keyIsDown = !lastUpdatePressed && isPressing;
        const bool keyIsUp = lastUpdatePressed && !isPressing;

        Grade grade = note.Duration == 0
                      ? GradeTapNote(&fxTap->Beatmap->Tolerance, timeNowMs, keyIsDown, noteStartMs)
                      : GradeHoldNote(&fxTap->Beatmap->Tolerance, timeNowMs, keyIsDown, keyIsUp,
                                      &column->HoldState, noteStartMs, noteStartMs + note.Duration);

        switch (grade)
        {
            case Grade_Null:
                continue;
            case Grade_Miss:
                fxTap->Grades.Miss += 1;
                break;
            case Grade_Meh:
                fxTap->Grades.Meh += 1;
                break;
            case Grade_Ok:
                fxTap->Grades.Ok += 1;
                break;
            case Grade_Good:
                fxTap->Grades.Good += 1;
                break;
            case Grade_Great:
                fxTap->Grades.Great += 1;
                break;
            case Grade_Perfect:
                fxTap->Grades.Perfect += 1;
                break;
        }

        column->FocusedNoteNo += 1;
        column->AccumulatedTimeMs += note.AccumulatedStartTime;
        HoldState_SetDefault(&fxTap->Columns[columnIndex].HoldState);
    }

    for (char key = 0; key < MAX_COLUMN_COUNT; key += 1)
        fxTap->LastUpdatePressedColumn[key] = isPressingColumn[key];

    return GameUpdateResult_OK;
}

KeyMapper FxTap_FetchKeyMapper(const FxTap *fxTap, const Config *config)
{
    switch (config->KeyMappingStyle)
    {
        case KeyMappingStyle_DJMAX:
            switch (fxTap->ColumnCount)
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
                default: return NULL;
            }
        case KeyMappingStyle_BeatmaniaIIDX:
            switch (fxTap->ColumnCount)
            {
                case 4: return &KeyMapper_BeatmaniaIIDX_4K;
                case 5: return &KeyMapper_BeatmaniaIIDX_5K;
                case 6: return &KeyMapper_BeatmaniaIIDX_6K;
                case 7: return &KeyMapper_BeatmaniaIIDX_7K;
                case 8: return &KeyMapper_BeatmaniaIIDX_8K;
                default: return NULL;
            }
        case KeyMappingStyle_Coop:
            return NULL;
    }
}
