#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fxTap/beatmap.h>
#include <fxTap/endian-utility.h>

#ifdef FXTAP_CORE_HAS_DIRENT
#include <dirent.h>
#endif

FXT_Tolerance FXT_Tolerance_FromOverallDifficulty(const double overallDifficulty)
{
	const double od3 = 3 * overallDifficulty;

	return (FXT_Tolerance){
		.Perfect = BASE_TOLERANCE_PERFECT,
		.Great = (int32_t) ((double) BASE_TOLERANCE_GREAT - od3),
		.Good = (int32_t) ((double) BASE_TOLERANCE_GOOD - od3),
		.Ok = (int32_t) ((double) BASE_TOLERANCE_OK - od3),
		.Meh = (int32_t) ((double) BASE_TOLERANCE_MEH - od3),
		.Miss = (int32_t) ((double) BASE_TOLERANCE_MISS - od3),
	};
}

FXT_BeatmapError Metadata_LoadFromFile(FXT_Metadata *metadata, FILE *file)
{
	if (1 > fread(metadata, sizeof(FXT_Metadata), 1, file))
		return FXT_BeatmapError_ReadMetadataFailed;

	if (EnvironmentIsBigEndian())
	{
		for (int column = 0; column < FXT_MaxColumnCount; column += 1)
			SwapBytes(metadata->SizeOfColumn[column]);

		SwapBytes(metadata->OverallDifficulty);
	}

	return FXT_BeatmapError_OK;
}

int FXT_Beatmap_NoteCount(const FXT_Beatmap *beatmap)
{
	int count = 0;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
		count += beatmap->Metadata.SizeOfColumn[column];

	return count;
}

FXT_BeatmapError Beatmap_LoadFromFile(FXT_Beatmap *beatmap, FILE *file)
{
	const FXT_BeatmapError error = Metadata_LoadFromFile(&beatmap->Metadata, file);

	if (error)
		return error;

	beatmap->Tolerance = FXT_Tolerance_FromOverallDifficulty(beatmap->Metadata.OverallDifficulty);

	// Load notes
	const int TotalNotesCount = FXT_Beatmap_NoteCount(beatmap);

	FXT_Note *notesBuffer = calloc(TotalNotesCount, sizeof(FXT_Note));

	if (notesBuffer == nullptr)
		return FXT_BeatmapError_MallocFailed;

	beatmap->Notes[0] = notesBuffer;
	int accumulatedNotesCount = beatmap->Metadata.SizeOfColumn[0];

	for (int column = 1; column < FXT_MaxColumnCount; column += 1)
	{
		beatmap->Notes[column] = notesBuffer + accumulatedNotesCount;
		accumulatedNotesCount += beatmap->Metadata.SizeOfColumn[column];
	}

	if (TotalNotesCount > fread(notesBuffer, sizeof(FXT_Note), TotalNotesCount, file))
	{
		free(notesBuffer);
		return FXT_BeatmapError_ReadNotesFailed;
	}

	if (EnvironmentIsBigEndian())
	{
		for (int column = 0; column < 8; column += 1)
		{
			for (int note = 0; note < beatmap->Metadata.SizeOfColumn[column]; note += 1)
			{
				SwapBytes(beatmap->Notes[column][note].AccumulatedStartTime);
				SwapBytes(beatmap->Notes[column][note].Duration);
			}
		}
	}

	return 0;
}

FXT_Beatmap *FXT_Beatmap_Load(const char *path, FXT_BeatmapError *error)
{
	FXT_Beatmap *beatmap = nullptr;

	FILE *file = fopen(path, "rb");

	if (file == nullptr)
	{
		*error = FXT_BeatmapError_FileNotFound;
		goto fail;
	}

	beatmap = malloc(sizeof(FXT_Beatmap));

	if (beatmap == nullptr)
	{
		*error = FXT_BeatmapError_MallocFailed;
		goto fail;
	}

	*error = Beatmap_LoadFromFile(beatmap, file);

	if (*error)
		goto fail;

	fclose(file);
	*error = FXT_BeatmapError_OK;
	return beatmap;

fail:
	if (file != nullptr) fclose(file);
	if (beatmap != nullptr) free(beatmap);
	return nullptr;
}

