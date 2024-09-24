/*
 * This file is part of the CitizenFX project - http://citizen.re/
 */

#include "sysAllocator.h"

#include <cassert>

#include "Pattern.h"
#include "Logger.h"

#include <intrin.h>

namespace rage {
static sysMemAllocator* g_gtaTlsEntry;
static uint32_t g_tlsOffset;
static uint32_t g_tempAllocatorTlsOffset;

void* sysUseAllocator::operator new(size_t size) {
    return GetAllocator()->allocate(size, 16, 0);
}

void sysUseAllocator::operator delete(void* memory) {
    GetAllocator()->free(memory);
}

uint32_t sysMemAllocator::GetAllocatorTlsOffset() {
    return g_tlsOffset;
}

sysMemAllocator* sysMemAllocator::UpdateAllocatorValue() {
    assert(g_gtaTlsEntry);

    *(sysMemAllocator**)(hook::get_tls() + sysMemAllocator::GetAllocatorTlsOffset()) = g_gtaTlsEntry;
    *(sysMemAllocator**)(hook::get_tls() + g_tempAllocatorTlsOffset) = g_gtaTlsEntry;

    return g_gtaTlsEntry;
}

void Init() {
    return;
    auto addr = Pattern::Find("B9 ? ? ? ? 48 8B 0C 01 45 33 C9 49 8B D2 48");
    if (addr == 0) {
        Logger::Write(ERROR, "Failed to find pat 1");
        return;
    }
    g_tlsOffset = *(uint32_t*)(addr + 1);
    Logger::Write(DEBUG, "g_tlsOffset = 0x%X", g_tlsOffset);

    g_gtaTlsEntry = *(sysMemAllocator**)(hook::get_tls() + sysMemAllocator::GetAllocatorTlsOffset());
    Logger::Write(DEBUG, "g_gtaTlsEntry = 0x%X", g_gtaTlsEntry);

    addr = Pattern::Find("4A 3B 1C 09 75 ? 49 8B 0C 08 48");
    if (addr == 0) {
        Logger::Write(ERROR, "Failed to find pat 2");
        return;
    }
    g_tempAllocatorTlsOffset = *(uint32_t*)(addr - 4);
    Logger::Write(DEBUG, "g_tempAllocatorTlsOffset = 0x%X", g_tempAllocatorTlsOffset);
}
}
