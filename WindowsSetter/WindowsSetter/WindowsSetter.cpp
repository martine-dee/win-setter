#include "WindowsSetter.h"

#include <string>
#include <ostream>
using namespace std;

#include "SettingsLoader.h"
#include "WSLib/ProcessPriorityClient.h"
#include "WSLib/TaskSchedulerClient.h"
#include "WSLib/WSServiceHelper.h"
#include "WSLib/WSTimeSetter.h"
#include "WSItems/TaskSchedulerItemFull.h"
#include "WSItems/TaskSchedulerItemPartial.h"

extern std::wostream* debugOutput;

WindowsSetter::WindowsSetter():
    soundCardName(L""),
    timeURL(L""),
    timeField(L"")
{}

WindowsSetter::WindowsSetter(const char* iniSettingsPath):
    soundCardName(L""),
    timeURL(L""),
    timeField(L"")
{
    SettingsLoader::LoadSettings(iniSettingsPath, *this);
}

WindowsSetter::WindowsSetter(const std::string& iniSettingsPath) :
    soundCardName(L""),
    timeURL(L""),
    timeField(L"")
{
    SettingsLoader::LoadSettings(iniSettingsPath.c_str(), *this);
}

WindowsSetter::~WindowsSetter() {
    ClearAllEntries();
}

bool WindowsSetter::SetAll() {
    if (taskSchedulerItems.size() > 0) {
        TaskSchedulerClient tsc;
        tsc.SetTasks(taskSchedulerItems);
    }

    if (cpuPriorityuItems.size() > 0) {
        ProcessPriorityClient ppc;
        ppc.SetCpuPriority(cpuPriorityuItems);
    }

    if (serviceStateItems.size() > 0) {
        WSServiceHelper wssh;
        wssh.SetServiceStartupTypeBatch(serviceStateItems);
    }

    if (this->IsTimeURLSet()) {
        WSTimeSetter::SetTime(this->timeURL, this->timeField);
    }

    // TODO: Act upon the Sound Card Setting

    return false;
}

bool WindowsSetter::ClearAllEntries() {
    this->ClearTaskSchedulerItems();
    this->ClearCpuPriorityItems();
    this->ClearServiceStateItems();
    this->ClearSoundCardName();
    return false;
}

bool WindowsSetter::ClearServiceStateItems() {
    for (unsigned int i = 0; i < serviceStateItems.size(); i++) {
        delete serviceStateItems[i];
    }
    serviceStateItems.clear();
    return false;
}

bool WindowsSetter::ClearTaskSchedulerItems() {
    for (unsigned int i = 0; i < taskSchedulerItems.size(); i++) {
        delete taskSchedulerItems[i];
    }
    taskSchedulerItems.clear();
    return false;
}

bool WindowsSetter::ClearCpuPriorityItems() {
    for (unsigned int i = 0; i < cpuPriorityuItems.size(); i++) {
        delete cpuPriorityuItems[i];
    }
    cpuPriorityuItems.clear();
    return false;
}

bool WindowsSetter::IsEmpty() {
    return taskSchedulerItems.size() == 0 && cpuPriorityuItems.size() == 0;
}

bool WindowsSetter::AddTaskSchedulerItem(TaskSchedulerItem* taskSchedulerItem) {
    if (taskSchedulerItem) {
        this->taskSchedulerItems.push_back(taskSchedulerItem);
    }
    else {
        (*debugOutput) << "Won't take a null TaskSchedulerItem* in ->AddTaskSchedulerItem()" << endl;
        return true;
    }
    return false;
}
bool WindowsSetter::AddCpuPriorityItem(CpuPriorityItem* cpuPriorityItem) {
    if (cpuPriorityItem) {
        this->cpuPriorityuItems.push_back(cpuPriorityItem);
    }
    else {
        (*debugOutput) << "Won't take a null CpuPriorityItem* in ->AddCpuPriorityItem()" << endl;
        return true;
    }
    return false;
}

