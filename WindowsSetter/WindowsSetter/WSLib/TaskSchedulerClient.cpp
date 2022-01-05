#include "TaskSchedulerClient.h"

#include <windows.h>
#include <ostream>
#include <vector>
#include <regex>
using namespace std;

#pragma comment(lib, "taskschd.lib")

extern wostream* debugOutput;

TaskSchedulerClient::TaskSchedulerClient() {}

bool TaskSchedulerClient::SetTasks(const std::vector<TaskSchedulerItem*>& taskSchedulerItems) {
    HRESULT hr = CoInitialize(NULL);
    {
        CComPtr<ITaskService> taskService;
        CComPtr<ITaskFolder> taskFolder;
        hr = taskService.CoCreateInstance(CLSID_TaskScheduler);
        taskService->Connect(CComVariant(), CComVariant(), CComVariant(), CComVariant());
        taskService->GetFolder(CComBSTR(L"\\"), &taskFolder);
        this->SetTasks(taskSchedulerItems, taskFolder);
    }
    CoUninitialize();

    return FAILED(hr) ? true : false;
}

bool TaskSchedulerClient::SetTasks(const std::vector<TaskSchedulerItem*>& taskSchedulerItems, ITaskFolder* taskFolder) {

    BSTR folderPathBSTR;
    taskFolder->get_Path(&folderPathBSTR);
    std::wstring folderPath(folderPathBSTR, SysStringLen(folderPathBSTR));

    CComPtr<IRegisteredTaskCollection> tasks;
    CComPtr<ITaskFolderCollection> children;
    long count;

    taskFolder->GetTasks(TASK_ENUM_HIDDEN, &tasks);
    tasks->get_Count(&count);

    for (long i = 1; i < (count + 1); i++) {
        CComPtr<IRegisteredTask> task;
        tasks->get_Item(CComVariant(i), &task);
        this->SetTask(taskSchedulerItems, task, folderPath);
    }

    // Get the subfolders
    taskFolder->GetFolders(0, &children);
    children->get_Count(&count);

    for (long i = 1; i < (count + 1); i++) {
        CComPtr<ITaskFolder> child;
        children->get_Item(CComVariant(i), &child);

        // Process the child
        this->SetTasks(taskSchedulerItems, child);
    }

    return false;
}

void TaskSchedulerClient::SetTask(const vector<TaskSchedulerItem*>& taskSchedulerItems, const CComPtr<IRegisteredTask>& task, const wstring& folderPath) {
    CComBSTR nameCComBSTR;
    task->get_Name(&nameCComBSTR);
    BSTR nameBSTR = nameCComBSTR.m_str;

    std::wstring name(nameBSTR, SysStringLen(nameBSTR));

    for (unsigned int i = 0; i < taskSchedulerItems.size(); i++) {
        if (taskSchedulerItems[i]->Matches(folderPath, name)) {
            bool stateShouldBe = taskSchedulerItems[i]->GetState();
            bool stateIs = true;
            bool stateIsUnknown = false;

            // Internals that I like to keep contained
            {
                VARIANT_BOOL isEnabled;
                HRESULT hrObtainedIsEnabled = task.p->get_Enabled(&isEnabled);
                if (FAILED(hrObtainedIsEnabled)) {
                    stateIsUnknown = true;
                }
                else {
                    stateIs = isEnabled ? true : false;
                }
            }

            bool successful = false;
            wstring status;

            // Unknown current state will lead to an attempt to set it
            if (stateIsUnknown || (stateIs && !stateShouldBe)) {
                // Disable
                HRESULT hrDisablingTheTask = task.p->put_Enabled(FALSE);
                if (!FAILED(hrDisablingTheTask)) {
                    successful = true;
                    status = L"disabled";
                }
            }
            else if (stateIsUnknown || (!stateIs && stateShouldBe)) {
                // Enable
                HRESULT hrDisablingTheTask = task.p->put_Enabled(TRUE);
                if (!FAILED(hrDisablingTheTask)) {
                    successful = true;
                    status = L"enabled";
                }
            }
            else {
                successful = true;
                status = L"keep state";
            }

            // Premises
            (*debugOutput) << "============================================" << endl;
            (*debugOutput) << "Folder:       " << folderPath << endl;
            (*debugOutput) << "Name:         " << name << endl;
            (*debugOutput) << "State is:     " << (stateIsUnknown ? "unknown" : stateIs ? "enabled" : "disabled") << endl;
            (*debugOutput) << "Target state: " << (stateShouldBe ? "enabled" : "disabled") << endl;

            // The outcome
            (*debugOutput) << "Outcome:      ";
            if (successful) {
                (*debugOutput) << status;
            }
            else {
                (*debugOutput) << "couldn't set the target state";
            }
            (*debugOutput) << endl;
        }
    }
}
