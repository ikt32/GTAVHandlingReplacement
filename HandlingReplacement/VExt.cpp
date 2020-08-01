#include "VExt.h"

#include "Logger.h"
#include "Pattern.h"

namespace {
    uintptr_t(*GetAddressOfEntity)(int entity) = nullptr;

    int handlingOffset = 0;
    int wheelsPtrOffset = 0;
    int numWheelsOffset = 0;
}

void VExt::Init() {
    auto addr = Pattern::Find("83 F9 FF 74 31 4C 8B 0D ? ? ? ? 44 8B C1 49 8B 41 08");
    if (!addr) 
        Logger::Write(ERROR, "Couldn't find GetAddressOfEntity");
    GetAddressOfEntity = reinterpret_cast<uintptr_t(*)(int)>(addr);

    addr = Pattern::Find("3C 03 0F 85 ? ? ? ? 48 8B 41 20 48 8B 88");
    handlingOffset = addr == 0 ? 0 : *(int*)(addr + 0x16);
    Logger::Write(handlingOffset == 0 ? WARN : DEBUG, "Handling Offset: 0x%X", handlingOffset);

    addr = Pattern::Find("3B B7 ? ? ? ? 7D 0D");
    wheelsPtrOffset = addr == 0 ? 0 : *(int*)(addr + 2) - 8;
    Logger::Write(wheelsPtrOffset == 0 ? WARN : DEBUG, "Wheels Pointer Offset: 0x%X", wheelsPtrOffset);

    numWheelsOffset = addr == 0 ? 0 : *(int*)(addr + 2);
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
