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
		free(db[i].value.Version);
		free(db[i].value.ColumnSize);
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
			.LastGrades = {.Exist = false},
			.Title = beatmap.Title,
			.Artist = beatmap.Artist,
			.Version = beatmap.Version,
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
		FXT_Grades bestGrades = {};

		if (bestGradesFile == nullptr)
			goto one_entry_done;

		if (! fread(&bestGrades, sizeof(FXT_Grades), 1, bestGradesFile))
			goto one_entry_done;

		record.LastGrades.Exist = true;
		record.LastGrades.Value = bestGrades;
		shput(db, beatmapPath, record);
		fclose(bestGradesFile);
		continue;

	one_entry_done:
		if (bestGradesFile != nullptr) fclose(bestGradesFile);
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

static FXT_DatabaseError SaveBestGrades(const char *path, const FXT_Grades *grades)
{
	auto const file = fopen(path, "wb");

	if (file == nullptr)
		return FXT_DatabaseError_CannotStartSavingGrades;

	if (! fwrite(grades, sizeof(FXT_Grades), 1, file))
	{
		fclose(file);
		return FXT_DatabaseError_CannotSaveGrades;
	}

	fclose(file);

	return 0;
}

FXT_DatabaseError FXT_SaveGradesAlongBeatmap(const char *beatmapPath, const FXT_Grades *grades)
{
	auto const pathLength = strlen(beatmapPath);

	char bestGradesPath[pathLength + 1] = {};
	{
		strcpy(bestGradesPath, beatmapPath);
		bestGradesPath[pathLength - 3] = 't';
		bestGradesPath[pathLength - 2] = 'b';
		bestGradesPath[pathLength - 1] = 'g';
	}

	return SaveBestGrades(bestGradesPath, grades);
}
