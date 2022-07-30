#ifndef INCL_WSLIB_HELPERS
#define INCL_WSLIB_HELPERS

#include <codecvt>
#include <locale>
#include <string>
#include <atlbase.h>

class WSLibHelpers {
    public:
        static BSTR wstringToBSTR(const std::wstring& wstr) {
            BSTR newBSTR = SysAllocStringLen(wstr.data(), (UINT) wstr.size());
            return newBSTR;
        }

        static std::string wstringToString(const std::wstring& wstr)
        {
            using convert_typeX = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<convert_typeX, wchar_t> wconverter;
            return wconverter.to_bytes(wstr);
        }

        static std::wstring stringTowstring(const std::string& str) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wconverter; // TODO: don't create this every time
            return wconverter.from_bytes(str);
        }

        static BSTR stringToBSTR(const std::string& str) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wconverter; // TODO: don't create this every time
            std::wstring wstr = wconverter.from_bytes(str);
            return wstringToBSTR(wstr);
        }

        static int wstringCompareCaseInsensitive(const std::wstring& a, const std::wstring& b) {
            return _wcsnicmp(a.c_str(), b.c_str(), a.size());
        }
};

#endif