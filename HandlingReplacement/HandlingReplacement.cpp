#include "HandlingReplacement.h"
#include "Main.h"

#include "HandlingContext.h"
#include "VExt.h"

#include <Windows.h>
#include <format>
#include <memory>
#include <unordered_map>

namespace {
// todo: rename to... capitalize
std::unordered_map<int, std::unique_ptr<HandlingContext>> handlings{};
}

//enum eHandlingType {
//    HANDLING_TYPE_BIKE,
//    HANDLING_TYPE_FLYING,
//    HANDLING_TYPE_VERTICAL_FLYING,
//    HANDLING_TYPE_BOAT,
//    HANDLING_TYPE_SEAPLANE,
//    HANDLING_TYPE_SUBMARINE,
//    HANDLING_TYPE_TRAIN,
//    HANDLING_TYPE_TRAILER,
//    HANDLING_TYPE_CAR,
//    HANDLING_TYPE_WEAPON,
//    HANDLING_TYPE_SPECIALFLIGHT, // ??
//    HANDLING_TYPE_MAX_TYPES
//};

void replaceHandling(int vehicle) {
    Logger::Write(DEBUG, "[Handling] Replacing handling for vehicle [%X]", vehicle);

    CHandlingData* origHandling = (CHandlingData*)VExt::GetHandlingPtr(vehicle);

    //static auto pool = rage::GetPoolBase(CHandlingObject::kHash);
    //handlingCache.emplace_back(pool->GetEntrySize());
    //CHandlingData* newHandling = handlingCache.back().get();
    //
    //memcpy(newHandling, origHandling, pool->GetEntrySize());

    //CHandlingData* newHandling = (CHandlingData*)rage::GetAllocator()->allocate(sizeof(CHandlingData), 16, 0);
    auto newHandling = std::make_shared<CHandlingData>();
    memcpy(newHandling.get(), origHandling, sizeof(*origHandling));

    //CBaseSubHandlingData* shds[6] = {};

    //for (int i = 0; i < newHandling->m_subHandlingData.GetCount(); i++) {
    //    if (newHandling->m_subHandlingData.Get(i)) {
    //        shds[i] = (CBaseSubHandlingData*)rage::GetAllocator()->allocate(1024, 16, 0);
    //        memcpy(shds[i], newHandling->m_subHandlingData.Get(i), 1024);

    //        //shds[i] = (CBaseSubHandlingData*)rage::PoolAllocate(pool);
    //        //memcpy(shds[i], newHandling->m_subHandlingData.Get(i), pool->GetEntrySize());

    //        Logger::Write(DEBUG, "[SubHandlingData] [%p] -> [%p]", newHandling->m_subHandlingData.Get(i), shds[i]);
    //    }
    //}

    //newHandling->m_subHandlingData.m_offset = nullptr;
    //newHandling->m_subHandlingData.Clear();
    //newHandling->m_subHandlingData.Set(0, shds[0]);
    //newHandling->m_subHandlingData.Set(1, shds[1]);
    //newHandling->m_subHandlingData.Set(2, shds[2]);
    //newHandling->m_subHandlingData.Set(3, shds[3]);
    //newHandling->m_subHandlingData.Set(4, shds[4]);
    //newHandling->m_subHandlingData.Set(5, shds[5]);

    //CBaseSubHandlingData* shds[6] = {};

    //for (int i = 0; i < newHandling->m_subHandlingData.GetCount(); i++) {
    //    if (newHandling->m_subHandlingData.Get(i)) {
    //        shds[i] = (CBaseSubHandlingData*)rage::GetAllocator()->allocate(1024, 16, 0);
    //        memcpy(shds[i], newHandling->m_subHandlingData.Get(i), 1024);

    //        //shds[i] = (CBaseSubHandlingData*)rage::PoolAllocate(pool);
    //        //memcpy(shds[i], newHandling->m_subHandlingData.Get(i), pool->GetEntrySize());

    //        Logger::Write(DEBUG, "[SubHandlingData] [%p] -> [%p]", newHandling->m_subHandlingData.Get(i), shds[i]);
    //    }
    //}

    //std::vector<std::shared_ptr<CBaseSubHandlingData>> subhandlingDataCache;


    //////////////////////////////////////////////////////////////////////////

    Logger::Write(LogLevel::INFO, "Pre shd stuff");
    //std::vector<std::shared_ptr<CBaseSubHandlingData>> subHandlingDataCache;
    std::shared_ptr<std::vector<CBaseSubHandlingData*>> subHandlingDataPointers = std::make_shared<std::vector<CBaseSubHandlingData*>>(); // for atArray to work with
    Logger::Write(LogLevel::INFO, "Orig handling's subhandlingdata: %p", &(origHandling->m_subHandlingData));
    subHandlingDataPointers->resize(origHandling->m_subHandlingData.GetCount());
    Logger::Write(LogLevel::INFO, "Resized shd");

    // this thing is just size and a pointer (to an array, but we're changing it to a std::vector here)
    // can be copied, this can go out of scope once copied
    atArray<CBaseSubHandlingData*> replacementSubHandlingData;
    replacementSubHandlingData.m_size = origHandling->m_subHandlingData.GetSize();
    replacementSubHandlingData.m_count = origHandling->m_subHandlingData.GetCount();
    replacementSubHandlingData.m_offset = subHandlingDataPointers.get()->data();
    Logger::Write(LogLevel::INFO, "created shdbase replacement array thing");

    std::shared_ptr<std::vector<CAdvancedData>> advancedDataStorage;

    for (uint16_t idx = 0; idx < origHandling->m_subHandlingData.GetCount(); ++idx) {
        Logger::Write(LogLevel::INFO, "doing... %d", idx);
        CBaseSubHandlingData* subHandlingData = origHandling->m_subHandlingData.Get(idx);

        if (!subHandlingData)
            continue;

        //std::string shdPtr = std::format("{:p}", (void*)subHandlingData);

        auto type = subHandlingData->GetHandlingType();
        switch (type) {
            case HANDLING_TYPE_BIKE:
            {
                auto* pData = static_cast<CBikeHandlingData*>(malloc(sizeof(CBikeHandlingData)));
                auto data = std::shared_ptr<CBikeHandlingData>(pData/*, [](auto* storage) {free(storage); }*/);
                memcpy(data.get(), subHandlingData, sizeof(CBikeHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_FLYING:
            {
                auto* pData = static_cast<CFlyingHandlingData*>(malloc(sizeof(CFlyingHandlingData)));
                auto data = std::shared_ptr<CFlyingHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CFlyingHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_VERTICAL_FLYING:
            {
                auto* pData = static_cast<CFlyingHandlingData*>(malloc(sizeof(CFlyingHandlingData)));
                auto data = std::shared_ptr<CFlyingHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CFlyingHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_BOAT:
            {
                auto* pData = static_cast<CBoatHandlingData*>(malloc(sizeof(CBoatHandlingData)));
                auto data = std::shared_ptr<CBoatHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CBoatHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_SEAPLANE:
            {
                auto* pData = static_cast<CSeaPlaneHandlingData*>(malloc(sizeof(CSeaPlaneHandlingData)));
                auto data = std::shared_ptr<CSeaPlaneHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CSeaPlaneHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_SUBMARINE:
            {
                auto* pData = static_cast<CSubmarineHandlingData*>(malloc(sizeof(CSubmarineHandlingData)));
                auto data = std::shared_ptr<CSubmarineHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CSubmarineHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_TRAILER:
            {
                auto* pData = static_cast<CTrailerHandlingData*>(malloc(sizeof(CTrailerHandlingData)));
                auto data = std::shared_ptr<CTrailerHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CTrailerHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_CAR:
            {
                Logger::Write(LogLevel::INFO, "doing... %d: HANDLING_TYPE_CAR", idx);
                auto* pData = static_cast<CCarHandlingData*>(malloc(sizeof(CCarHandlingData)));
                auto data = std::shared_ptr<CCarHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CCarHandlingData));
                //subHandlingDataCache.push_back(data);
                // TODO: advanced data :sob:

                // pData->pAdvancedData currently is an atArray with a pointer to GTA managed memory
                // let's change that

                //advancedDataStorage = std::make_shared<std::vector<CAdvancedData>>(pData->pAdvancedData.GetCount());

                // auto lmao = static_cast<CAdvancedData*>(malloc(sizeof(CAdvancedData) * pData->pAdvancedData.GetCount()));
                // 
                // for (int i = 0; i < pData->pAdvancedData.GetCount(); ++i) {
                // 
                //     //(*advancedDataStorage)[i] = pData->pAdvancedData[i];
                //     lmao[i] = pData->pAdvancedData[i];
                // }
                // pData->pAdvancedData.m_offset = lmao;//(*advancedDataStorage).data();
                break;

            }
            case HANDLING_TYPE_SPECIALFLIGHT:
            {
                auto* pData = static_cast<CSpecialFlightHandlingData*>(malloc(sizeof(CSpecialFlightHandlingData)));
                auto data = std::shared_ptr<CSpecialFlightHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CSpecialFlightHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_WEAPON:
            {
                auto* pData = static_cast<CVehicleWeaponHandlingData*>(malloc(sizeof(CVehicleWeaponHandlingData)));
                auto data = std::shared_ptr<CVehicleWeaponHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CVehicleWeaponHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            case HANDLING_TYPE_TRAIN:
            {
                auto* pData = static_cast<CTrainHandlingData*>(malloc(sizeof(CTrainHandlingData)));
                auto data = std::shared_ptr<CTrainHandlingData>(pData/*, [](auto* storage) {free(storage); }*/); //pData, [](auto* storage) {free(storage); });
                memcpy(data.get(), subHandlingData, sizeof(CTrainHandlingData));
                //subHandlingDataCache.push_back(data);
                break;
            }
            default:
                Logger::Write(LogLevel::ERROR, "Unknown handling type %d", (int)type);
                continue;
        }
        Logger::Write(LogLevel::INFO, "assigning replacement");


    }

            // this is array
        //newHandling->m_subHandlingData[idx] = subHandlingDataCache.back().get();
    newHandling->m_subHandlingData = replacementSubHandlingData;
////////////////////////////////////////////////////////////////////////////

    Logger::Write(LogLevel::INFO, "creating handling context and moving stuff");

    handlings[vehicle] = {
        std::make_unique<HandlingContext>(
            vehicle,
            origHandling,
            std::move(newHandling)//,
          //  subHandlingDataCache,
        //    std::move(subHandlingDataPointers),
          //  advancedDataStorage
        )
    };
    handlings[vehicle]->TimesReferenced++;

    uint64_t oldAddr0 = (uint64_t)handlings[vehicle]->OriginalHandling;
    uint64_t newAddr0 = (uint64_t)handlings[vehicle]->ReplacedHandling.get();

    VExt::SetHandlingPtr(vehicle, (uint64_t)handlings[vehicle]->ReplacedHandling.get());
    for (uint8_t idx = 0; idx < VExt::GetNumWheels(vehicle); ++idx) {
        VExt::SetWheelHandlingPtr(vehicle, idx, (uint64_t)handlings[vehicle]->ReplacedHandling.get());
    }

    Logger::Write(INFO, "[Handling] Changed handling for [0x%X]: [0x%llx] -> [0x%llx]", vehicle, oldAddr0, newAddr0);
}

bool restoreHandling(int vehicle) {
    auto itHandlingContext = handlings.find(vehicle);
    if (itHandlingContext == handlings.end())
        return false;

    if (--itHandlingContext->second->TimesReferenced == 0) {
        // This should call the destructor, I think!
        // Not using natives, so I *think* an invalid vehicle on game exit, doesn't cause problems.
        handlings.erase(vehicle);
        return true;
    }
    return false;
}

void HR_Init() {
    Logger::Write(LogLevel::INFO, "Initializing HandlingReplacement");
    //rage::Init();
    VExt::Init();
}

void HR_Exit() {
    handlings.clear();
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
    if (handlings.find(vehicle) == handlings.end()) {
        replaceHandling(vehicle);
        *pHandlingData = (void*)VExt::GetHandlingPtr(vehicle);
        return true;
    }

    // existing entry
    handlings[vehicle]->TimesReferenced++;
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

    // todo: delete instead
    // entry exists
    if (auto it = handlings.find(vehicle); it != handlings.end()) {
        //bool restored = restoreHandling(vehicle);
        //*pHandlingData = (void*)VExt::GetHandlingPtr(vehicle);
        handlings.erase(it);
        return true;
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

    auto itHandlingCtx = handlings.find(vehicle);
    if (itHandlingCtx != handlings.end()) {
        *pHandlingDataOriginal = itHandlingCtx->second->OriginalHandling;
        *pHandlingDataReplaced = itHandlingCtx->second->ReplacedHandling.get();
        return true;
    }

    *pHandlingDataOriginal = (void*)VExt::GetHandlingPtr(vehicle);
    *pHandlingDataReplaced = (void*)VExt::GetHandlingPtr(vehicle);
    return false;
}
