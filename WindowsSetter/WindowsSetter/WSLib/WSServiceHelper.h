#ifndef INCL_WSSERVICEHELPER
#define INCL_WSSERVICEHELPER

#include <vector>
#include <string>
#include <windows.h>

#include "../WSItems/ServiceStateItem.h"

class WSServiceHelper {
public:
	WSServiceHelper() {}

	int SetServiceStartupType(const std::wstring&, DWORD);
	int SetServiceStartupTypeBatch(std::vector< std::pair<std::wstring, DWORD> >&);
	int SetServiceStartupTypeBatch(std::vector<ServiceStateItem*>&);
};

#endif