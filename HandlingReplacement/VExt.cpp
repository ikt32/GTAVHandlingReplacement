#include "VExt.h"

#include "Logger.h"
#include "Pattern.h"
#include "Versions.h"

namespace {
uintptr_t(*GetAddressOfEntity)(int entity) = nullptr;

int handlingOffset = 0;
int wheelsPtrOffset = 0;
int numWheelsOffset = 0;

void InitGetEntityAddress() {
    uintptr_t addr = 0;
    if (!Versions::IsEnhanced()) {
        if (Versions::GetBuildNumber() >= 3788) {
            addr = Pattern::Find("85 ED 74 0F 8B CD E8 ? ? ? ? 48 8B F8 48 85 C0 74 2E");
            GetAddressOfEntity = addr == 0 ? nullptr : reinterpret_cast<uintptr_t(*)(int)>(addr + 11 + *(int*)(addr + 7));
        }
        else {
            addr = Pattern::Find("83 F9 FF 74 31 4C 8B 0D ? ? ? ? 44 8B C1 49 8B 41 08");
            GetAddressOfEntity = addr == 0 ? nullptr : reinterpret_cast<uintptr_t(*)(int)>(addr);
        }
    }
    else {
        if (Versions::GetBuildNumber() >= 1013) {
            addr = Pattern::Find("41 8B 4C 1C ? E8");
            GetAddressOfEntity = addr == 0 ? nullptr : reinterpret_cast<uintptr_t(*)(int)>(addr + 10 + *(int*)(addr + 6));
        }
        else {
            addr = Pattern::Find("83 F9 FF 74 64 41 89 C8");
            GetAddressOfEntity = addr == 0 ? nullptr : reinterpret_cast<uintptr_t(*)(int)>(addr);
        }
    }

    if (!GetAddressOfEntity)
        LOG(Error, "Couldn't find GetAddressOfEntity");
    else
        LOG(Debug, "Found GetAddressOfEntity at 0x{:X}", addr);
}
}

void VExt::Init() {
    uintptr_t addr;

    LOG(Info, "Version: {}", Versions::IsEnhanced() ? "Enhanced" : "Legacy");

    InitGetEntityAddress();

    if (!Versions::IsEnhanced()) {
        addr = Pattern::Find("3C 03 0F 85 ? ? ? ? 48 8B 41 20 48 8B 88");
        handlingOffset = addr == 0 ? 0 : *(int*)(addr + 0x16);
    }
    else {
        addr = Pattern::Find("88 90 ? ? ? 00 0F B7 90 ? 00 00 00");
        handlingOffset = addr == 0 ? 0 : *(int*)(addr + 2) - 9;
    }
    LOG(handlingOffset == 0 ? ELogLevel::Warning : ELogLevel::Debug, "Handling Offset: 0x{:X}", handlingOffset);

    if (!Versions::IsEnhanced()) {
        addr = Pattern::Find("3B B7 ? ? ? ? 7D 0D");
        wheelsPtrOffset = addr == 0 ? 0 : *(int*)(addr + 2) - 8;
        numWheelsOffset = addr == 0 ? 0 : *(int*)(addr + 2);
    }
    else {
        addr = Pattern::Find("8B 90 ? ? 00 00 4C 8B ? ? ? 00 00 48 8B 40 20 48 8B 80 B0 00 00 00 4C 8B ? F3 0F 11 44 24 30");
        wheelsPtrOffset = addr == 0 ? 0 : *(int*)(addr + 2) - 8;
        numWheelsOffset = addr == 0 ? 0 : *(int*)(addr + 2);
    }
    LOG(wheelsPtrOffset == 0 ? ELogLevel::Warning : ELogLevel::Debug, "Wheels Pointer Offset: 0x{:X}", wheelsPtrOffset);
    LOG(numWheelsOffset == 0 ? ELogLevel::Warning : ELogLevel::Debug, "Wheel Count Offset: 0x{:X}", numWheelsOffset);
}

uint8_t* VExt::GetAddress(int handle) {
    return reinterpret_cast<uint8_t*>(GetAddressOfEntity(handle));
}


uint64_t VExt::GetHandlingPtr(int handle) {
    if (handlingOffset == 0) return 0;
    auto address = GetAddress(handle);
    if (!address) return 0;
    return *reinterpret_cast<uint64_t*>(address + handlingOffset);
}

void VExt::SetHandlingPtr(int handle, uint64_t value) {
    if (handlingOffset == 0) return;
    auto address = GetAddress(handle);
    if (!address) return;
    *reinterpret_cast<uint64_t*>(address + handlingOffset) = value;
}

uint64_t VExt::GetWheelsPtr(int handle) {
    if (wheelsPtrOffset == 0) return 0;
    auto address = GetAddress(handle);
    if (!address) return 0;
    return *reinterpret_cast<uint64_t*>(address + wheelsPtrOffset);
}

uint8_t VExt::GetNumWheels(int handle) {
    if (numWheelsOffset == 0) return 0;
    auto address = GetAddress(handle);
    if (!address) return 0;
    return *reinterpret_cast<int*>(address + numWheelsOffset);
}

uint64_t VExt::GetWheelHandlingPtr(int handle, uint8_t index) {
    if (handlingOffset == 0) return 0;

    auto wheelPtr = GetWheelsPtr(handle);
    if (!wheelPtr) return 0;

    auto wheelAddr = *reinterpret_cast<uint64_t*>(wheelPtr + 0x008 * index);

    return *reinterpret_cast<uint64_t*>(wheelAddr + 0x120);
}

void VExt::SetWheelHandlingPtr(int handle, uint8_t index, uint64_t value) {
    if (handlingOffset == 0) return;

    auto wheelPtr = GetWheelsPtr(handle);
    if (!wheelPtr) return;

    auto wheelAddr = *reinterpret_cast<uint64_t*>(wheelPtr + 0x008 * index);

    *reinterpret_cast<uint64_t*>(wheelAddr + 0x120) = value;
}
