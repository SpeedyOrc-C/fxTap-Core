#include <stdio.h>
#include <stdlib.h>
#include <fxTap/beatmap.h>

#ifdef FXTAP_CORE_HAS_DIRENT
#include <dirent.h>
#endif

static constexpr auto BASE_TOLERANCE_PERFECT = 16;
static constexpr auto BASE_TOLERANCE_GREAT = 64;
static constexpr auto BASE_TOLERANCE_GOOD = 97;
static constexpr auto BASE_TOLERANCE_OK = 127;
static constexpr auto BASE_TOLERANCE_MEH = 151;
static constexpr auto BASE_TOLERANCE_MISS = 188;

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

int FXT_Beatmap_NoteCount(const FXT_Beatmap *beatmap)
{
	int count = 0;

	for (int column = 0; column < FXT_MaxColumnCount; column += 1)
		count += beatmap->ColumnSize[column];

	return count;
}

FXT_BeatmapError Beatmap_LoadFromFile(FXT_Beatmap *dst, FILE *file)
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

	if (!fread(header, sizeof(header), 1, file))
		goto fail;

	if (!fread(&overallDifficulty, sizeof(overallDifficulty), 1, file))
		goto fail;

	if (!fread(&titleLength, sizeof(titleLength), 1, file))
		goto fail;

	title = malloc(titleLength + 1);

	if (title == nullptr)
		goto fail;

	if (!fread(title, titleLength, 1, file))
		goto fail;

	title[titleLength] = 0;

	if (!fread(&artistLength, sizeof(artistLength), 1, file))
		goto fail;

	artist = malloc(artistLength + 1);

	if (!fread(artist, artistLength, 1, file))
		goto fail;

	artist[artistLength] = 0;

	if (!fread(&columnCount, sizeof(columnCount), 1, file))
		goto fail;

	sizeOfColumn = malloc(2 * columnCount);

	if (sizeOfColumn == nullptr)
		goto fail;

	if (!fread(sizeOfColumn, 2 * columnCount, 1, file))
		goto fail;

	int noteCount = 0;

	for (int i = 0; i < columnCount; i += 1)
		noteCount += sizeOfColumn[i];

	notes = malloc(sizeof(FXT_Note) * noteCount);

	if (notes == nullptr)
		goto fail;

	if (fread(notes, sizeof(FXT_Note), noteCount, file) < noteCount)
		goto fail;

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

FXT_BeatmapError FXT_Beatmap_Load(FXT_Beatmap *dst, const char *path)
{
	FXT_BeatmapError error = 0;

	FILE *file = fopen(path, "rb");

	if (file == nullptr)
		return FXT_BeatmapError_FileNotFound;

	error = Beatmap_LoadFromFile(dst, file);
	fclose(file);

	return error;
}

void FXT_Beatmap_FreeInner(const FXT_Beatmap *beatmap)
{
	free(beatmap->Title);
	free(beatmap->Artist);
	free(beatmap->ColumnSize);
	free(beatmap->Notes);
}

void FXT_Beatmap_Free(FXT_Beatmap *beatmap)
{
	FXT_Beatmap_FreeInner(beatmap);
	free(beatmap);
}
