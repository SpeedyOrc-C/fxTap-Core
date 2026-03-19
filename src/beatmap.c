#include <stdio.h>
#include <stdlib.h>
#include <fxTap/beatmap.h>


FXT_Tolerance FXT_Tolerance_FromOverallDifficulty(const double overallDifficulty)
{
	static constexpr int16_t BaseTolerancePerfect = 16;
	static constexpr int16_t BaseToleranceGreat = 64;
	static constexpr int16_t BaseToleranceGood = 97;
	static constexpr int16_t BaseToleranceOk = 127;
	static constexpr int16_t BaseToleranceMeh = 151;
	static constexpr int16_t BaseToleranceMiss = 188;

	const double od3 = 3 * overallDifficulty;

	return (FXT_Tolerance){
		.Perfect = BaseTolerancePerfect,
		.Great = (int16_t) ((double) BaseToleranceGreat - od3),
		.Good = (int16_t) ((double) BaseToleranceGood - od3),
		.Ok = (int16_t) ((double) BaseToleranceOk - od3),
		.Meh = (int16_t) ((double) BaseToleranceMeh - od3),
		.Miss = (int16_t) ((double) BaseToleranceMiss - od3),
	};
}

float FXT_Grades_ScoreV1(const FXT_Grades grades)
{
	const float max = grades.Perfect;
	const float n300 = grades.Great;
	const float n200 = grades.Good;
	const float n100 = grades.Ok;
	const float n50 = grades.Meh;
	const float miss = grades.Miss;

	return (300 * (max + n300) + 200 * n200 + 100 * n100 + 50 * n50)
	       / (300 * (max + n300 + n200 + n100 + n50 + miss));
}

float FXT_Grades_ScoreV2(const FXT_Grades grades)
{
	const float max = grades.Perfect;
	const float n300 = grades.Great;
	const float n200 = grades.Good;
	const float n100 = grades.Ok;
	const float n50 = grades.Meh;
	const float miss = grades.Miss;

	return (305 * max + 300 * n300 + 200 * n200 + 100 * n100 + 50 * n50)
	       / (305 * (max + n300 + n200 + n100 + n50 + miss));
}

int FXT_Beatmap_NoteCount(const FXT_Beatmap *beatmap)
{
	int count = 0;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
		count += beatmap->ColumnSize[column];

	return count;
}

FXT_BeatmapError Beatmap_LoadFromFile(FXT_Beatmap *dst, FILE *file, const bool readNotes)
{
	char header[8];
	uint8_t titleLength;
	char *title = nullptr;
	uint8_t artistLength;
	char *artist = nullptr;
	double overallDifficulty;
	uint8_t columnCount;
	uint16_t *sizeOfColumn = nullptr;
	FXT_Note *notes = nullptr;

	if (! fread(header, sizeof(header), 1, file))
		goto fail;

	if (! fread(&overallDifficulty, sizeof(overallDifficulty), 1, file))
		goto fail;

	if (! fread(&titleLength, sizeof(titleLength), 1, file))
		goto fail;

	title = malloc(titleLength + 1);

	if (title == nullptr)
		goto fail;

	if (! fread(title, titleLength, 1, file))
		goto fail;

	title[titleLength] = 0;

	if (! fread(&artistLength, sizeof(artistLength), 1, file))
		goto fail;

	artist = malloc(artistLength + 1);

	if (! fread(artist, artistLength, 1, file))
		goto fail;

	artist[artistLength] = 0;

	if (! fread(&columnCount, sizeof(columnCount), 1, file))
		goto fail;

	sizeOfColumn = malloc(2 * columnCount);

	if (sizeOfColumn == nullptr)
		goto fail;

	if (! fread(sizeOfColumn, 2 * columnCount, 1, file))
		goto fail;

	if (readNotes)
	{
		int noteCount = 0;

		for (int i = 0; i < columnCount; i += 1)
			noteCount += sizeOfColumn[i];

		notes = malloc(sizeof(FXT_Note) * noteCount);

		if (notes == nullptr)
			goto fail;

		if (fread(notes, sizeof(FXT_Note), noteCount, file) < noteCount)
			goto fail;
	}

	dst->Title = title;
	dst->Artist = artist;
	dst->OverallDifficulty = overallDifficulty;
	dst->ColumnCount = columnCount;
	dst->ColumnSize = sizeOfColumn;
	dst->Notes = notes;

	return 0;

fail:
	if (title != nullptr) free(title);
	if (artist != nullptr) free(artist);
	if (sizeOfColumn != nullptr) free(sizeOfColumn);
	if (notes != nullptr) free(notes);

	return FXT_BeatmapError_ReadNotesFailed;
}

FXT_BeatmapError Beatmap_Load(FXT_Beatmap *dst, const char *path, const bool readNotes)
{
	FXT_BeatmapError error = 0;

	FILE *file = fopen(path, "rb");

	if (file == nullptr)
		return FXT_BeatmapError_FileNotFound;

	error = Beatmap_LoadFromFile(dst, file, readNotes);
	fclose(file);

	return error;
}

FXT_BeatmapError FXT_Beatmap_Load(FXT_Beatmap *dst, const char *path)
{
	return Beatmap_Load(dst, path, true);
}

FXT_BeatmapError FXT_Beatmap_LoadMetadata(FXT_Beatmap *dst, const char *path)
{
	return Beatmap_Load(dst, path, false);
}

void FXT_Beatmap_FreeInner(const FXT_Beatmap *beatmap)
{
	free(beatmap->Title);
	free(beatmap->Artist);
	free(beatmap->ColumnSize);
	if (beatmap->Notes != nullptr)
		free(beatmap->Notes);
}

void FXT_Beatmap_Free(FXT_Beatmap *beatmap)
{
	FXT_Beatmap_FreeInner(beatmap);
	free(beatmap);
}
