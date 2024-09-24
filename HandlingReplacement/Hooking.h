/*
 * This file was part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#pragma once

#include <Windows.h>
#include <stdint.h>
#include <memory>
#include <functional>

namespace hook {
    // gets the current executable TLS offset
template<typename T = char*>
T get_tls() {
    // ah, the irony in using TLS to get TLS
    static auto tlsIndex = ([]() {
        auto base = (char*)GetModuleHandle(NULL);
        auto moduleBase = (PIMAGE_DOS_HEADER)base;
        auto ntBase = (PIMAGE_NT_HEADERS)(base + moduleBase->e_lfanew);
        auto tlsBase = (PIMAGE_TLS_DIRECTORY)(base + ntBase->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

        return reinterpret_cast<uint32_t*>(tlsBase->AddressOfIndex);
    })();

    LPVOID* tlsBase = (LPVOID*)__readgsqword(0x58);

    return (T)tlsBase[*tlsIndex];
}
}
