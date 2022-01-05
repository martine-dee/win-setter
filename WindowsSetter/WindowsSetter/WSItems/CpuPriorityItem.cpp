#include "CpuPriorityItem.h"

using namespace std;

CpuPriorityItem::CpuPriorityItem(const std::wstring& name, const int& priority) :
    name(name), priority(priority)
{}

wstring CpuPriorityItem::GetName() { return name; }
int CpuPriorityItem::GetPriority() { return priority; }