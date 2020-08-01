#include "Main.h"
#include "HandlingReplacement.h"
#include "Logger.h"

#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            Logger::SetFile("./HandlingReplacement.log");
            Logger::Clear();
            Logger::Write(LogLevel::INFO, "HandlingReplacement v%s (built %s %s)", HR_VER, __DATE__, __TIME__);
            HR_Init();
            break;
        }
        case DLL_PROCESS_DETACH: {
            HR_Exit();
            break;
        }
    }
    return TRUE;
}
