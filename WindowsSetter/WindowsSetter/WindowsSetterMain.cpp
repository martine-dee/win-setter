#include "WindowsSetter.h"

#include <map>
#include <iostream>
#include <ostream>
using namespace std;

#include "WSLib/WSLibHelpers.h"

wostream* debugOutput = &wcout;
string globalBasePath;

int mainProcessArgv(int, char**, std::map<std::string, std::string>&);

int main(int argc, char** argv) {

    map<string, string> options;
    mainProcessArgv(argc, argv, options);

    // Load the *.ini file
    WindowsSetter ws(globalBasePath + "WinSetter.ini");

    // TODO: Let the user know what settings are going to be probed for and what is going to happen for each

    // Obtain user's consent to continue (or not), if not given yet
    if (options["yesGiven"] != "true") {
        wstring answer;
        wcout << "Do you want to proceed? (yes/NO)" << endl;
        wcin >> answer;

        if (answer != L"yes") {
            if (WSLibHelpers::wstringCompareCaseInsensitive(answer, L"y") == 0
                || WSLibHelpers::wstringCompareCaseInsensitive(answer, L"yes") == 0
                ) {
                wcout << "You have to type 'yes' (lowercase) to proceed." << endl;
                wcin >> answer;
            }
            else {
                wcout << "No confimation. No changes will be made." << endl;
                return 0;
            }
        }

        if (answer == L"yes") {
            options["yesGiven"] = "true";
        }
    }

    // If consent is given, set all things as specified in the *.ini file
    if (options["yesGiven"] == "true") {
        ws.SetAll();
    }

    // Give the user time to review the output, unless they set the arg --yes
    if (options["consoleYesGiven"] != "true") {
        system("pause");
    }

    return 0;
}

int mainProcessArgv(int argc, char** argv, map<string, string>& options) {

    // Obtain the working directory; not pretty, but it works
    {
        string executablePathRaw(argv[0]);
        size_t pos = executablePathRaw.rfind("\\");
        if (pos != -1) {
            globalBasePath = executablePathRaw.substr(0, pos + 1);
            options["basePath"] = globalBasePath;
        }
    }

    // Process argv
    options["consoleYesGiven"] = "false";
    options["yesGiven"] = "false";
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            char* arg = argv[i];
            if (strcmp(arg, "--yes") == 0) {
                options["consoleYesGiven"] = "true";
                options["yesGiven"] = "true";
            }
        }
    }

    return 0;
}