#include "WSTimeSetter.h"

#pragma comment(lib,"wininet.lib")

#include <regex>
#include <iostream>
#include <Windows.h>
#include <wininet.h>
#include <cstring>
#include <atlbase.h>

#include "WSLibHelpers.h"

extern std::wostream* debugOutput;

using namespace std;

int WSTimeSetter::SetTime(const wstring& timeURL, const wstring& timeField) {
    HINTERNET connect = InternetOpen(L"MyBrowser", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    if (!connect) {
        (*debugOutput) << "Connection Failed or Syntax error";
        return 0;
    }

    HINTERNET OpenAddress = InternetOpenUrl(
        connect,
        WSLibHelpers::wstringToBSTR(timeURL),
        NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0
    );

    if (!OpenAddress) {
        DWORD ErrorNum = GetLastError();
        (*debugOutput) << "Failed to open URL \nError No: " << ErrorNum;
        InternetCloseHandle(connect);
        return 0;
    }

    char DataReceived[4096];
    DWORD NumberOfBytesRead = 0;
    InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead);
    InternetCloseHandle(OpenAddress);
    InternetCloseHandle(connect);
    
    if(NumberOfBytesRead) {
        DataReceived[NumberOfBytesRead] = 0;

        string timeFieldString = WSLibHelpers::wstringToString(timeField);
        regex ParseDateAndTime((timeFieldString + ":\\s+(\\d{4})[^\\d]+(\\d{2})[^\\d]{1}(\\d{2})T(\\d{2}):(\\d{2}):(\\d{2}).+").c_str());

        string str = DataReceived;

        size_t pos1 = str.find((timeFieldString + ":").c_str());
        size_t pos2 = str.find("\n", pos1);
        str = str.substr(pos1, pos2 - pos1);

        smatch parsedDateAndTime;
        if (regex_match(str, parsedDateAndTime, ParseDateAndTime)) {
            if (parsedDateAndTime.size() == 7) {
                string year = parsedDateAndTime[1].str();
                string month = parsedDateAndTime[2].str();
                string day = parsedDateAndTime[3].str();
                string hour = parsedDateAndTime[4].str();
                string minute = parsedDateAndTime[5].str();
                string second = parsedDateAndTime[6].str();

                SYSTEMTIME st;
                st.wYear = std::stoi(year);
                st.wMonth = std::stoi(month);
                st.wDay = std::stoi(day);
                st.wHour = std::stoi(hour);
                st.wMinute = std::stoi(minute);
                st.wSecond = std::stoi(second);
                st.wMilliseconds = 999;
                ::SetSystemTime(&st);
            }
        }
        else {
            // TODO: (*debugOutput)
        }
    }

    return 0;
}





