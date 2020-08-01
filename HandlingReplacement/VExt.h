#pragma once

#include <cstdint>

class VExt
{
public:
    static void Init();
    static uint8_t* GetAddress(int handle);

    static uint64_t GetHandlingPtr(int handle);
    static void SetHandlingPtr(int handle, uint64_t value);
    static uint64_t GetWheelsPtr(int handle);
    static uint8_t GetNumWheels(int handle);

    static uint64_t GetWheelHandlingPtr(int handle, uint8_t index);
    static void SetWheelHandlingPtr(int handle, uint8_t index, uint64_t value);
};

