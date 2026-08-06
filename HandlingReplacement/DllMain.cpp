#include "Main.h"
#include "HandlingReplacement.h"
#include "Logger.h"

#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            gLogger.SetPath("./HandlingReplacement.log");
            gLogger.SetLogLevel(ELogLevel::Debug);
            gLogger.Clear();
            LOG(Info, "HandlingReplacement v{} (built {} {})", HR_VER, __DATE__, __TIME__);
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
