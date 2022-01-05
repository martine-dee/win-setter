#ifndef INCL_SOUND_CARD_SETTING_CLIENT
#define INCL_SOUND_CARD_SETTING_CLIENT

#include <string>
#include <vector>

class SoundCardSettingClient {
    public:
        SoundCardSettingClient();

        bool GetSoundCardNames(std::vector<std::wstring>&);
};

#endif