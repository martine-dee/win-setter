#ifndef INCL_TASK_SCHEDULER_ITEM_FULL
#define INCL_TASK_SCHEDULER_ITEM_FULL

#include "TaskSchedulerItem.h"
#include <string>

class TaskSchedulerItemFull : public TaskSchedulerItem {
    public:
        TaskSchedulerItemFull(const std::wstring&, const std::wstring&, bool);

        bool Matches(const std::wstring&, const std::wstring&);
};

#endif