#ifndef FXTAP_CORE_BFILE_BEATMAP_H
#define FXTAP_CORE_BFILE_BEATMAP_H

#include <fxTap/beatmap.h>
#include <fxTap/bfile-extern.h>

__attribute__ ((malloc))
Beatmap* Beatmap_New_LoadFromPath_BFile(const char *path, BeatmapError *error);

#endif //FXTAP_CORE_BFILE_BEATMAP_H
