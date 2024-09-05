#ifndef FXTAP_CORE_CONFIG_CASIOWIN_H
#define FXTAP_CORE_CONFIG_CASIOWIN_H

#include <fxTap/config.h>

#define FXTAP_CONFIG_BFILE_PATH u"\\\\fls0\\FXTAP.cfg"

__attribute__ ((malloc))
Config *Config_New_LoadFromDisk_BFile(ConfigError *error);

ConfigError Config_SaveToDisk_BFile(Config *config);

#endif //FXTAP_CORE_CONFIG_CASIOWIN_H
