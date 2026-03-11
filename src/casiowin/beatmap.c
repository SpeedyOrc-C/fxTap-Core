#ifdef FXTAP_CORE_USE_CASIOWIN

#include <stdlib.h>
#include <fxTap/beatmap.h>
#include <fxTap/bfile-interface.h>

FXT_BeatmapError Beatmap_LoadFromBFile(FXT_Beatmap *dst, const int file)
{
	return FXT_BeatmapError_ReadNotesFailed;
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
