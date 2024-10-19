#ifndef FXTAP_CORE_CONFIG_CASIOWIN_H
#define FXTAP_CORE_CONFIG_CASIOWIN_H

#include <fxTap/config.h>
#include <fxTap/lib-config.h>

#if defined(FXTAP_CORE_ON_GINT)

#define FXTAP_CONFIG_BFILE_PATH u"\\\\fls0\\FXTAP.cfg"

__attribute__ ((malloc))
Config *Config_New_LoadFromDisk_BFile(ConfigError *error);

ConfigError Config_SaveToDisk_BFile(Config *config);

#endif

#endif //FXTAP_CORE_CONFIG_CASIOWIN_H
