#ifdef FXTAP_CORE_USE_GINT

#include <stdlib.h>
#include <fxTap/beatmap.h>
#include <gint/bfile.h>
#include <gint/fs.h>

static FXT_BeatmapError Beatmap_LoadFromFile_BFile(FXT_Beatmap *dst, const int file, const bool readNotes)
{
	char header[8];
	uint8_t titleLength;
	char *title = nullptr;
	uint8_t artistLength;
	char *artist = nullptr;
	double overallDifficulty;
	uint8_t columnCount;
	uint16_t *columnSize = nullptr;
	FXT_Note *notes = nullptr;

	if (sizeof(header) > BFile_Read(file, header, sizeof(header), -1))
		goto fail;

	if (sizeof(overallDifficulty) > BFile_Read(file, &overallDifficulty, sizeof(overallDifficulty), -1))
		goto fail;

	if (sizeof(titleLength) > BFile_Read(file, &titleLength, sizeof(titleLength), -1))
		goto fail;

	title = malloc(titleLength + 1);

	if (title == nullptr)
		goto fail;

	if (titleLength > BFile_Read(file, title, titleLength, -1))
		goto fail;

	title[titleLength] = 0;

	if (sizeof(artistLength) > BFile_Read(file, &artistLength, sizeof(artistLength), -1))
		goto fail;

	artist = malloc(artistLength + 1);

	if (artistLength > BFile_Read(file, artist, artistLength, -1))
		goto fail;

	artist[artistLength] = 0;

	if (sizeof(columnCount) > BFile_Read(file, &columnCount, sizeof(columnCount), -1))
		goto fail;

	columnSize = malloc(sizeof(*columnSize) * columnCount);

	if (columnSize == nullptr)
		goto fail;

	if (sizeof(*columnSize) * columnCount > BFile_Read(file, columnSize, (int) sizeof(*columnSize) * columnCount, -1))
		goto fail;

	if (readNotes)
	{
		int noteCount = 0;

		for (int i = 0; i < columnCount; i += 1)
			noteCount += columnSize[i];

		notes = malloc(sizeof(FXT_Note) * noteCount);

		if (notes == nullptr)
			goto fail;

		if (noteCount * sizeof(FXT_Note) > BFile_Read(file, notes, (int) sizeof(FXT_Note) * noteCount, -1))
			goto fail;
	}

	dst->Title = title;
	dst->Artist = artist;
	dst->OverallDifficulty = overallDifficulty;
	dst->ColumnCount = columnCount;
	dst->ColumnSize = columnSize;
	dst->Notes = notes;

	return 0;

fail:
	if (title != nullptr) free(title);
	if (artist != nullptr) free(artist);
	if (columnSize != nullptr) free(columnSize);
	if (notes != nullptr) free(notes);

	return FXT_BeatmapError_ReadNotesFailed;
}

static FXT_BeatmapError Beatmap_Load_BFile(FXT_Beatmap *dst, const char *path, const bool readNotes)
{
	FXT_BeatmapError error = 0;
	int file = -1;
	const auto path16 = fs_path_normalize_fc(path);

	if (path16 == nullptr)
	{
		error = FXT_BeatmapError_MallocFailed;
		goto fail;
	}

	file = BFile_Open(path16, BFile_ReadOnly);

	if (file < 0)
	{
		error = FXT_BeatmapError_FileNotFound;
		goto fail;
	}

	error = Beatmap_LoadFromFile_BFile(dst, file, readNotes);

fail:
	if (path16 != nullptr) free(path16);
	if (file >= 0) BFile_Close(file);
	return error;
}

FXT_BeatmapError FXT_Beatmap_Load_BFile(FXT_Beatmap *dst, const char *path)
{
	return Beatmap_Load_BFile(dst, path, true);
}

FXT_BeatmapError FXT_Beatmap_LoadMetadata_BFile(FXT_Beatmap *dst, const char *path)
{
	return Beatmap_Load_BFile(dst, path, false);
}

#endif
