#pragma once
#include "Logger.h"
#include "HandlingData.h"
#include "VExt.h"

#include <array>

// TODO: Rename to SHandlingContext
struct HandlingContext {
    HandlingContext() = default;
    HandlingContext(int v, CHandlingData* o, std::shared_ptr<CHandlingData>&& r//,
    //                const std::vector<std::shared_ptr<CBaseSubHandlingData>>& shdCache,
      //              std::shared_ptr<std::vector<CBaseSubHandlingData*>>&& shdPtrCache,
        //            std::shared_ptr<std::vector<CAdvancedData>> adv
    )
        : Vehicle(v)
        , OriginalHandling(o)
        , ReplacedHandling(std::move(r))
        //, SubHandlingDataCache(shdCache)
       // , SubHandlingDataPointers(std::move(shdPtrCache))
       // , AdvancedDataStorage(adv)
        , TimesReferenced(0) {}

    //HandlingContext(const HandlingContext& hc)
    //    : Vehicle(hc.Vehicle)
    //    , OriginalHandling(hc.OriginalHandling)
    //    , ReplacedHandling(hc.ReplacedHandling)
    //    , TimesReferenced(hc.TimesReferenced) {}
    //
    //HandlingContext(HandlingContext&& hc) noexcept
    //    : Vehicle(hc.Vehicle)
    //    , OriginalHandling(hc.OriginalHandling)
    //    , ReplacedHandling(hc.ReplacedHandling)
    //    , TimesReferenced(hc.TimesReferenced) {
    //    hc.Vehicle = 0;
    //    hc.OriginalHandling = nullptr;
    //    hc.ReplacedHandling = nullptr;
    //    hc.TimesReferenced = 0;
    //}
    //
    //HandlingContext& operator=(const HandlingContext& other) {
    //    if (this == &other)
    //        return *this;
    //
    //    Vehicle = other.Vehicle;
    //    OriginalHandling = other.OriginalHandling;
    //    ReplacedHandling = other.ReplacedHandling;
    //    TimesReferenced = other.TimesReferenced;
    //    return *this;
    //}
    //
    //HandlingContext& operator=(HandlingContext&& other) noexcept {
    //    Vehicle = other.Vehicle;
    //    OriginalHandling = other.OriginalHandling;
    //    ReplacedHandling = other.ReplacedHandling;
    //    TimesReferenced = other.TimesReferenced;
    //    return *this;
    //}

    ~HandlingContext() {
        if (!OriginalHandling || !ReplacedHandling || !Vehicle)
            return;
        Logger::Write(DEBUG, "[Handling] Deleting handling for [%p] to [%p]", Vehicle, OriginalHandling);
        VExt::SetHandlingPtr(Vehicle, (uint64_t)OriginalHandling);
        for (uint8_t idx = 0; idx < VExt::GetNumWheels(Vehicle); ++idx) {
            VExt::SetWheelHandlingPtr(Vehicle, idx, (uint64_t)OriginalHandling);
        }
        Logger::Write(DEBUG, "[Handling] Restored handling for [%p] to [%p]", Vehicle, OriginalHandling);
        // delete ReplacedHandling;
        //rage::GetAllocator()->free(ReplacedHandling);
        //Vehicle = 0;
        //ReplacedHandling = nullptr;
        //OriginalHandling = nullptr;
        //TimesReferenced = 0;
    }
    int Vehicle = 0;
    CHandlingData* OriginalHandling = nullptr;
    std::shared_ptr<CHandlingData> ReplacedHandling;
    //std::array<std::shared_ptr<CBaseSubHandlingData>, 6> SubhandlingDataCache;

//   std::vector<std::shared_ptr<CBaseSubHandlingData>> SubHandlingDataCache;
//   std::shared_ptr<std::vector<CBaseSubHandlingData*>> SubHandlingDataPointers;
//   std::shared_ptr<std::vector<CAdvancedData>> AdvancedDataStorage;
    int TimesReferenced = 0;
};
