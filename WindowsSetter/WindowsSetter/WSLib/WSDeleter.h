#ifndef INCL_WS_DELETER
#define INCL_WS_DELETER

#include <string>

class WSDeleter {
public:
    static bool deleteTarget(const std::wstring&);
};

#endif