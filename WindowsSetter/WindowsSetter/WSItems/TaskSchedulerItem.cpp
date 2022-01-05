#include "TaskSchedulerItem.h"

using namespace std;

TaskSchedulerItem::TaskSchedulerItem(const wstring& folderPath, const wstring& taskName, bool state):
    folderPath(folderPath),
    taskName(taskName),
    state(state)
{}

wstring TaskSchedulerItem::GetFolderPath() {
    return folderPath;
}

wstring TaskSchedulerItem::GetTaskName() {
    return taskName;
}

bool TaskSchedulerItem::GetState() {
    return state;
}