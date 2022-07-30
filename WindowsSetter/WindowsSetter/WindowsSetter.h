#ifndef INCL_WINDOWS_SETTER
#define INCL_WINDOWS_SETTER

#include <vector>
#include <string>

#include "WSItems/ServiceStateItem.h"
#include "WSItems/TaskSchedulerItem.h"
#include "WSItems/CpuPriorityItem.h"

class WindowsSetter {
    private:
        std::vector<TaskSchedulerItem*> taskSchedulerItems;
        std::vector<CpuPriorityItem*> cpuPriorityuItems;
        std::vector<ServiceStateItem*> serviceStateItems;
        std::wstring soundCardName;
        std::wstring timeURL;
        std::wstring timeField;

    public:
        WindowsSetter();
        ~WindowsSetter();
        WindowsSetter(const char*);
        WindowsSetter(const std::string&);
        
        // The main function
        bool SetAll();

        // Entry management
        bool IsEmpty();
        bool ClearAllEntries();
        bool AddTaskSchedulerItem(TaskSchedulerItem*);
        bool AddCpuPriorityItem(CpuPriorityItem*);
        bool AddServiceStateItem(ServiceStateItem*);
        bool SetSoundCardName(const std::wstring&);
        bool IsTimeURLSet();
        bool SetTimeURL(const std::wstring&);
        bool IsTimeFieldSet();
        bool SetTimeField(const std::wstring&);
        bool ClearServiceStateItems();
        bool ClearSoundCardName();
        bool ClearTaskSchedulerItems();
        bool ClearCpuPriorityItems();

        // Testing only
        bool FillWithHardCodedEntries();
};

#endif
