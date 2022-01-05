#ifndef INCL_TASK_SCHEDULER_ITEM_PARTIAL
#define INCL_TASK_SCHEDULER_ITEM_PARTIAL

#include "TaskSchedulerItem.h"
#include <string>

class TaskSchedulerItemPartial : public TaskSchedulerItem {
    public:
        TaskSchedulerItemPartial(const std::wstring&, const std::wstring&, bool);

        bool Matches(const std::wstring&, const std::wstring&);
};

#endif