#include "TaskSchedulerItemFull.h"

using namespace std;

TaskSchedulerItemFull::TaskSchedulerItemFull(const wstring& folderPath, const wstring& taskName, bool state) :
    TaskSchedulerItem(folderPath, taskName, state)
{}

bool TaskSchedulerItemFull::Matches(const wstring& folderPath, const wstring& taskName) {
    return folderPath == this->folderPath && taskName == this->taskName;
}
