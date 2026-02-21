#pragma once

#if defined(FX9860G) || defined(FXCG50) || defined(__linux__) || defined(_WIN64)
#define FXTAP_CORE_HAS_DIRENT
#endif

#if defined(FX9860G) && !defined(FXTAP_CORE_USE_CASIOWIN)
#define FXTAP_CORE_USE_CASIOWIN
#endif
