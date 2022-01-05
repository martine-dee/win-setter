#ifndef INCL_WINDOWS_SETTER_SETTINGS_LOADER
#define INCL_WINDOWS_SETTER_SETTINGS_LOADER

#include "WindowsSetter.h"

class SettingsLoader {
    public:
        static bool LoadSettings(const char*, WindowsSetter&);
};

#endif