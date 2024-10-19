#ifndef FXTAP_CORE_LIB_CONFIG_H
#define FXTAP_CORE_LIB_CONFIG_H

#if !defined(FXTAP_CORE_ON_GINT)      & \
    !defined(FXTAP_CORE_ON_ARDUINO)   & \
    !defined(FXTAP_CORE_ON_MODERN_OS)
    #warning Platform for fxTap Core not selected.
#endif

#if defined(FXTAP_CORE_ON_GINT) | defined(FXTAP_CORE_ON_MODERN_OS)
    #define FXTAP_CORE_HAS_DIRENT
#endif

#endif //FXTAP_CORE_LIB_CONFIG_H
