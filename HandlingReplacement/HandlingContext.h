#pragma once
#include <new>
#include <vector>

#include "Logger.h"
#include "HandlingData.h"
#include "VExt.h"

struct SHandlingContext {
    SHandlingContext() = default;
    SHandlingContext(int v, CHandlingData* o, CHandlingData* r)
        : Vehicle(v)
        , OriginalHandling(o)
        , ReplacedHandling(r)
        , TimesReferenced(0) {}

    SHandlingContext(const SHandlingContext& hc)
        : Vehicle(hc.Vehicle)
        , OriginalHandling(hc.OriginalHandling)
        , ReplacedHandling(hc.ReplacedHandling)
        , SubHandlingArray(hc.SubHandlingArray)
        , SubHandlingCount(hc.SubHandlingCount)
        , ClonedSubHandlings(hc.ClonedSubHandlings)
        , TimesReferenced(hc.TimesReferenced) {}

    SHandlingContext(SHandlingContext&& hc) noexcept
        : Vehicle(hc.Vehicle)
        , OriginalHandling(hc.OriginalHandling)
        , ReplacedHandling(hc.ReplacedHandling)
        , SubHandlingArray(hc.SubHandlingArray)
        , SubHandlingCount(hc.SubHandlingCount)
        , ClonedSubHandlings(std::move(hc.ClonedSubHandlings))
        , TimesReferenced(hc.TimesReferenced) {
        hc.Vehicle = 0;
        hc.OriginalHandling = nullptr;
        hc.ReplacedHandling = nullptr;
        hc.SubHandlingArray = nullptr;
        hc.SubHandlingCount = 0;
        hc.TimesReferenced = 0;
    }

    SHandlingContext& operator=(const SHandlingContext& other) {
        if (this == &other)
            return *this;

        Vehicle = other.Vehicle;
        OriginalHandling = other.OriginalHandling;
        ReplacedHandling = other.ReplacedHandling;
        SubHandlingArray = other.SubHandlingArray;
        SubHandlingCount = other.SubHandlingCount;
        ClonedSubHandlings = other.ClonedSubHandlings;
        TimesReferenced = other.TimesReferenced;
        return *this;
    }

    SHandlingContext& operator=(SHandlingContext&& other) noexcept {
        Vehicle = other.Vehicle;
        OriginalHandling = other.OriginalHandling;
        ReplacedHandling = other.ReplacedHandling;
        SubHandlingArray = other.SubHandlingArray;
        SubHandlingCount = other.SubHandlingCount;
        ClonedSubHandlings = std::move(other.ClonedSubHandlings);
        TimesReferenced = other.TimesReferenced;
        return *this;
    }

    ~SHandlingContext() {
        if (!OriginalHandling || !ReplacedHandling || !Vehicle)
            return;
        Logger::Write(DEBUG, "[Handling] Deleting handling for [%p] to [%p]", Vehicle, OriginalHandling);
        VExt::SetHandlingPtr(Vehicle, (uint64_t)OriginalHandling);
        for (uint8_t idx = 0; idx < VExt::GetNumWheels(Vehicle); ++idx) {
            VExt::SetWheelHandlingPtr(Vehicle, idx, (uint64_t)OriginalHandling);
        }
        Logger::Write(DEBUG, "[Handling] Restored handling for [%p] to [%p]", Vehicle, OriginalHandling);

        // Once the entity is pointed back at OriginalHandling (with its own, untouched
        // subhandling array), our clones are no longer referenced by the game and can
        // be freed. Every clone here was allocated with a raw, aligned ::operator new
        // (see replaceHandling), so it must be freed with the matching ::operator
        // delete - never a `delete` expression, since no constructor was ever run.
        for (auto* clone : ClonedSubHandlings) {
            if (clone)
                ::operator delete(clone, std::align_val_t(16));
        }
        ClonedSubHandlings.clear();

        if (SubHandlingArray) {
            ::operator delete(SubHandlingArray, std::align_val_t(16));
            SubHandlingArray = nullptr;
        }
        SubHandlingCount = 0;

        ::operator delete(ReplacedHandling, std::align_val_t(16));
        ReplacedHandling = nullptr;
    }
    int Vehicle = 0;
    CHandlingData* OriginalHandling = nullptr;
    CHandlingData* ReplacedHandling = nullptr;

    // Own backing storage for ReplacedHandling->m_subHandlingData.m_offset - this must
    // NOT reuse the original array's backing storage, since that array is shared with
    // every other vehicle using the same original handling entry.
    CBaseSubHandlingData** SubHandlingArray = nullptr;
    uint16_t SubHandlingCount = 0;
    // Cloned subhandling blocks whose type/size we could confidently resolve. Entries
    // we couldn't resolve are left pointing at the original (shared, unowned) data
    // instead of being guessed at - see replaceHandling.
    std::vector<CBaseSubHandlingData*> ClonedSubHandlings;

    int TimesReferenced = 0;
};

