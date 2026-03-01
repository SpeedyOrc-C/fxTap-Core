#ifdef FXTAP_CORE_USE_CASIOWIN

#include <stdlib.h>
#include <fxTap/beatmap.h>
#include <fxTap/bfile-interface.h>
#include <fxTap/endian-utility.h>

FXT_BeatmapError Metadata_LoadFromBFile(FXT_Metadata *dst, const int bfile)
{
	char header[16];

	if (sizeof(header) > BFile_Read(bfile, header, sizeof(header), -1))
		return FXT_BeatmapError_ReadMetadataFailed;

	if (sizeof(FXT_Metadata) > BFile_Read(bfile, dst, sizeof(FXT_Metadata), -1))
		return FXT_BeatmapError_ReadMetadataFailed;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
		SwapBytes(dst->SizeOfColumn[column]);

	SwapBytes(dst->OverallDifficulty);

	return 0;
}

FXT_BeatmapError Beatmap_LoadFromBFile(FXT_Beatmap *dst, const int file)
{
	const FXT_BeatmapError error = Metadata_LoadFromBFile(&dst->Metadata, file);

	if (error)
		return error;

	dst->Tolerance = FXT_Tolerance_FromOverallDifficulty(dst->Metadata.OverallDifficulty);

	// Load notes
	const int TotalNotesCount = FXT_Beatmap_NoteCount(dst);

	FXT_Note *notesBuffer = calloc(TotalNotesCount, sizeof(FXT_Note));

	if (notesBuffer == nullptr)
		return FXT_BeatmapError_MallocFailed;

	dst->Notes[0] = notesBuffer;
	int accumulatedNotesCount = dst->Metadata.SizeOfColumn[0];

	for (int column = 1; column < FXT_MaxColumnCount; column += 1)
	{
		dst->Notes[column] = notesBuffer + accumulatedNotesCount;
		accumulatedNotesCount += dst->Metadata.SizeOfColumn[column];
	}

	if (sizeof(FXT_Note) * TotalNotesCount >
	    BFile_Read(file, notesBuffer, (int) sizeof(FXT_Note) * TotalNotesCount, -1))
	{
		free(notesBuffer);
		return FXT_BeatmapError_ReadNotesFailed;
	}

	for (int column = 0; column < 8; column += 1)
	{
		for (int note = 0; note < dst->Metadata.SizeOfColumn[column]; note += 1)
		{
			SwapBytes(dst->Notes[column][note].AccumulatedStartTime);
			SwapBytes(dst->Notes[column][note].Duration);
		}
	}

	return FXT_BeatmapError_OK;
}

FXT_BeatmapError FXT_Beatmap_Load_BFile(FXT_Beatmap *dst, const char *path)
{
	FXT_BeatmapError error = 0;
	int file = -1;
	uint16_t *pathU16 = fs_path_normalize_fc(path);

	if (pathU16 == nullptr)
	{
		error = FXT_BeatmapError_MallocFailed;
		goto fail;
	}

	file = BFile_Open(pathU16, BFile_ReadOnly);

	if (file < 0)
	{
		error = FXT_BeatmapError_FileNotFound;
		goto fail;
	}

	error = Beatmap_LoadFromBFile(dst, file);

	if (error)
		goto fail;

	error = 0;

fail:
	if (pathU16 != nullptr) free(pathU16);
	if (file >= 0) BFile_Close(file);
	return error;
}

#endif