bool WindowsSetter::SetSoundCardName(const wstring& soundCardName) {
    if (this->soundCardName == L"") {
        this->soundCardName = soundCardName;
    }
    else {
        (*debugOutput) << "Can't SetSoundCardName('" << soundCardName << "'). It is already set to '" << this->soundCardName << ". You can use ClearSoundCardName() first." << endl;
        return true;
    }
    return false;
}

bool WindowsSetter::ClearSoundCardName() {
    this->soundCardName = L"";
    return false;
}

bool WindowsSetter::FillWithHardCodedEntries() {
    // Do this only if empty, else refuse
    if (!this->IsEmpty()) {
        (*debugOutput) << "The WindowsSetter has entries set. Won't fill it with hard-coded data. To do this, please .ClearAllEntries() in the WindowsSetter." << endl;
        return true;
    }

    // Task Scheduler settings
    taskSchedulerItems.push_back(new TaskSchedulerItemPartial(L"\\", L"NvTmRep_CrashReport", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemPartial(L"\\", L"NVIDIA GeForce Experience SelfUpdate", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemPartial(L"\\", L"NvProfileUpdaterDaily_", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemPartial(L"\\", L"NvDriverUpdateCheckDaily", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\", L"GoogleUpdateTaskMachineCore", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\", L"GoogleUpdateTaskMachineUA", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\", L"MicrosoftEdgeUpdateTaskMachineCore", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\", L"MicrosoftEdgeUpdateTaskMachineUA", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\", L"NahimicSvc32Run", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\", L"NahimicSvc64Run", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"AcPowerNotification", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"ArmouryAIOFanServer", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"ArmourySocketServer", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"ASUSUpdateTaskMachineCore", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"ASUSUpdateTaskMachineUA", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"Framework Service", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"NoiseCancelingEngine.exe", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\ASUS", L"P508PowerAgent_sdk", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\Microsoft\\VisualStudio", L"VSIX Auto Update", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemFull(L"\\Microsoft\\VisualStudio\\Updates", L"BackgroundDownload", false));
    taskSchedulerItems.push_back(new TaskSchedulerItemPartial(L"\\Microsoft\\VisualStudio\\Updates", L"UpdateConfiguration_", false));

    // Cpu Priority Settings: High
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"Cyberpunk2077.exe", 3));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"QuakeChampions.exe", 3));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"RainbowSix.exe", 3));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"RainbowSix_BE.exe", 3));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"Control_DX12.exe", 3));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"Control_DX11.exe", 3));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"fury.exe", 3)); // Ion Fury
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"QuakeChampions.exe", 3));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"QuakeChampions.exe", 3));

    // Cpu Priority Settings: Low
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"AsusUpdate.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"LibHWInfo.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"CueLLAccessService.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"Corsair.Service.CpuIdRemote64.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"Corsair.Service.DisplayAdapter.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"Corsair.Service.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"GameBarPresenceWriter.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"iCUE.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"Microsoft.ServiceHub.Controller.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"NVIDIA Web Helper.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"NVIDIA Share.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"PerfWatson2.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"RzKLService.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"SearchApp.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"SearchIndexer.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"sqlwriter.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"StartMenuExperienceHost.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"TextInputHost.exe", 1));
    cpuPriorityuItems.push_back(new CpuPriorityItem(L"TT RGB Plus.exe", 1));

    return false;
}

bool WindowsSetter::AddServiceStateItem(ServiceStateItem* serviceStateItem) {
    serviceStateItems.push_back(serviceStateItem);
    return false;
}

bool WindowsSetter::SetTimeURL(const wstring& timeURL) {
    this->timeURL = timeURL;
    return false;
}

bool WindowsSetter::IsTimeURLSet() {
    return timeURL != L"";
}

bool WindowsSetter::SetTimeField(const wstring& timeURL) {
    this->timeField = timeURL;
    return false;
}

bool WindowsSetter::IsTimeFieldSet() {
    return timeField != L"";
}