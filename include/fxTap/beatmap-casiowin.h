#pragma once

#include <fxTap/beatmap.h>
#include <fxTap/lib-config.h>

#ifdef FXTAP_CORE_USE_CASIOWIN

// This is the same as Beatmap_New_LoadFromPath(), but runs on old CASIO models.
[[nodiscard]]
Beatmap *Beatmap_New_LoadFromPath_BFile(const char *path, BeatmapError *error);

#endif
