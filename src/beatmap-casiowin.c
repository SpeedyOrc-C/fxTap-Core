#ifdef FXTAP_CORE_USE_CASIOWIN

#include <stdlib.h>
#include <fxTap/beatmap.h>
#include <fxTap/bfile-interface.h>
#include <fxTap/endian-utility.h>

FXT_BeatmapError Metadata_LoadFromFile_BFile(FXT_Metadata *metadata, int bfile)
{
	if (sizeof(FXT_Metadata) > BFile_Read(bfile, metadata, sizeof(FXT_Metadata), -1))
		return FXT_BeatmapError_ReadMetadataFailed;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
		SwapBytes(metadata->SizeOfColumn[column]);

	SwapBytes(metadata->OverallDifficulty);

	return FXT_BeatmapError_OK;
}

FXT_BeatmapError Beatmap_LoadFromFile_BFile(FXT_Beatmap *beatmap, int bfile)
{
	const FXT_BeatmapError error = Metadata_LoadFromFile_BFile(&beatmap->Metadata, bfile);

	if (error)
		return error;

	beatmap->Tolerance = FXT_Tolerance_FromOverallDifficulty(beatmap->Metadata.OverallDifficulty);

	// Load notes
	const int TotalNotesCount = FXT_Beatmap_NoteCount(beatmap);

	FXT_Note *notesBuffer = calloc(TotalNotesCount, sizeof(FXT_Note));

	if (notesBuffer == NULL)
		return FXT_BeatmapError_MallocFailed;

	beatmap->Notes[0] = notesBuffer;
	int accumulatedNotesCount = beatmap->Metadata.SizeOfColumn[0];

	for (int column = 1; column < FXT_MaxColumnCount; column += 1)
	{
		beatmap->Notes[column] = notesBuffer + accumulatedNotesCount;
		accumulatedNotesCount += beatmap->Metadata.SizeOfColumn[column];
	}

	if (sizeof(FXT_Note) * TotalNotesCount > BFile_Read(bfile, notesBuffer, (int) sizeof(FXT_Note) * TotalNotesCount, -1))
	{
		free(notesBuffer);
		return FXT_BeatmapError_ReadNotesFailed;
	}

	for (int column = 0; column < 8; column += 1)
	{
		for (int note = 0; note < beatmap->Metadata.SizeOfColumn[column]; note += 1)
		{
			SwapBytes(beatmap->Notes[column][note].AccumulatedStartTime);
			SwapBytes(beatmap->Notes[column][note].Duration);
		}
	}

	return FXT_BeatmapError_OK;
}

FXT_Beatmap *FXT_Beatmap_Load_BFile(const char *path, FXT_BeatmapError *error)
{
	uint16_t *pathCasiowin = fs_path_normalize_fc(path);

	if (pathCasiowin == NULL)
		return nullptr;

	int bfile = BFile_Open(pathCasiowin, BFile_ReadOnly);

	if (bfile < 0)
	{
		free(pathCasiowin);
		*error = FXT_BeatmapError_FileNotFound;
		return nullptr;
	}

	FXT_Beatmap *beatmap = malloc(sizeof(FXT_Beatmap));

	if (beatmap == NULL)
	{
		free(pathCasiowin);
		BFile_Close(bfile);
		*error = FXT_BeatmapError_MallocFailed;
		return nullptr;
	}

	*error = Beatmap_LoadFromFile_BFile(beatmap, bfile);

	if (*error)
	{
		free(pathCasiowin);
		free(beatmap);
		BFile_Close(bfile);
		return nullptr;
	}

	free(pathCasiowin);
	BFile_Close(bfile);

	*error = FXT_BeatmapError_OK;
	return beatmap;
}

#endif
