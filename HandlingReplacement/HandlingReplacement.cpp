#include "HandlingReplacement.h"
#include "Main.h"

#include "HandlingContext.h"
#include "VExt.h"

#include <Windows.h>
#include <memory>
#include <unordered_map>

namespace {
std::unordered_map<int, std::shared_ptr<SHandlingContext>> gStoredHandlings{};
}

void replaceHandling(int vehicle) {
    Logger::Write(DEBUG, "[Handling] Replacing handling for vehicle [%X]", vehicle);

    CHandlingData* origHandling = (CHandlingData*)VExt::GetHandlingPtr(vehicle);
    CHandlingData* newHandling = new CHandlingData; //(CHandlingData*)rage::GetAllocator()->allocate(sizeof(CHandlingData), 16, 0);
    memcpy(newHandling, origHandling, sizeof(*origHandling));

    CBaseSubHandlingData* shds[6] = {};

    //for (int i = 0; i < newHandling->m_subHandlingData.GetCount(); i++) {
    //    if (newHandling->m_subHandlingData.Get(i)) {
    //        shds[i] = (CBaseSubHandlingData*)rage::GetAllocator()->allocate(1024, 16, 0);
    //        memcpy(shds[i], newHandling->m_subHandlingData.Get(i), 1024);
    //
    //        Logger::Write(DEBUG, "[SubHandlingData] [%p] -> [%p]", newHandling->m_subHandlingData.Get(i), shds[i]);
    //    }
    //}
    //
    //newHandling->m_subHandlingData.m_offset = nullptr;
    //newHandling->m_subHandlingData.Clear();
    //newHandling->m_subHandlingData.Set(0, shds[0]);
    //newHandling->m_subHandlingData.Set(1, shds[1]);
    //newHandling->m_subHandlingData.Set(2, shds[2]);
    //newHandling->m_subHandlingData.Set(3, shds[3]);
    //newHandling->m_subHandlingData.Set(4, shds[4]);
    //newHandling->m_subHandlingData.Set(5, shds[5]);

    gStoredHandlings[vehicle] = {
        std::make_shared<SHandlingContext>(
            vehicle,
            origHandling,
            newHandling)
    };
    gStoredHandlings[vehicle]->TimesReferenced++;

    uint64_t oldAddr0 = (uint64_t)gStoredHandlings[vehicle]->OriginalHandling;
    uint64_t newAddr0 = (uint64_t)gStoredHandlings[vehicle]->ReplacedHandling;

    VExt::SetHandlingPtr(vehicle, (uint64_t)gStoredHandlings[vehicle]->ReplacedHandling);
    for (uint8_t idx = 0; idx < VExt::GetNumWheels(vehicle); ++idx) {
        VExt::SetWheelHandlingPtr(vehicle, idx, (uint64_t)gStoredHandlings[vehicle]->ReplacedHandling);
    }

    Logger::Write(INFO, "[Handling] Changed handling for [0x%X]: [0x%llx] -> [0x%llx]", vehicle, oldAddr0, newAddr0);
}

bool restoreHandling(int vehicle) {
    auto itHandlingContext = gStoredHandlings.find(vehicle);
    if (itHandlingContext == gStoredHandlings.end())
        return false;

    if (--itHandlingContext->second->TimesReferenced == 0) {
        // This should call the destructor, I think!
        // Not using natives, so I *think* an invalid vehicle on game exit, doesn't cause problems.
        gStoredHandlings.erase(vehicle);
        return true;
    }
    return false;
}

void HR_Init() {
    Logger::Write(LogLevel::INFO, "Initializing HandlingReplacement");
    VExt::Init();
}

void HR_Exit() {
    gStoredHandlings.clear();
    Logger::Write(LogLevel::INFO, "Cleared handlings");
}

bool HR_Enable(int vehicle, void** pHandlingData) {
    if (!pHandlingData) {
        return false;
    }

    if (!VExt::GetAddress(vehicle)) {
        *pHandlingData = nullptr;
        return false;
    }

    // new entry
    if (gStoredHandlings.find(vehicle) == gStoredHandlings.end()) {
        replaceHandling(vehicle);
        *pHandlingData = (void*)VExt::GetHandlingPtr(vehicle);
        return true;
    }

    // existing entry
    gStoredHandlings[vehicle]->TimesReferenced++;
    *pHandlingData = (void*)VExt::GetHandlingPtr(vehicle);
    return false;
}

bool HR_Disable(int vehicle, void** pHandlingData) {
    if (!pHandlingData) {
        return false;
    }

    if (!VExt::GetAddress(vehicle)) {
        *pHandlingData = nullptr;
        return false;
    }

    // entry exists
    if (gStoredHandlings.find(vehicle) != gStoredHandlings.end()) {
        bool restored = restoreHandling(vehicle);
        *pHandlingData = (void*)VExt::GetHandlingPtr(vehicle);
        return restored;
    }

    // entry doesn't exist
    *pHandlingData = (void*)VExt::GetHandlingPtr(vehicle);
    return false;
}

bool HR_GetHandlingData(int vehicle, void** pHandlingDataOriginal, void** pHandlingDataReplaced) {
    if (!pHandlingDataOriginal || !pHandlingDataReplaced) {
        return false;
    }

    if (!VExt::GetAddress(vehicle)) {
        pHandlingDataOriginal = nullptr;
        pHandlingDataReplaced = nullptr;
        return false;
    }

    auto itHandlingCtx = gStoredHandlings.find(vehicle);
    if (itHandlingCtx != gStoredHandlings.end()) {
        *pHandlingDataOriginal = itHandlingCtx->second->OriginalHandling;
        *pHandlingDataReplaced = itHandlingCtx->second->ReplacedHandling;
        return true;
    }

    *pHandlingDataOriginal = (void*)VExt::GetHandlingPtr(vehicle);
    *pHandlingDataReplaced = (void*)VExt::GetHandlingPtr(vehicle);
    return false;
}
