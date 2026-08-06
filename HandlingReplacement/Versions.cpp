#include "Versions.h"

#include <string>
#include <vector>
#include <Windows.h>
#include <winver.h>

#pragma comment(lib, "version.lib")

namespace {
bool nVersionResolved = false;
bool nIsEnhanced = false;

    // Reads a StringFileInfo value (e.g. "FileDescription", "ProductName") from the
    // version resource embedded in the given module. This is far more reliable than
    // inspecting the executable's path/filename, since that can be renamed freely by
    // the user while the signed version resource is set by Rockstar at build time.
    std::string getModuleVersionString(const char* moduleName, const char* key) {
        DWORD handle = 0;
        DWORD size = GetFileVersionInfoSizeA(moduleName, &handle);
        if (size == 0) {
            return {};
        }

        std::vector<uint8_t> data(size);
        if (!GetFileVersionInfoA(moduleName, handle, size, data.data())) {
            return {};
        }

        struct LangAndCodePage {
            WORD language;
            WORD codePage;
        }* translations = nullptr;
        UINT translationsSize = 0;
        if (!VerQueryValueA(data.data(), "\\VarFileInfo\\Translation",
            reinterpret_cast<LPVOID*>(&translations), &translationsSize) ||
            translationsSize < sizeof(LangAndCodePage)) {
            return {};
        }

        char subBlock[64];
        sprintf_s(subBlock, "\\StringFileInfo\\%04x%04x\\%s",
            translations[0].language, translations[0].codePage, key);

        LPVOID value = nullptr;
        UINT valueSize = 0;
        if (!VerQueryValueA(data.data(), subBlock, &value, &valueSize) || valueSize == 0) {
            return {};
        }

        return std::string(static_cast<char*>(value));
    }
}

namespace Versions {
    bool IsEnhanced() {
        if (nVersionResolved) {
            return nIsEnhanced;
        }

        char path[MAX_PATH]{};
        GetModuleFileNameA(nullptr, path, MAX_PATH);

        std::string description = getModuleVersionString(path, "FileDescription");
        if (description.empty()) {
            description = getModuleVersionString(path, "ProductName");
        }

        nVersionResolved = true;
        nIsEnhanced = description.find("Enhanced") != std::string::npos;
        return nIsEnhanced;
    }
}
