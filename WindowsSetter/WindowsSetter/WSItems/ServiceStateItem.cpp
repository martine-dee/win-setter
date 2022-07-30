#include "ServiceStateItem.h"

ServiceStateItem::ServiceStateItem(
	const std::wstring& serviceName,
	DWORD serviceState
):serviceName(serviceName), serviceState(serviceState) {
}
