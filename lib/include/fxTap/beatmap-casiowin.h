#ifndef FXTAP_CORE_BEATMAP_CASIOWIN_H
#define FXTAP_CORE_BEATMAP_CASIOWIN_H

#include <fxTap/beatmap.h>

// This is the same as Beatmap_New_LoadFromPath(), but runs on old CASIO models.
__attribute__ ((malloc))
Beatmap* Beatmap_New_LoadFromPath_BFile(const char *path, BeatmapError *error);

#endif //FXTAP_CORE_BEATMAP_CASIOWIN_H
