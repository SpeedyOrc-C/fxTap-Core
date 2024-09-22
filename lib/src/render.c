#include <fxTap/render.h>
#include <fxTap/game.h>
#include <fxTap/beatmap.h>

void RendererController_Run(const RendererController *controller, const FxTap *fxTap, FxtapTime timeNow)
{
    const double TimeScale = controller->HeightAbove / controller->VisibleTime;
    const Metadata *Metadata = &fxTap->Beatmap->Metadata;

    for (char column = 0; column < fxTap->ColumnCount && Metadata->SizeOfColumn[column] > 0; column += 1)
    {
        const FxtapTime InitialAccumulatedStartTime = fxTap->Columns[column].AccumulatedTimeMs;
        const int32_t FocusedNoteNo = fxTap->Columns[column].FocusedNoteNo;
        const uint16_t SizeOfColumn = Metadata->SizeOfColumn[column];

        FxtapTime controllerAccumulatedStartTime;

        // Render notes above the line
        controllerAccumulatedStartTime = InitialAccumulatedStartTime;

        for (int noteNo = FocusedNoteNo; noteNo < SizeOfColumn; noteNo += 1)
        {
            const Note *note = &fxTap->Beatmap->Notes[column][noteNo];
            const FxtapTime NoteAccumulatedStartTime = note->AccumulatedStartTime;
            const FxtapTime NoteStartTime = controllerAccumulatedStartTime + NoteAccumulatedStartTime;
            const FxtapTime HeadTimeDelta = NoteStartTime - timeNow;
            const FxtapTime TailTimeDelta = HeadTimeDelta + note->Duration;

            controllerAccumulatedStartTime += NoteAccumulatedStartTime;

            // Stop if the head is above the screen top
            if (HeadTimeDelta > controller->VisibleTime)
                break;

            if (TailTimeDelta < 0)
                continue;

            const double PositionBottom = TimeScale * HeadTimeDelta;

            if (note->Duration == 0)
            {
                controller->RenderTap(column, PositionBottom);
            } else
            {
                const double PositionTop = TimeScale * (HeadTimeDelta + note->Duration);

                controller->RenderHold(column, PositionBottom, PositionTop);
            }
        }

        // Render hold notes below the line
        controllerAccumulatedStartTime = InitialAccumulatedStartTime;

        for (int noteNo = FocusedNoteNo - 1; noteNo >= 0; noteNo -= 1)
        {
            const Note *note = &fxTap->Beatmap->Notes[column][noteNo];
            const FxtapTime NoteAccumulatedStartTime = note->AccumulatedStartTime;
            const FxtapTime NoteEndTime = controllerAccumulatedStartTime + note->Duration;
            const FxtapTime TailTimeDelta = NoteEndTime - timeNow;

            controllerAccumulatedStartTime -= NoteAccumulatedStartTime;

            // Stop if the tail is below the line
            if (TailTimeDelta < 0)
                break;

            // Skip if this is a tap note
            if (note->Duration == 0)
                continue;

            const double PositionTop = TimeScale * TailTimeDelta;
            const double PositionBottom = TimeScale * (TailTimeDelta - note->Duration);

            controller->RenderHold(column, PositionBottom, PositionTop);
        }
    }
}
