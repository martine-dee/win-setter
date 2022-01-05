#ifndef INCL_TASK_SCHEDULER_CLIENT
#define INCL_TASK_SCHEDULER_CLIENT

#include <string>
#include <vector>
#include <atlbase.h>
#include <taskschd.h>

#include "../WSItems/TaskSchedulerItem.h"

class TaskSchedulerClient {
public:
    TaskSchedulerClient();

    bool SetTasks(const std::vector<TaskSchedulerItem*>&);
    bool SetTasks(const std::vector<TaskSchedulerItem*>&, ITaskFolder*);
    void SetTask(const std::vector<TaskSchedulerItem*>&, const CComPtr<IRegisteredTask>&, const std::wstring&);
};

#endif