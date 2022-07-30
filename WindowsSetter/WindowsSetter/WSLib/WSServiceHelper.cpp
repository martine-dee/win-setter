#include "WSServiceHelper.h"
#include "WSLibHelpers.h"

#include <ostream>
#include <string>
extern std::wostream* debugOutput;

using namespace std;

#include <windows.h>

int WSServiceHelper::SetServiceStartupTypeBatch(std::vector<ServiceStateItem*>& serviceStateItems) {
	vector<pair<wstring, DWORD>> serviceChanges;
	for (int i = 0; i < serviceStateItems.size(); i++) {
		wstring serviceName = serviceStateItems[i]->getServiceName();
		DWORD serviceState = serviceStateItems[i]->getServiceState();
		// Windows
		serviceChanges.push_back(make_pair(WSLibHelpers::wstringToBSTR(serviceName), serviceState));
	}
	return this->SetServiceStartupTypeBatch(serviceChanges);
}

int WSServiceHelper::SetServiceStartupTypeBatch(vector<pair<wstring, DWORD>>& batch) {

	SC_HANDLE hSCManager = OpenSCManager(
		NULL,					// local machine
		NULL,					// SERVICES_ACTIVE_DATABASE database is opened by default
		SC_MANAGER_ALL_ACCESS	// full access rights
	);
	if (!hSCManager) {
		(*debugOutput) << L"Couldn't obtain the SC Manager!" << endl;
		return 1;
	}

	SC_LOCK serviceDatabaseLock = LockServiceDatabase(hSCManager);
	if (!serviceDatabaseLock) {
		(*debugOutput) << L"Couldn't lock the service database!" << endl;
		return 1;
	}

	for (int i = 0; i < batch.size(); i++) {
		pair<wstring, DWORD> pair = batch[i];
		wstring& serviceName = pair.first;
		DWORD targetStartType = pair.second;

		(*debugOutput) << "============================================" << endl;
		(*debugOutput) << "Working on service: '" << serviceName << "'" << endl;

		if (targetStartType != SERVICE_BOOT_START
			&& targetStartType != SERVICE_SYSTEM_START
			&& targetStartType != SERVICE_AUTO_START
			&& targetStartType != SERVICE_DEMAND_START
			&& targetStartType != SERVICE_DISABLED
		) {			
			(*debugOutput) << L"Received invalid targetStarttype='"<< targetStartType << L"' for '" << serviceName << L"'." << endl;
			continue; // Invalid type
		}

		// TODO: check first if the service needs an update

		try {

			SC_HANDLE schService = OpenService(
				hSCManager,									// SCManager database
				WSLibHelpers::wstringToBSTR(serviceName),	// name of service
				SERVICE_CHANGE_CONFIG						// need CHANGE access
			);

			if (!schService) {
				(*debugOutput) << L"Couldn't open the service '" << serviceName << L"' for updating! Error: " << GetLastError() << endl;
				continue; // Couldn't open the service for editing
			}

			if (!ChangeServiceConfig(
				schService,			// handle of service
				SERVICE_NO_CHANGE,	// service type: no change
				targetStartType,	// dwStartType, change service startup type to auto
				SERVICE_NO_CHANGE,	// error control: no change
				NULL,				// binary path: no change
				NULL,				// load order group: no change
				NULL,				// tag ID: no change
				NULL,				// dependencies: no change
				NULL,				// account name: no change
				NULL,				// password: no change
				NULL)				// display name: no change 
			) {
				(*debugOutput) << L"Couldn't update the service '" << serviceName << L"'! Error: " << GetLastError() << endl;
			}
			else {
				(*debugOutput) << L"Target start type '" << targetStartType << L"' was set successfully for '" << serviceName << L"'" << endl;
			}

			// Close the handle to the service.
			if (CloseServiceHandle(schService) == 0) {
				(*debugOutput) << L"Couldn't close the service handle for '" << serviceName << L"'! Error: " << GetLastError() << endl;
			} // Else fine
		}
		catch (...) {
			(*debugOutput) << L"Unexpected issue occured while setting '" << serviceName << L"'! Error: " << GetLastError() << endl;
		}
	}

	if (UnlockServiceDatabase(serviceDatabaseLock) == 0) {
		(*debugOutput) << L"Couldn't close the service databse lock! Error: " << GetLastError() << endl;
	} // Else fine

	return 0;
}

int WSServiceHelper::SetServiceStartupType(const wstring& serviceName, DWORD targetStartType) {
	// Validate the input
	if (targetStartType != SERVICE_BOOT_START
		&& targetStartType != SERVICE_SYSTEM_START
		&& targetStartType != SERVICE_AUTO_START
		&& targetStartType != SERVICE_DEMAND_START
		&& targetStartType != SERVICE_DISABLED
	) {
		// Invalid type
		return 1; 
	}

	SC_HANDLE hSCManager = OpenSCManager(
		NULL,					// local machine
		NULL,					// SERVICES_ACTIVE_DATABASE database is opened by default
		SC_MANAGER_ALL_ACCESS	// full access rights
	);
	if (!hSCManager) { return 1; }

	SC_LOCK serviceDatabaseLock = LockServiceDatabase(hSCManager);
	if (!serviceDatabaseLock) { return 1; }

	SC_HANDLE schService = OpenService(
		hSCManager,									// SCManager database
		WSLibHelpers::wstringToBSTR(serviceName),	// name of service
		SERVICE_CHANGE_CONFIG						// need CHANGE access
	);
	if (!schService) {
		UnlockServiceDatabase(serviceDatabaseLock);
		return 1;
	}

	if (!ChangeServiceConfig(
		schService,			// handle of service
		SERVICE_NO_CHANGE,	// service type: no change
		targetStartType,	// dwStartType, change service startup type to auto
		SERVICE_NO_CHANGE,	// error control: no change
		NULL,				// binary path: no change
		NULL,				// load order group: no change
		NULL,				// tag ID: no change
		NULL,				// dependencies: no change
		NULL,				// account name: no change
		NULL,				// password: no change
		NULL)				// display name: no change 
	) {
		// Failed!
	}

	/*
	* 	sdBuf.lpDescription = (LPWSTR)lpDesc;

	if (!ChangeServiceConfig2(
		schService, // handle to service
		SERVICE_CONFIG_DESCRIPTION, // change: description
		&sdBuf)) // value: new description
	{
		printf("ChangeServiceConfig2() failed, error: %d.\n", GetLastError());
	}
	else
		printf("ChangeServiceConfig2() looks OK, service description changed.\n");
	*/
	
	if (UnlockServiceDatabase(serviceDatabaseLock) == 0) {
		// Error: GetLastError()
	} // Else fine


	// Close the handle to the service.
	if (CloseServiceHandle(schService) == 0) {
		// Error: GetLastError()
	} // Else fine

	return 0;
}