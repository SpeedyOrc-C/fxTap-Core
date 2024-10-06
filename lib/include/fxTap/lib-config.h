#ifndef FXTAP_CORE_LIB_CONFIG_H
#define FXTAP_CORE_LIB_CONFIG_H

// It's very likely that this library will be compiled on an embedded system, not PC,
// since there are already tons of rhythm games on PC.
// To make sure this library compiles on as many platforms as possible,
// if your platform doesn't support any header files, please disable them below.

// If you're not sure, pick ONLY ONE macro below, and we'll get it sorted.

#define FXTAP_CORE_AUTO_DETECT_ENVIRONMENT
//#define FXTAP_CORE_ON_GINT
//#define FXTAP_CORE_ON_ARDUINO
//#define FXTAP_CORE_ON_MODERN_OS

// Please DO NOT touch the code below.

#if defined(FXTAP_CORE_AUTO_DETECT_ENVIRONMENT)

    #if defined(_WIN32) | defined(_WIN64) | defined(__APPLE__) | defined(__linux__)
        #define FXTAP_CORE_ON_MODERN_OS
    #endif

    #if defined(FX9860G) | defined(FXCG50)
        #define FXTAP_CORE_ON_GINT
    #endif

#endif

#if defined(FXTAP_CORE_ON_GINT) | defined(FXTAP_CORE_ON_MODERN_OS)
    #define FXTAP_CORE_HAS_DIRENT
#endif

#endif //FXTAP_CORE_LIB_CONFIG_H
