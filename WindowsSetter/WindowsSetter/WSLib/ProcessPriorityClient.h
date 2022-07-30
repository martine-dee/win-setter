#ifndef INCL_PROCESS_PRIORITY_CLIENT
#define INCL_PROCESS_PRIORITY_CLIENT

#include <string>
#include <vector>

#include "../WSItems/CpuPriorityItem.h"

class ProcessPriorityClient {
    public:
        ProcessPriorityClient();
        bool GetPriority(const std::wstring&, int&);
        bool SetPriority(const std::wstring&, int);

        bool SetCpuPriority(const std::vector<CpuPriorityItem*>&);

    private:
        std::wstring KeyPathFromExecutableName(const std::wstring&);
};

#endif
