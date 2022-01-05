#include "TaskSchedulerItemPartial.h"

using namespace std;

TaskSchedulerItemPartial::TaskSchedulerItemPartial(const wstring& folderPath, const wstring& taskName, bool state):
    TaskSchedulerItem(folderPath, taskName, state)
{}

bool TaskSchedulerItemPartial::Matches(const wstring& folderPath, const wstring& taskName) {
    return folderPath == this->folderPath && taskName.rfind(this->taskName, 0) == 0;
}