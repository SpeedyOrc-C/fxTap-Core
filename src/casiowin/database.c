#ifdef FXTAP_CORE_USE_GINT

#include <stb_ds.h>
#include <string.h>
#include <fxTap/database.h>
#include <gint/bfile.h>
#include <gint/fs.h>

FXT_DatabaseError FXT_Database_SyncFromFileSystem_BFile(FXT_Database *database)
{
	auto db = *database;

	int handle;
	uint16_t beatmapPath16[64] = {};
	struct BFile_FileInfo foundFile;

	auto findResult = BFile_FindFirst(
		u"\\\\fls0\\*.fxt", &handle, beatmapPath16, &foundFile);

	if (findResult != 0)
	{
		BFile_FindClose(handle);
		return 0;
	}

	do
	{
		size_t pathLength = 0;

		for (int i = 0; i < 64; i += 1)
			if (beatmapPath16[i] == 0)
			{
				pathLength = i;
				break;
			}

		if (pathLength <= 4)
			return FXT_DatabaseError_BrokenBFileSearch;

		char beatmapPath[pathLength + 1] = {};

		for (int i = 0; i < pathLength; i += 1)
			beatmapPath[i] = (char) beatmapPath16[i];

		FXT_Beatmap beatmap = {};

		if (FXT_Beatmap_LoadMetadata_BFile(&beatmap, beatmapPath))
			return FXT_DatabaseError_BrokenBFileSearch;

		if (! FXT_DatabaseRecord_IsNull(shget(db, beatmapPath)))
			return FXT_DatabaseError_BrokenBFileSearch;

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

		auto const bestGradesPath16 = fs_path_normalize_fc(bestGradesPath);
		auto const bestGradesFile = BFile_Open(bestGradesPath16, BFile_ReadOnly);
		free(bestGradesPath16);

		FXT_Grades bestGrades = {};

		if (bestGradesFile < 0)
			goto one_entry_done;

		if (sizeof(FXT_Grades) > BFile_Read(bestGradesFile, &bestGrades, sizeof(FXT_Grades), -1))
			goto one_entry_done;

		record.LastGrades.Exist = true;
		record.LastGrades.Value = bestGrades;
		BFile_Close(bestGradesFile);

	one_entry_done:
		if (bestGradesFile < 0) BFile_Close(bestGradesFile);
		shput(db, beatmapPath, record);
		findResult = BFile_FindNext(handle, beatmapPath16, &foundFile);
	}
	while (! findResult);

	BFile_FindClose(handle);
	*database = db;
	return 0;
}

static FXT_DatabaseError SaveBestGrades(const char *path, const FXT_Grades *grades)
{
	auto const path16 = fs_path_normalize_fc(path);

	if (path16 == nullptr)
		return FXT_DatabaseError_MallocFailed;

	BFile_Remove(path16);

	int size = sizeof(FXT_Grades);

	if (BFile_Create(path16, BFile_File, &size))
		return FXT_DatabaseError_CannotStartSavingGrades;

	auto const file = BFile_Open(path16, BFile_WriteOnly);

	if (file < 0)
		return FXT_DatabaseError_CannotStartSavingGrades;

	if (sizeof(FXT_Grades) > BFile_Write(file, grades, sizeof(FXT_Grades)))
	{
		BFile_Close(file);
		return FXT_DatabaseError_CannotSaveGrades;
	}

	BFile_Close(file);
	return 0;
}

FXT_DatabaseError FXT_SaveGradesAlongBeatmap_BFile(const char *beatmapPath, const FXT_Grades *grades)
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

#endif
