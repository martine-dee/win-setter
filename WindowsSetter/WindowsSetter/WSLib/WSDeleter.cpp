#include "WSDeleter.h"

#include <fstream>
#include "../WSLib/WSLibHelpers.h"

bool WSDeleter::deleteTarget(const std::wstring& path) {
	{
		std::ifstream fin(path);
		if (!fin) { return true; }
		fin.close();
	}

	// Delete - DeleteFile returns non-zero on success, zero on failure
	return !DeleteFile(WSLibHelpers::wstringToBSTR(path));
}