void FXT_Beatmap_Free(FXT_Beatmap *beatmap)
{
	free(beatmap->Notes[0]);
	free(beatmap);
}

int FXT_Beatmap_ColumnCount(const FXT_Beatmap *beatmap)
{
	int fromColumn = -1;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
	{
		if (beatmap->Metadata.SizeOfColumn[column] > 0)
		{
			fromColumn = column;
			break;
		}
	}

	if (fromColumn == -1)
		return 0;

	int toColumn = FXT_MaxColumnCount + 1;

	for (int column = FXT_MaxColumnCount - 1; column >= 0; column -= 1)
	{
		if (beatmap->Metadata.SizeOfColumn[column] > 0)
		{
			toColumn = column;
			break;
		}
	}

	return toColumn - fromColumn + 1;
}

#ifdef FXTAP_CORE_HAS_DIRENT

FXT_BeatmapFindEntries *BeatmapFindEntries_New_InsideDirectory(const char *path, FXT_FindError *error)
{
	FXT_BeatmapFindEntries *entries = malloc(sizeof(FXT_BeatmapFindEntries));

	if (entries == nullptr)
	{
		*error = FXT_FindError_MallocFailed;
		return nullptr;
	}

	DIR *directory = opendir(path);

	if (directory == nullptr)
	{
		free(entries);
		*error = FXT_FindError_FxtapFolderNotFound;
		return nullptr;
	}

	entries->Count = 0;

	entries->Entries = calloc(1, sizeof(FXT_BeatmapFindEntry));

	while (true)
	{
		// Read an entry
		struct dirent *entry = readdir(directory);

		if (entry == nullptr)
			break;

		// Check whether the file extension is *.FXT
		const unsigned long fileNameLength = strlen(entry->d_name);

		if (fileNameLength < 5)
			continue;

		if (toupper(entry->d_name[fileNameLength - 3]) != 'F' ||
		    toupper(entry->d_name[fileNameLength - 2]) != 'X' ||
		    toupper(entry->d_name[fileNameLength - 1]) != 'T')
		{
			continue;
		}

		// Extend the file name list
		entries->Count += 1;

		FXT_BeatmapFindEntry *extendedEntries =
				realloc(entries->Entries, entries->Count * sizeof(FXT_BeatmapFindEntry));

		if (extendedEntries == nullptr)
		{
			closedir(directory);

			for (int i = 0; i < entries->Count - 1; i += 1)
				free(entries->Entries[i].FileName);
			free(entries->Entries);
			free(entries);
			*error = FXT_FindError_MallocFailed;
			return nullptr;
		}

		entries->Entries = extendedEntries;

		// Append the entry's file name to the list
		char *fileName = malloc(strlen(entry->d_name) + 1);

		if (fileName == nullptr)
		{
			closedir(directory);

			for (int i = 0; i < entries->Count - 1; i += 1)
				free(entries->Entries[i].FileName);
			free(entries->Entries);
			free(entries);
			*error = FXT_FindError_MallocFailed;
			return nullptr;
		}

		strcpy(fileName, entry->d_name);

		entries->Entries[entries->Count - 1].FileName = fileName;

		// Compose the beatmap's path
		char filePath[strlen(path) + 1 + strlen(fileName) + 1];

		sprintf(filePath, "%s/%s", path, fileName);

		// Read metadata
		FILE *file = fopen(filePath, "rb");

		assert(file != nullptr);

		if (Metadata_LoadFromFile(&entries->Entries[entries->Count - 1].Metadata, file))
		{
			closedir(directory);

			for (int i = 0; i < entries->Count; i += 1)
				free(entries->Entries[i].FileName);
			free(entries->Entries);
			free(entries);
			*error = FXT_FindError_BadFile;
			return nullptr;
		}

		fclose(file);
	}

	closedir(directory);

	*error = FXT_FindError_OK;
	return entries;
}

#endif

void FXT_BeatmapFindEntries_Free(FXT_BeatmapFindEntries *entries)
{
	for (int i = 0; i < entries->Count; i += 1)
		free(entries->Entries[i].FileName);
	free(entries->Entries);
	free(entries);
}
