#ifndef INCL_TASK_SCHEDULER_ITEM
#define INCL_TASK_SCHEDULER_ITEM

#include <string>

class TaskSchedulerItem {
    protected:
        std::wstring taskName;
        std::wstring folderPath;
        bool state;

    public:
        TaskSchedulerItem(const std::wstring&, const std::wstring&, bool);

        virtual bool Matches(const std::wstring&, const std::wstring&) = 0;

        std::wstring GetFolderPath();
        std::wstring GetTaskName();
        bool GetState();
};

#endif