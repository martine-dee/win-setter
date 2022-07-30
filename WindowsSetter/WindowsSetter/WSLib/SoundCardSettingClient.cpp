#include "SoundCardSettingClient.h"

#include <atlbase.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>

#pragma comment(lib, "Winmm.lib")

using namespace std;

SoundCardSettingClient::SoundCardSettingClient() {}

bool SoundCardSettingClient::GetSoundCardNames(std::vector<std::wstring>& foundNames) {
    foundNames.clear();
    int nSoundCardCount = waveOutGetNumDevs();

    bool found = false;
    for (int i = 0; i < nSoundCardCount; i++) {
        WAVEOUTCAPS woc;
        waveOutGetDevCaps(i, &woc, sizeof(woc));
        wstring foundName = woc.szPname;
        foundNames.push_back(foundName);
    }

    return false;
}
