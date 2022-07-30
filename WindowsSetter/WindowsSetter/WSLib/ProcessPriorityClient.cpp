#include "ProcessPriorityClient.h"

#include <atlbase.h>
#include <ostream>
#include "WSLibHelpers.h"

using namespace std;

extern std::wostream* debugOutput;

// An empty ctor
ProcessPriorityClient::ProcessPriorityClient() {}

bool ProcessPriorityClient::GetPriority(const wstring& executableName, int& executablePriority) {
    wstring keyPath = this->KeyPathFromExecutableName(executableName);
    BSTR keyPathBSTR = WSLibHelpers::wstringToBSTR(keyPath);

    HKEY hKey = NULL;
    LONG regOpenKeyResult = ERROR_SUCCESS;
    regOpenKeyResult = RegOpenKey(HKEY_LOCAL_MACHINE, keyPathBSTR, &hKey);

    if (regOpenKeyResult == ERROR_SUCCESS) {
        DWORD currentValue;
        DWORD dwBufferSize(sizeof(DWORD));

        LONG regQueryValueResult = RegQueryValueExW(
            hKey,
            L"CpuPriorityClass",
            0,
            NULL,
            reinterpret_cast<LPBYTE>(&currentValue),
            &dwBufferSize
        );

        if (ERROR_SUCCESS == regQueryValueResult) {
            executablePriority = (int)currentValue;
            (*debugOutput) << L"Obtained priority '" << executablePriority << L"' for '" << executableName << L"'" << endl;
        }
        else {
            (*debugOutput) << L"Couldn't obtain the priority value for '" << executableName << L"'" << endl;
            return true;
        }
        RegCloseKey(hKey);
    }
    else {
        *debugOutput << L"Couldn't obtain the priority key for '" << executableName << L"'" << endl;
        return true;
    }

    return false;
}

bool ProcessPriorityClient::SetPriority(const std::wstring& executableName, int priority) {
    // Only accept 1 = low, 2 = normal, and 3 = high
    if (priority != 3 && priority != 2 && priority != 1) { return true; }

    // The executable name is on you
    // TODO: check executableName

    wstring keyPath = this->KeyPathFromExecutableName(executableName);
    BSTR keyPathBSTR = WSLibHelpers::wstringToBSTR(keyPath);

    HKEY hKey = NULL;
    LONG regOpenKeyResult = ERROR_SUCCESS;
    regOpenKeyResult = RegOpenKey(HKEY_LOCAL_MACHINE, keyPathBSTR, &hKey);

    // Key doesn't exist, create
    if (regOpenKeyResult != ERROR_SUCCESS) {
        regOpenKeyResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyPathBSTR, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
        if (regOpenKeyResult != ERROR_SUCCESS) {
            // Something went wrong; can't create the key
            (*debugOutput) << L"Couldn't create priority key for '" << executableName << L"'" << endl;
            return true;
        }
    }

    // Key exists, update
    if (regOpenKeyResult == ERROR_SUCCESS) {
        DWORD dwordVal = (DWORD) priority;
        LONG regSetValueResult = RegSetValueEx(hKey, L"CpuPriorityClass", 0, REG_DWORD, (const BYTE*) &dwordVal, sizeof(dwordVal));
        RegCloseKey(hKey);

        if (regSetValueResult == ERROR_SUCCESS) {
            (*debugOutput) << L"Successfully set priority '" << priority << L"' for '" << executableName << L"'" << endl;
            return false;
        }
        else {
            (*debugOutput) << L"Couldn't set priority '" << priority << L"' for '" << executableName << L"'" << endl;
            return true;
        }
    }
    else {
        (*debugOutput) << L"Couldn't obtain priority key for '" << executableName << L"'" << endl;
        return true;
    }

    return false;
}

wstring ProcessPriorityClient::KeyPathFromExecutableName(const wstring& executableName) {
    wstring keyPath = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\" + executableName + L"\\PerfOptions";
    return keyPath;
}

bool ProcessPriorityClient::SetCpuPriority(const vector<CpuPriorityItem*>& cpuPriorityItems) {
    for (unsigned int i = 0; i < cpuPriorityItems.size(); i++) {

        wstring name = cpuPriorityItems[i]->GetName();
        int wantedPriority = cpuPriorityItems[i]->GetPriority();

        (*debugOutput) << "============================================" << endl;
        (*debugOutput) << L"Working on '" << name << L"'" << endl;

        int presentPriority;
        bool obtainingPrioritySuccessful = !this->GetPriority(name, presentPriority);

        if (!obtainingPrioritySuccessful || (wantedPriority != presentPriority)) {
            // The priority needs modifying
            bool setPriorityOutcome = this->SetPriority(name, wantedPriority); // false = all fine, true = not good
            // setPriorityOutcome - false = ok; true = not ok
            if (setPriorityOutcome) {
                (*debugOutput) << L"Setting the priority failed." << endl;
            }
            else {
                (*debugOutput) << L"Setting the priority succeeded." << endl;
            }
        }
        else {
            // The priority is already fine
            (*debugOutput) << L"The priority was already fine." << endl;
        }
    }
    
    return false;
}