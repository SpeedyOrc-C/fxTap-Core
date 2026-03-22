#ifdef FXTAP_CORE_USE_GINT

#include <string.h>
#include <fxTap/database.h>
#include <gint/bfile.h>
#include <gint/fs.h>

FXT_DatabaseError FXT_Database_SyncFromFileSystem_BFile(FXT_Database*database)
{
	// TODO))
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
