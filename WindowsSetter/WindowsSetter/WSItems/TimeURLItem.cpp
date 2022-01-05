#include "TimeURLItem.h"

TimeURLItem::TimeURLItem(const std::wstring& timeURL):
	timeURL(timeURL)
{}

const std::wstring& TimeURLItem::getTimeURL() {
	return timeURL;
}