#include "VExt.h"

#include "Logger.h"
#include "Pattern.h"
#include "Versions.h"

namespace {
uintptr_t(*GetAddressOfEntity)(int entity) = nullptr;

int handlingOffset = 0;
int wheelsPtrOffset = 0;
int numWheelsOffset = 0;
}

void VExt::Init() {
    uintptr_t addr;

    Logger::Write(INFO, "Version: %s", Versions::IsEnhanced() ? "Enhanced" : "Legacy");

    if (!Versions::IsEnhanced()) {
        addr = Pattern::Find("83 F9 FF 74 31 4C 8B 0D ? ? ? ? 44 8B C1 49 8B 41 08");
        GetAddressOfEntity = addr == 0 ? nullptr : reinterpret_cast<uintptr_t(*)(int)>(addr);
    }
    else {
        addr = Pattern::Find("41 8B 4C 1C ? E8");
        GetAddressOfEntity = addr == 0 ? nullptr : reinterpret_cast<uintptr_t(*)(int)>(addr + 10 + *(int*)(addr + 6));
    }
    if (!GetAddressOfEntity)
        Logger::Write(ERROR_, "Couldn't find GetAddressOfEntity");

    if (!Versions::IsEnhanced()) {
        addr = Pattern::Find("3C 03 0F 85 ? ? ? ? 48 8B 41 20 48 8B 88");
        handlingOffset = addr == 0 ? 0 : *(int*)(addr + 0x16);
    }
    else {
        addr = Pattern::Find("88 90 ? ? ? 00 0F B7 90 ? 00 00 00");
        handlingOffset = addr == 0 ? 0 : *(int*)(addr + 2) - 9;
    }
    Logger::Write(handlingOffset == 0 ? WARN : DEBUG, "Handling Offset: 0x%X", handlingOffset);

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
    Logger::Write(wheelsPtrOffset == 0 ? WARN : DEBUG, "Wheels Pointer Offset: 0x%X", wheelsPtrOffset);
    Logger::Write(numWheelsOffset == 0 ? WARN : DEBUG, "Wheel Count Offset: 0x%X", numWheelsOffset);
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
