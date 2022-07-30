#ifndef INCL_SERVICE_STATE_ITEM
#define INCL_SERVICE_STATE_ITEM

#include <string>
#include <windows.h>

class ServiceStateItem {

	std::wstring serviceName;
	DWORD serviceState;

public:
	ServiceStateItem(const std::wstring&, DWORD);

	// Getters
	const std::wstring& getServiceName() {
		return serviceName;
	}
	DWORD getServiceState() {
		return serviceState;
	}
};

#endif