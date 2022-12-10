#include "SettingsLoader.h"

#include <atlbase.h>

#include <fstream>
#include <regex>
#include <ostream>
using namespace std;

#include "WSItems/CpuPriorityItem.h"
#include "WSItems/ServiceStateItem.h"
#include "WSItems/TaskSchedulerItemFull.h"
#include "WSItems/TaskSchedulerItemPartial.h"
#include "WSLib/SoundCardSettingClient.h"
#include "WSLib/WSLibHelpers.h"

extern wostream* debugOutput;

// Code expander for reporting a parsing issue
#define REPORT_ISSUE(____DESC)  (*debugOutput) << ____DESC << " on line " << linePos << " in '" << settingIniPath << "':" << endl;\
(*debugOutput) << "  [" << line.c_str() << "]" << endl;

bool SettingsLoader::LoadSettings(const char* settingIniPath, WindowsSetter& windowsSetter) {
    if (settingIniPath == 0 || *settingIniPath == 0) {
        (*debugOutput) << "The supplied path can't be null or empty. It needs to be path to a file. No setting was loaded." << endl;
        return true;
    }

    ifstream fin;
    fin.open(settingIniPath);
    if (!fin.good()) {
        (*debugOutput) << "Couldn't find the file '" << settingIniPath << "'. No setting was loaded." << endl;
    }

    if (!windowsSetter.IsEmpty()) {
        (*debugOutput) << "The WindowsSetter has entries set. Won't fill it with hard-coded data. To do this, please .ClearAllEntries() in the WindowsSetter." << endl;
        return true;
    }

    regex RemoveLeadingTrailingWhitespaces("^\\s+|\\s+$");
    regex ParseTaskSchedulerLine("TASK SCHEDULER: ([^\\s]+) \"([^\"]+)\" ([^\\s]+)");
    regex ParseCpuPriorityClassLine("REGISTRY CPUPRIORITYCLASS \"([^\"]+)\" ([^\\s]+)");
    regex ParseSetSoundCardLine("SETSOUNDCARD \"([^\"]+)\"");
    regex ParserSetServiceState("SET SERVICE STATE \"([^\"]+)\" \"([^\"]+)\"");
    regex ParserTimeURL("TIME URL \"([^\"]+)\"");
    regex ParserTimeField("TIME FIELD \"([^\"]+)\"");
    regex ParserDeletionTarget("DELETE \"([^\"]+)\"");

    int linePos = 0;
    string line;
    while (std::getline(fin, line)) {
        linePos++;
        // Remove comments
        size_t cutPos = line.find("#", 0L);
        if (cutPos != -1) { line = line.substr(0L, cutPos); }

        // Remove all leading and trailing spaces
        line = regex_replace(line, RemoveLeadingTrailingWhitespaces, string(""));

        // Skip empty lines
        if (line.size() == 0) { continue; }

        // Sort out what this line is
        smatch taskSchedulerMatch;
          if (regex_match(line, taskSchedulerMatch, ParseTaskSchedulerLine)) {
            if (taskSchedulerMatch.size() == 4) {
                string match_type = taskSchedulerMatch[1].str();
                string path = taskSchedulerMatch[2].str();
                string target_state_raw = taskSchedulerMatch[3].str();

                // Split the path into folder and item
                string folder;
                string item;
                {
                    size_t slashPos = path.rfind("\\");
                    if (slashPos == -1) {
                        REPORT_ISSUE("Couldn't parse path in a TASK SCHEDULER entry: '" << path.c_str() << "';");
                        continue;
                    }
                    if (slashPos == 0) {
                        folder = "\\";
                    }
                    else {
                        folder = path.substr(0, slashPos);
                    }
                    item = path.substr(slashPos + 1);
                }

                // Validate the match_type
                if (match_type != "STARTS_WITH" && match_type != "MATCHES") {
                    REPORT_ISSUE("Unknown match_type='" << match_type.c_str() << "'");
                    continue;
                }

                // Validate the target_state_raw
                bool target_state = false;
                if (target_state_raw == "DISABLE") {
                    target_state = false;
                }
                else if (target_state_raw == "ENABLE") {
                    target_state = true;
                }
                else {
                    REPORT_ISSUE("Unknown target_state='" << target_state_raw.c_str() << "'");
                    continue;
                }

                //
                // Create the entry
                //

                if (match_type == "STARTS_WITH") {
                    windowsSetter.AddTaskSchedulerItem(new TaskSchedulerItemPartial(
                        WSLibHelpers::stringTowstring(folder),
                        WSLibHelpers::stringTowstring(item),
                        target_state
                    ));
                }
                if (match_type == "MATCHES") {
                    windowsSetter.AddTaskSchedulerItem(new TaskSchedulerItemFull(
                        WSLibHelpers::stringTowstring(folder),
                        WSLibHelpers::stringTowstring(item),
                        target_state
                    ));
                }
            }
            else {
                REPORT_ISSUE("Couldn't parse a TASK SCHEDULER entry");
                continue;
            }

            continue;
        }
        
        smatch priorityClassMatch;
        if (regex_match(line, priorityClassMatch, ParseCpuPriorityClassLine)) {
            if (priorityClassMatch.size() == 3) {
                string executableName = priorityClassMatch[1].str();
                string targetPriorityRaw = priorityClassMatch[2].str();
                int targetPriority;
                try {
                    targetPriority = stoi(targetPriorityRaw.c_str());
                }
                catch (exception e) {
                    REPORT_ISSUE("Couldn't parse priority '" << targetPriorityRaw.c_str() << "' in a TASK SCHEDULER entry; ");
                    continue;
                }

                if (targetPriority < 1 || targetPriority > 3) {
                    REPORT_ISSUE("Invalid target_priority in a CPUPRIORITYCLASS Entry: " << targetPriority << "; ");
                    continue;
                }

                // TODO: valide the executableName

                // Add the item
                windowsSetter.AddCpuPriorityItem(new CpuPriorityItem(
                    WSLibHelpers::stringTowstring(executableName),
                    targetPriority
                ));
            }

            continue;
        }

        smatch setServiceStateMatch;
        if (regex_match(line, setServiceStateMatch, ParserSetServiceState)) {
            if (setServiceStateMatch.size() == 3) {
                string serviceNameRaw = setServiceStateMatch[1].str();
                string serviceStateRaw = setServiceStateMatch[2].str();

                wstring serviceName = WSLibHelpers::stringTowstring(serviceNameRaw);
                DWORD serviceState = -1;

                if (serviceStateRaw == "SERVICE_BOOT_START") {
                    serviceState = SERVICE_BOOT_START;
                }
                else if (serviceStateRaw == "SERVICE_SYSTEM_START") {
                    serviceState = SERVICE_SYSTEM_START;
                }
                else if (serviceStateRaw == "SERVICE_AUTO_START") {
                    serviceState = SERVICE_AUTO_START;
                }
                else  if (serviceStateRaw == "SERVICE_DEMAND_START") {
                    serviceState = SERVICE_DEMAND_START;
                }
                else if (serviceStateRaw == "SERVICE_DISABLED") {
                    serviceState = SERVICE_DISABLED;
                }
                else {
                    // TODO: don't recognize this...
                    continue;
                }

                windowsSetter.AddServiceStateItem(new ServiceStateItem(serviceName, serviceState));
                continue;
            }
        }

        smatch setSoundCardMatch;
        if (regex_match(line, setSoundCardMatch, ParseSetSoundCardLine)) {
            if (setSoundCardMatch.size() == 2) {
                string soundCardNameRaw = setSoundCardMatch[1].str();
                wstring soundCardName = WSLibHelpers::stringTowstring(soundCardNameRaw);

                vector<wstring> existingSoundCardNames;
                {
                    SoundCardSettingClient scsc;
                    scsc.GetSoundCardNames(existingSoundCardNames);
                }

                bool found = false;
                wstring foundCaseInsensitive = L"";
                for(unsigned int i = 0; i < existingSoundCardNames.size(); i++) {
                    wstring foundName = existingSoundCardNames[i];
                    if (foundName == soundCardName) {
                        found = true;
                        break;
                    }
                    if (WSLibHelpers::wstringCompareCaseInsensitive(foundName, soundCardName) == 0) {
                        foundCaseInsensitive = foundName;
                    }
                }

                if (!found) {
                    if (foundCaseInsensitive != L"") {
                        (*debugOutput) << "The sound card name '" << soundCardName << "', wasn't found exactly, but '" << foundCaseInsensitive << "' resembles enough. Will use it instead." << endl;
                        soundCardName = foundCaseInsensitive;
                        found = true;
                    }
                    else {
                        // Bad news
                        REPORT_ISSUE("Couldn't find the sound card named '" << soundCardName << "'. Please check this setting.");
                        continue;
                    }
                }
                
                if (found) {
                    windowsSetter.SetSoundCardName(soundCardName);
                }
            }
            else {
                REPORT_ISSUE("Couldn't parse a SETSOUNDCARD entry");
                continue;
            }
            continue;
        }

        smatch timeURLMatch;
        if (regex_match(line, timeURLMatch, ParserTimeURL)) {
            if (windowsSetter.IsTimeURLSet()) {
                REPORT_ISSUE("TimeURL is already set. Ignoring this entry.");
            }
            else if (timeURLMatch.size() == 2) {
                string timeURLRaw = timeURLMatch[1].str();
                wstring timeURL = WSLibHelpers::stringTowstring(timeURLRaw);
                windowsSetter.SetTimeURL(timeURL);
            }
            else {
                REPORT_ISSUE("Couldn't parse a TIME URL entry");
            }
            continue;
        }

        smatch timeFieldMatch;
        if (regex_match(line, timeFieldMatch, ParserTimeField)) {
            if (windowsSetter.IsTimeFieldSet()) {
                REPORT_ISSUE("TimeField is already set. Ignoring this entry.");
            }
            else if (timeFieldMatch.size() == 2) {
                string timeFieldRaw = timeFieldMatch[1].str();
                wstring timeField = WSLibHelpers::stringTowstring(timeFieldRaw);
                windowsSetter.SetTimeField(timeField);
            }
            else {
                REPORT_ISSUE("Couldn't parse a TIME FIELD entry");
            }
            continue;
        }

        smatch deletionTargetMatch;
        if (regex_match(line, deletionTargetMatch, ParserDeletionTarget)) {
            if (deletionTargetMatch.size() == 2) {
                string deletionTargetPathRaw = deletionTargetMatch[1].str();
                {
                    char buffer[1024];
                    ExpandEnvironmentStringsA(
                        deletionTargetPathRaw.c_str(),
                        buffer,
                        1024
                    );
                    deletionTargetPathRaw = buffer;
                }
                wstring deletionTargetPath = WSLibHelpers::stringTowstring(deletionTargetPathRaw);

                if (!deletionTargetPath.empty()) {
                    std::ifstream fin(deletionTargetPath);
                    if (fin) {
                        fin.close();
                        windowsSetter.AddDeletionTargetItem(deletionTargetPath);
                    }
                    else {
                        (*debugOutput) << "============================================" << endl;
                        (*debugOutput) << "The DELETE entry targets an already absent file/invalid path '" << deletionTargetPath << "'" << endl;
                    }
                    continue;
                }
                else {
                    REPORT_ISSUE("The DELETE entry is empty");
                }
            }
            else {
                REPORT_ISSUE("Couldn't parse a DELETE entry");
            }
        }
        
        REPORT_ISSUE("Couldn't parse an entry");
    }

    fin.close();
    return false;
}
