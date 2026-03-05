#include <dirent.h>
#include <stb_ds.h>
#include <fxTap/database.h>

static auto const DatabasePath = "fxTap.db";

void FXT_Database_Init(FXT_Database *dst)
{
	*dst = nullptr;
	sh_new_strdup(*dst);
	shdefault(*dst, FXT_DatabaseRecord_Null);
}

FXT_DatabaseError FXT_Database_Load(FXT_Database *dst)
{
	auto _dst = *dst;
	*dst = _dst;
	return 0;
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
	// TODO)) Removing deleted beatmaps

	// Add new beatmaps
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

		// Not a beatmap
		auto const pathLength = strlen(dirent->d_name);
		if (pathLength <= 4 || strcmp(dirent->d_name + pathLength - 4, ".fxt") != 0)
			continue;

		auto const path = dirent->d_name;

		FXT_Beatmap beatmap = {};

		if (FXT_Beatmap_LoadMetadata(&beatmap, path))
			continue;

		if (FXT_DatabaseRecord_IsNull(shget(db, path)))
		{
			auto const record = (FXT_DatabaseRecord){
				.LastGrades = nullptr,
				.Title = beatmap.Title,
				.Artist = beatmap.Artist,
				.OverallDifficulty = beatmap.OverallDifficulty,
				.ColumnCount = beatmap.ColumnCount,
				.ColumnSize = beatmap.ColumnSize,
			};

			shput(db, path, record);
		}
		else
		{
			auto const oldIndex = shgeti(db, path);

			db[oldIndex].value.Title = beatmap.Title;
			db[oldIndex].value.Artist = beatmap.Artist;
			db[oldIndex].value.OverallDifficulty = beatmap.OverallDifficulty;
			db[oldIndex].value.ColumnCount = beatmap.ColumnCount;
			db[oldIndex].value.ColumnSize = beatmap.ColumnSize;
		}
	}

	closedir(dir);
	*database = db;
	return 0;
}

bool FXT_DatabaseRecord_IsNull(const FXT_DatabaseRecord record)
{
	return record.Title == nullptr;
}
