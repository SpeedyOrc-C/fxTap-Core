#include <fxTap/beatmap.h>
#include <fxTap/game.h>
#include <fxTap/render.h>

void FXT_RendererController_Run(
	const FXT_RendererController *controller,
	const FXT_Game *game,
	const FXT_TimeMs timeNow)
{
	auto const timeScale = controller->HeightAbove / controller->VisibleTime;

	for (int column = 0; column < game->Beatmap->ColumnCount && game->Beatmap->ColumnSize[column] > 0; column += 1)
	{
		const FXT_TimeMs initialAccumulatedStartTime = game->ColumnsStates[column].AccumulatedTime;
		const int32_t focusedNoteNo = game->ColumnsStates[column].FocusedNoteNo;
		const uint16_t columnSize = game->Beatmap->ColumnSize[column];
		auto const columnOffset = game->ColumnOffset[column];

		// Render notes above the line
		FXT_TimeMs controllerAccumulatedStartTime = initialAccumulatedStartTime;

		for (int noteNo = focusedNoteNo; noteNo < columnSize; noteNo += 1)
		{
			const FXT_Note note = game->Beatmap->Notes[noteNo + columnOffset];
			const FXT_TimeMs noteAccumulatedStartTime = note.AccumulatedStartTime;
			const FXT_TimeMs headTime = controllerAccumulatedStartTime + noteAccumulatedStartTime;
			const FXT_TimeMs headTimeToBottom = headTime - timeNow;
			const FXT_TimeMs tailTimeToBottom = headTimeToBottom + note.Duration;

			controllerAccumulatedStartTime += noteAccumulatedStartTime;

			// Stop if the head is above the screen top
			if (headTimeToBottom > controller->VisibleTime)
				break;

			const bool isTap = note.Duration == 0;

			// Stop if the tail is below the line,
			// but allow taps render for an extra 100ms.
			if ((isTap && tailTimeToBottom < -100) || (!isTap && tailTimeToBottom < 0))
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
			const FXT_Note note = game->Beatmap->Notes[noteNo + columnOffset];
			const FXT_TimeMs noteAccumulatedStartTime = note.AccumulatedStartTime;
			const FXT_TimeMs tailTime = controllerAccumulatedStartTime + note.Duration;
			const FXT_TimeMs tailTimeToBottom = tailTime - timeNow;

			controllerAccumulatedStartTime -= noteAccumulatedStartTime;

			const bool isTap = note.Duration == 0;

			// Stop if the tail is below the line,
			// but allow taps render for an extra 100ms.
			if ((isTap && tailTimeToBottom < -100) || (!isTap && tailTimeToBottom < 0))
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
