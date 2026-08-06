#include "HandlingReplacement.h"
#include "Main.h"

#include "HandlingContext.h"
#include "VExt.h"

#include <Windows.h>
#include <memory>
#include <new>
#include <unordered_map>
#include <vector>

namespace {
std::unordered_map<int, std::shared_ptr<SHandlingContext>> gStoredHandlings{};
}

void replaceHandling(int vehicle) {
    LOG(Debug, "[Handling] Replacing handling for vehicle [{:X}]", vehicle);

    CHandlingData* origHandling = (CHandlingData*)VExt::GetHandlingPtr(vehicle);

    // Allocate raw, 16-byte-aligned memory instead of using a plain `new CHandlingData`.
    // We only ever memcpy raw bytes in/out of this block and never rely on the C++
    // constructor/destructor running on it, so operator new/delete (not new/delete
    // expressions) is the correct, symmetric pair to use here. This also matches the
    // alignment the game's own allocator expects for this struct.
    auto* newHandling = static_cast<CHandlingData*>(
        ::operator new(sizeof(CHandlingData), std::align_val_t(16)));
    memcpy(newHandling, origHandling, sizeof(*origHandling));

    // The memcpy above copied m_subHandlingData's atArray header verbatim, which means
    // newHandling->m_subHandlingData.m_offset currently points at the SAME backing
    // array as origHandling - i.e. shared with every other vehicle using this handling
    // entry. We must give newHandling its own backing array before touching any
    // entries, otherwise writing cloned pointers into it would corrupt that shared data.
    CBaseSubHandlingData** subHandlingArray = nullptr;
    uint16_t subHandlingCount = origHandling->m_subHandlingData.GetCount();
    std::vector<CBaseSubHandlingData*> clonedSubHandlings;

    if (subHandlingCount > 0) {
        subHandlingArray = static_cast<CBaseSubHandlingData**>(
            ::operator new(sizeof(CBaseSubHandlingData*) * subHandlingCount, std::align_val_t(16)));

        for (uint16_t idx = 0; idx < subHandlingCount; ++idx) {
            CBaseSubHandlingData* origSubHandling = origHandling->m_subHandlingData.Get(idx);
            if (!origSubHandling) {
                subHandlingArray[idx] = nullptr;
                continue;
            }

            eHandlingType type = origSubHandling->GetHandlingType();
            size_t subSize = GetSubHandlingDataSize(type);
            if (subSize == 0) {
                // Unknown/unresolvable type - don't guess a size and risk heap
                // corruption. Fall back to sharing the original, unowned pointer.
                LOG(Warning, "[Handling] Unknown subhandling type {} for vehicle [{:X}], sharing original", (int)type, vehicle);
                subHandlingArray[idx] = origSubHandling;
                continue;
            }

            auto* clonedSubHandling = static_cast<CBaseSubHandlingData*>(
                ::operator new(subSize, std::align_val_t(16)));
            memcpy(clonedSubHandling, origSubHandling, subSize);

            subHandlingArray[idx] = clonedSubHandling;
            clonedSubHandlings.push_back(clonedSubHandling);
        }
    }

    newHandling->m_subHandlingData.m_offset = subHandlingArray;
    // Sub-handling data pointers inside newHandling now point either at freshly cloned,
    // owned blocks (freed in SHandlingContext's destructor) or, for any type we
    // couldn't confidently resolve, back at the original shared struct.

    gStoredHandlings[vehicle] = {
        std::make_shared<SHandlingContext>(
            vehicle,
            origHandling,
            newHandling)
    };
    gStoredHandlings[vehicle]->SubHandlingArray = subHandlingArray;
    gStoredHandlings[vehicle]->SubHandlingCount = subHandlingCount;
    gStoredHandlings[vehicle]->ClonedSubHandlings = std::move(clonedSubHandlings);
    gStoredHandlings[vehicle]->TimesReferenced++;

    uint64_t oldAddr0 = (uint64_t)gStoredHandlings[vehicle]->OriginalHandling;
    uint64_t newAddr0 = (uint64_t)gStoredHandlings[vehicle]->ReplacedHandling;

    VExt::SetHandlingPtr(vehicle, (uint64_t)gStoredHandlings[vehicle]->ReplacedHandling);
    for (uint8_t idx = 0; idx < VExt::GetNumWheels(vehicle); ++idx) {
        VExt::SetWheelHandlingPtr(vehicle, idx, (uint64_t)gStoredHandlings[vehicle]->ReplacedHandling);
    }

    LOG(Info, "[Handling] Changed handling for [0x{:X}]: [0x{:x}] -> [0x{:x}]", vehicle, oldAddr0, newAddr0);
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
    LOG(Info, "Initializing HandlingReplacement");
    VExt::Init();
}

void HR_Exit() {
    gStoredHandlings.clear();
    LOG(Info, "Cleared handlings");
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
