#ifndef INCL_CPU_PRIORITY_ITEM
#define INCL_CPU_PRIORITY_ITEM

#include <string>

class CpuPriorityItem {
    private:
        std::wstring name;
        int priority;

    public:
        CpuPriorityItem(const std::wstring&, const int&);
        std::wstring GetName();
        int GetPriority();
};

#endif