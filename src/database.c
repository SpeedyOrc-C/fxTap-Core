#include <dirent.h>
#include <stb_ds.h>
#include <stdio.h>
#include <fxTap/database.h>

void FXT_Database_Init(FXT_Database *dst)
{
	*dst = nullptr;
	sh_new_strdup(*dst);
	shdefault(*dst, FXT_DatabaseRecord_Null);
}

void FXT_Database_FreeInner(FXT_Database *database)
{
	auto const db = *database;

	for (size_t i = 0; i < shlenu(db); i += 1)
	{
		free(db[i].value.Title);
		free(db[i].value.Artist);
		free(db[i].value.ColumnSize);
		free(db[i].value.LastGrades);
	}

	*database = db;
}

void FXT_Database_Free(FXT_Database *database)
{
	hmfree(*database);
}

FXT_DatabaseError FXT_Database_SyncFromFileSystem(FXT_Database *database)
{
	auto db = *database;

	auto const dir = opendir(".");

	if (dir == nullptr)
		return FXT_DatabaseError_CannotOpenDir;

	while (true)
	{
		auto const dirent = readdir(dir);

		// Reached the end
		if (dirent == nullptr)
			break;

		auto const pathLength = strlen(dirent->d_name);

		// Doesn't have file extension
		if (pathLength <= 4)
			continue;

		// Found a beatmap
		if (strcmp(dirent->d_name + pathLength - 4, ".fxt") != 0)
			continue;

		auto const beatmapPath = dirent->d_name;

		FXT_Beatmap beatmap = {};

		if (FXT_Beatmap_LoadMetadata(&beatmap, beatmapPath))
			continue;

		if (! FXT_DatabaseRecord_IsNull(shget(db, beatmapPath)))
			continue;

		auto record = (FXT_DatabaseRecord){
			.LastGrades = nullptr,
			.Title = beatmap.Title,
			.Artist = beatmap.Artist,
			.OverallDifficulty = beatmap.OverallDifficulty,
			.ColumnCount = beatmap.ColumnCount,
			.ColumnSize = beatmap.ColumnSize,
		};

		// Also check .tbg file for player's best grade
		char bestGradesPath[pathLength + 1] = {};
		{
			strcpy(bestGradesPath, beatmapPath);
			bestGradesPath[pathLength - 3] = 't';
			bestGradesPath[pathLength - 2] = 'b';
			bestGradesPath[pathLength - 1] = 'g';
		}

		auto const bestGradesFile = fopen(bestGradesPath, "rb");
		FXT_Grades *bestGrades = nullptr;

		if (bestGradesFile == nullptr)
			goto one_entry_done;

		bestGrades = malloc(sizeof(FXT_Grades));

		if (bestGrades == nullptr)
			goto one_entry_done;

		if (! fread(bestGrades, sizeof(FXT_Grades), 1, bestGradesFile))
			goto one_entry_done;

		record.LastGrades = bestGrades;
		shput(db, beatmapPath, record);
		fclose(bestGradesFile);
		continue;

	one_entry_done:
		if (bestGradesFile != nullptr) fclose(bestGradesFile);
		if (bestGrades != nullptr) free(bestGrades);
		shput(db, beatmapPath, record);
	}

	closedir(dir);
	*database = db;
	return 0;
}

bool FXT_DatabaseRecord_IsNull(const FXT_DatabaseRecord record)
{
	return record.Title == nullptr;
}

int FXT_Database_Compare(const struct FXT_Database *a, const struct FXT_Database *b)
{
	return strcmp(a->value.Title, b->value.Title);
}

int FXT_Database_Compare_Reverse(const struct FXT_Database *a, const struct FXT_Database *b)
{
	return strcmp(b->value.Title, a->value.Title);
}

int FXT_Database_Compare_Void(const void *a, const void *b)
{
	return FXT_Database_Compare(*(void **) a, *(void **) b);
}

int FXT_Database_Compare_Reverse_Void(const void *a, const void *b)
{
	return FXT_Database_Compare_Reverse(*(void **) a, *(void **) b);
}
