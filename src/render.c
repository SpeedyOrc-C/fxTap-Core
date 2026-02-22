#include <fxTap/beatmap.h>
#include <fxTap/game.h>
#include <fxTap/render.h>

void FXT_RendererController_Run(
	const FXT_RendererController *controller,
	const FXT_Game *game,
	const FxtapTime timeNow)
{
	const double timeScale = controller->HeightAbove / controller->VisibleTime;
	const FXT_Metadata *metadata = &game->Beatmap->Metadata;

	for (int column = 0; column < game->ColumnCount && metadata->SizeOfColumn[column] > 0; column += 1)
	{
		const FxtapTime initialAccumulatedStartTime = game->ColumnsStates[column].AccumulatedTimeMs;
		const int32_t focusedNoteNo = game->ColumnsStates[column].FocusedNoteNo;
		const uint16_t sizeOfColumn = metadata->SizeOfColumn[column];

		// Render notes above the line
		FxtapTime controllerAccumulatedStartTime = initialAccumulatedStartTime;

		for (int noteNo = focusedNoteNo; noteNo < sizeOfColumn; noteNo += 1)
		{
			const FXT_Note note = game->Beatmap->Notes[column][noteNo];
			const FxtapTime noteAccumulatedStartTime = note.AccumulatedStartTime;
			const FxtapTime headTime = controllerAccumulatedStartTime + noteAccumulatedStartTime;
			const FxtapTime headTimeToBottom = headTime - timeNow;
			const FxtapTime tailTimeToBottom = headTimeToBottom + note.Duration;

			controllerAccumulatedStartTime += noteAccumulatedStartTime;

			// Stop if the head is above the screen top
			if (headTimeToBottom > controller->VisibleTime)
				break;

			const bool isTap = note.Duration == 0;

			// Stop if the tail is below the line,
			// but allow taps render for an extra 100ms.
			if (isTap && tailTimeToBottom < -100 || !isTap && tailTimeToBottom < 0)
				continue;

			const double PositionBottom = timeScale * headTimeToBottom;

			if (note.Duration == 0)
				controller->RenderTap(column, PositionBottom);
			else
			{
				const double PositionTop = timeScale * (headTimeToBottom + note.Duration);
				controller->RenderHold(column, PositionBottom, PositionTop);
			}
		}

		// Render notes below the line
		controllerAccumulatedStartTime = initialAccumulatedStartTime;

		for (int noteNo = focusedNoteNo - 1; noteNo >= 0; noteNo -= 1)
		{
			const FXT_Note note = game->Beatmap->Notes[column][noteNo];
			const FxtapTime noteAccumulatedStartTime = note.AccumulatedStartTime;
			const FxtapTime tailTime = controllerAccumulatedStartTime + note.Duration;
			const FxtapTime tailTimeToBottom = tailTime - timeNow;

			controllerAccumulatedStartTime -= noteAccumulatedStartTime;

			const bool isTap = note.Duration == 0;

			// Stop if the tail is below the line,
			// but allow taps render for an extra 100ms.
			if (isTap && tailTimeToBottom < -100 || !isTap && tailTimeToBottom < 0)
				break;

			const double PositionBottom = timeScale * (tailTimeToBottom - note.Duration);

			if (isTap)
			{
				controller->RenderTap(column, PositionBottom);
				continue;
			}

			const double PositionTop = timeScale * tailTimeToBottom;

			controller->RenderHold(column, PositionBottom, PositionTop);
		}
	}
}
