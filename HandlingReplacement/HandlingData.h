#pragma once

#include "atArray.h"
#include "atPool.h"

#include <cstdint>
#include <cmath>
#include <vector>
#include <string>

// from types.h scripthookv
typedef unsigned long Hash;


constexpr unsigned long joaat(const char* s) {
    unsigned long hash = 0;
    for (; *s != '\0'; ++s) {
        auto c = *s;
        if (c >= 0x41 && c <= 0x5a) {
            c += 0x20;
        }
        hash += c;
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}
//
//DLL_IMPORT atPoolBase* GetPoolBase(uint32_t hash);
//
//namespace rage
//{
//    
//    DLL_IMPORT void* PoolAllocate(atPoolBase* pool);
//
//    DLL_IMPORT void PoolRelease(atPoolBase* pool, void* entry);
//
//    inline ::atPoolBase* GetPoolBase(const char* hashString) {
//        return GetPoolBase(joaat(hashString));
//    }
//
//    template<typename T>
//    inline atPool<T>* GetPool(uint32_t hash) {
//        return static_cast<atPool<T>*>(GetPoolBase(hash));
//    }
//
//    template<typename T>
//    inline atPool<T>* GetPool(const char* hashString) {
//        return static_cast<atPool<T>*>(GetPoolBase(HashString(hashString)));
//    }
//}
//


enum eHandlingType {
    HANDLING_TYPE_BIKE,
    HANDLING_TYPE_FLYING,
    HANDLING_TYPE_VERTICAL_FLYING,
    HANDLING_TYPE_BOAT,
    HANDLING_TYPE_SEAPLANE,
    HANDLING_TYPE_SUBMARINE,
    HANDLING_TYPE_TRAIN,
    HANDLING_TYPE_TRAILER,
    HANDLING_TYPE_CAR,
    HANDLING_TYPE_WEAPON,
    HANDLING_TYPE_SPECIALFLIGHT, // ??
    HANDLING_TYPE_MAX_TYPES
};

//struct CHandlingObject {
//    // boost::typeindex::ctti_type_index doesn't expose a string_view :(
//    static constexpr uint32_t kHash = joaat("CHandlingObject");
//};
//
//template<typename TPoolName>
//struct PoolAllocated
//{
//public:
//    inline void* operator new(size_t size) {
//        return rage::PoolAllocate(rage::GetPoolBase(TPoolName::kHash));
//    }
//
//    inline void* operator new[](size_t size) {
//        return PoolAllocated::operator new(size);
//    }
//
//    inline void operator delete(void* memory) {
//        rage::PoolRelease(rage::GetPoolBase(TPoolName::kHash), memory);
//    }
//
//    inline void operator delete[](void* memory) {
//        return PoolAllocated::operator delete(memory);
//    }
//};

class CAdvancedData {
public:
    virtual ~CAdvancedData() = default;
    int Slot;
    int Index;
    float Value;
};

class CHandlingObject {
public:
    virtual ~CHandlingObject() = default; // ?
    virtual void* parser_GetStructure() = 0; //ret rage::parStructure
};

class CBaseSubHandlingData: public CHandlingObject {
public:
    virtual eHandlingType GetHandlingType() = 0;
    virtual void OnPostLoad() = 0;
};

class CHandlingData {// : PoolAllocated<CHandlingObject> {
public:
    uint32_t m_name;
    char m_pad[332]; // 1290, 1365, 1493, 1604
    atArray<CBaseSubHandlingData*> m_subHandlingData; // this thing is 16 bytes
    // ^ find offset using a variant of 48 85 C9 74 13 BA 04 00 00 00 E8 (and go to the call in there)
    char m_pad2[1000];

    virtual ~CHandlingData() = default;

    //inline uint32_t GetName()
    //{
    //    return m_name;
    //}

    inline atArray<CBaseSubHandlingData*>& GetSubHandlingData() {
        return m_subHandlingData;
    }
};

static_assert(offsetof(CHandlingData, m_subHandlingData) == 0x158, "wrong offset");


namespace rage {
class Vec2V {
public:
    Vec2V() = default;

    Vec2V(float x, float y)
        :
        x(x), y(y) {}

    float x{};
    float y{};

    Vec2V operator + (Vec2V const& right) {
        return { this->x + right.x, this->y + right.y };
    }

    Vec2V operator - (Vec2V const& right) {
        return { this->x - right.x, this->y - right.y };
    }

    Vec2V operator * (float const& scale) {
        return { this->x * scale, this->y * scale };
    }

    Vec2V operator * (Vec2V& scale) {
        return { this->x * scale.x, this->y * scale.y };
    }

    operator bool() {
        return !IsZero();
    }

    bool IsZero() const {
        return (this->x == 0 && this->y == 0);
    }
};
class Vec3 {
public:
    Vec3() = default;

    Vec3(float x, float y, float z)
        :
        x(x), y(y), z(z) {}

    Vec3(Vec2V const& vec2)
        :
        x(vec2.x),
        y(vec2.y) {}

    float x{};
    float y{};
    float z{};

    Vec3 operator + (Vec3 const& right) {
        return { this->x + right.x, this->y + right.y, this->z + right.z };
    }

    Vec3 operator - (Vec3 const& right) {
        return { this->x - right.x, this->y - right.y, this->z - right.z };
    }

    Vec3 operator * (float const& scale) {
        return { this->x * scale, this->y * scale, this->z * scale };
    }

    Vec3 operator * (Vec3& scale) {
        return { this->x * scale.x, this->y * scale.y, this->z * scale.z };
    }

    operator bool() {
        return !IsZero();
    }

    float Magnitude() {
        return sqrtf(x * x + y * y + z * z);
    }

    Vec3 Normalize() {
        float mag = Magnitude();
        return  { x / mag, y / mag, z / mag };
    }

    float Distance(Vec3 const& right) {
        auto x = static_cast<float>(std::pow((this->x - right.x), 2));
        auto y = static_cast<float>(std::pow((this->y - right.y), 2));
        auto z = static_cast<float>(std::pow((this->z - right.z), 2));
        return std::sqrt(x + y + z);
    }

    Vec3 Cross(Vec3 const& right) {
        Vec3 result;
        result.x = this->y * right.z - this->z * right.y;
        result.y = this->z * right.x - this->x * right.z;
        result.z = this->x * right.y - this->y * right.x;
        return result;
    }

    bool IsZero() const {
        return (this->x == 0 && this->y == 0 && this->z == 0);
    }

    Vec2V Vec2() {
        return { x, y };
    }
};

// Vec3V has a padding at the end so that it is 128 bits and is so much faster thanks to SIMD/xmm intrinsics
class Vec3V: public Vec3 {
public:
    Vec3V() = default;

    Vec3V(float x, float y, float z)
        :
        Vec3(x, y, z) {}

    Vec3V(Vec2V const& vec2)
        :
        Vec3(vec2) {}
private:
    float m_Extra;
};
}


class CCarHandlingData: public CBaseSubHandlingData {
public:
    // vtable? (ptr)                                //0x0000
    float fBackEndPopUpCarImpulseMult;              //0x0008
    float fBackEndPopUpBuildingImpulseMult;         //0x000C
    float fBackEndPopUpMaxDeltaSpeed;               //0x0010
    float fToeFront;                                //0x0014
    float fToeRear;                                 //0x0018
    float fCamberFront;                             //0x001C
    float fCamberRear;                              //0x0020
    float fCastor;                                  //0x0024
    float fEngineResistance;                        //0x0028
    float fMaxDriveBiasTransfer;                    //0x002C // Seems to be ignored
    float fJumpForceScale;                          //0x0030
    float fIncreasedRammingForceScale;              //0x0034 // Always 1.0?
    uint32_t Unk_0x038;                             //0x0038 // Could be some hash? 0 when no strAdvancedFlags
    uint32_t strAdvancedFlags;                      //0x003C
    atArray<CAdvancedData> pAdvancedData;           //0x0040
};

class CBoatHandlingData: public CBaseSubHandlingData {
public:
    float fBoxFrontMult;                   //0x0008
    float fBoxRearMult;                    //0x000C
    float fBoxSideMult;                    //0x0010
    float fSampleTop;                      //0x0014
    float fSampleBottom;                   //0x0018
    float fSampleBottomTestCorrection;     //0x001C
    float fAquaplaneForce;                 //0x0020
    float fAquaplanePushWaterMult;         //0x0024
    float fAquaplanePushWaterCap;          //0x0028
    float fAquaplanePushWaterApply;        //0x002C
    float fRudderForce;                    //0x0030
    float fRudderOffsetSubmerge;           //0x0034
    float fRudderOffsetForce;              //0x0038
    float fRudderOffsetForceZMult;         //0x003C
    float fWaveAudioMult;                  //0x0040
    uint32_t unk_0x0044;
    uint32_t unk_0x0048;
    uint32_t unk_0x004C;
    rage::Vec3V vecMoveResistance;         //0x0050
    rage::Vec3V vecTurnResistance;         //0x0060
    float fLook_L_R_CamHeight;             //0x0070
    float fDragCoefficient;                //0x0074
    float fKeelSphereSize;                 //0x0078
    float fPropRadius;                     //0x007C
    float fLowLodAngOffset;                //0x0080
    float fLowLodDraughtOffset;            //0x0084
    float fImpellerOffset;                 //0x0088
    float fImpellerForceMult;              //0x008C
    float fDinghySphereBuoyConst;          //0x0090
    float fProwRaiseMult;                  //0x0094
    float fDeepWaterSampleBuoyancyMult;    //0x0098
    float fTransmissionMultiplier;         //0x009C
    float fTractionMultiplier;             //0x00A0
};

class CBikeHandlingData: public CBaseSubHandlingData {
public:
    float fLeanFwdCOMMult;                 //0x0008
    float fLeanFwdForceMult;               //0x000C
    float fLeanBakCOMMult;                 //0x0010
    float fLeanBakForceMult;               //0x0014
    float fMaxBankAngle;                   //0x0018
    float fFullAnimAngle;                  //0x001C
    float fUnk_0x0024;                     //0x0024
    float fDesLeanReturnFrac;              //0x0028
    float fStickLeanMult;                  //0x002C
    float fBrakingStabilityMult;           //0x0030
    float fInAirSteerMult;                 //0x0034
    float fWheelieBalancePoint;            //0x0038
    float fStoppieBalancePoint;            //0x003C
    float fWheelieSteerMult;               //0x0040
    float fRearBalanceMult;                //0x0044
    float fFrontBalanceMult;               //0x0048
    float fBikeGroundSideFrictionMult;     //0x004C
    float fBikeWheelGroundSideFrictionMult;//0x0050
    float fBikeOnStandLeanAngle;           //0x0054
    float fBikeOnStandSteerAngle;          //0x0058
    float fJumpForce;                      //0x005C
};

class CFlyingHandlingData: public CBaseSubHandlingData {
public:
    float fThrust;                              //0x0008
    float fThrustFallOff;                       //0x000C
    float fThrustVectoring;                     //0x0010
    float fInitialThrust;                       //0x0014
    float fInitialThrustFallOff;                //0x0018
    float fYawMult;                             //0x001C
    float fYawStabilise;                        //0x0020
    float fSideSlipMult;                        //0x0024
    float fInitialYawMult;                      //0x0028
    float fRollMult;                            //0x002C
    float fRollStabilise;                       //0x0030
    float fInitialRollMult;                     //0x0034
    float fPitchMult;                           //0x0038
    float fPitchStabilise;                      //0x003C
    float fInitialPitchMult;                    //0x0040
    float fFormLiftMult;                        //0x0044
    float fAttackLiftMult;                      //0x0048
    float fAttackDiveMult;                      //0x004C
    float fGearDownDragV;                       //0x0050
    float fGearDownLiftMult;                    //0x0054
    float fWindMult;                            //0x0058
    float fMoveRes;                             //0x005C
    rage::Vec3V vecTurnRes;                     //0x0060
    rage::Vec3V vecSpeedRes;                    //0x0070
    float fGearDoorFrontOpen;                   //0x0080
    float fGearDoorRearOpen;                    //0x0084
    float fGearDoorRearOpen2;                   //0x0088
    float fGearDoorRearMOpen;                   //0x008C
    float fTurublenceMagnitudeMax;              //0x0090
    float fTurublenceForceMulti;                //0x0094
    float fTurublenceRollTorqueMulti;           //0x0098
    float fTurublencePitchTorqueMulti;          //0x009C
    float fBodyDamageControlEffectMult;         //0x00A0
    float fInputSensitivityForDifficulty;       //0x00A4
    float fOnGroundYawBoostSpeedPeak;           //0x00A8
    float fOnGroundYawBoostSpeedCap;            //0x00AC
    float fEngineOffGlideMulti;                 //0x00B0
    float fAfterburnerEffectRadius;             //0x00B4
    float fAfterburnerEffectDistance;           //0x00B8
    float fAfterburnerEffectForceMulti;         //0x00BC
    float fSubmergeLevelToPullHeliUnderwater;   //0x00C0
    float fExtraLiftWithRoll;                   //0x00C4
    eHandlingType handlingType;                 //0x00C8
};

// CVehicleWeaponHandlingData__sTurretPitchLimits
struct sTurretPitchLimits {
    float fForwardAngleMin;
    float fForwardAngleMax;
    float fBackwardAngleMin;
    float fBackwardAngleMid;
    float fBackwardAngleMax;
    float fBackwardForcedPitch;
};

class CVehicleWeaponHandlingData: public CBaseSubHandlingData {
public:
    atFixedArray <Hash, 6>                           m_WeaponHash;                  // 0x0008
    atFixedArray<int, 6>                             m_WeaponSeats;                 // 0x0020
    atFixedArray<enum eVehicleModType, 6>            m_WeaponVehicleModType;        // 0x0038
    atFixedArray<float, 12>                          m_TurretSpeed;                 // 0x0050
    atFixedArray<float, 12>                          m_TurretPitchMin;              // 0x0080
    atFixedArray<float, 12>                          m_TurretPitchMax;              // 0x00B0
    atFixedArray<float, 12>                          m_TurretCamPitchMin;           // 0x00E0
    atFixedArray<float, 12>                          m_TurretCamPitchMax;           // 0x0110
    atFixedArray<float, 12>                          m_BulletVelocityForGravity;    // 0x0140
    atFixedArray<float, 12>                          m_TurretPitchForwardMin;       // 0x0170
    atFixedArray<struct sTurretPitchLimits, 12>      m_TurretPitchLimitData;        // 0x01A0
    float                                            m_UvAnimationMult;             // 0x0320
    float                                            m_MiscGadgetVar;               // 0x0324
    float                                            m_WheelImpactOffset;           // 0x0328
};

class CSubmarineHandlingData: public CBaseSubHandlingData {
public:
    uint32_t pad_0x0008;
    uint32_t pad_0x000C;
    rage::Vec3V vTurnRes;                  //0x0010
    float fMoveResXY;                      //0x0020
    float fMoveResZ;                       //0x0024
    float fPitchMult;                      //0x0028
    float fPitchAngle;                     //0x002C
    float fYawMult;                        //0x0030
    float fDiveSpeed;                      //0x0034
    float fRollMult;                       //0x0038
    float fRollStab;                       //0x003C
};

class CTrailerHandlingData: public CBaseSubHandlingData {
public:
    float fAttachLimitPitch;               //0x0008
    float fAttachLimitRoll;                //0x000C
    float fAttachLimitYaw;                 //0x0010
    float fUprightSpringConstant;          //0x0014
    float fUprightDampingConstant;         //0x0018
    float fAttachedMaxDistance;            //0x001C
    float fAttachedMaxPenetration;         //0x0020
    float fAttachRaiseZ;                   //0x0024
    float fPosConstraintMassRatio;         //0x0028
};

class CSeaPlaneHandlingData: public CBaseSubHandlingData {
public:
    int fLeftPontoonComponentId;                    //0x0008
    int fRightPontoonComponentId;                   //0x000C
    float fPontoonBuoyConst;                        //0x0010
    float fPontoonSampleSizeFront;                  //0x0014
    float fPontoonSampleSizeMiddle;                 //0x0018
    float fPontoonSampleSizeRear;                   //0x001C
    float fPontoonLengthFractionForSamples;         //0x0020
    float fPontoonDragCoefficient;                  //0x0024
    float fPontoonVerticalDampingCoefficientUp;     //0x0028
    float fPontoonVerticalDampingCoefficientDown;   //0x002C
    float fKeelSphereSize;                          //0x0030
};

class CSpecialFlightHandlingData: public CBaseSubHandlingData {
public:
    uint32_t pad_0x0008;
    uint32_t pad_0x000C;
    rage::Vec3V vecAngularDamping;         //0x0010
    rage::Vec3V vecAngularDampingMin;      //0x0020
    rage::Vec3V vecLinearDamping;          //0x0030
    rage::Vec3V vecLinearDampingMin;       //0x0040
    float fLiftCoefficient;                //0x0050
    float fCriticalLiftAngle;              //0x0054
    float fInitialLiftAngle;               //0x0058
    float fMaxLiftAngle;                   //0x005C
    float fDragCoefficient;                //0x0060
    float fBrakingDrag;                    //0x0064
    float fMaxLiftVelocity;                //0x0068
    float fMinLiftVelocity;                //0x006C
    float fRollTorqueScale;                //0x0070
    float fMaxTorqueVelocity;              //0x0074
    float fMinTorqueVelocity;              //0x0078
    float fYawTorqueScale;                 //0x007C
    float fSelfLevelingPitchTorqueScale;   //0x0080
    float fInitalOverheadAssist;           //0x0084
    float fMaxPitchTorque;                 //0x0088
    float fMaxSteeringRollTorque;          //0x008C
    float fPitchTorqueScale;               //0x0090
    float fSteeringTorqueScale;            //0x0094
    float fMaxThrust;                      //0x0098
    float fTransitionDuration;             //0x009C
    float fHoverVelocityScale;             //0x00A0
    float fStabilityAssist;                //0x00A4
    float fMinSpeedForThrustFalloff;       //0x00A8
    float fBrakingThrustScale;             //0x00AC
    int mode;                              //0x00B0
    uint32_t pad_0x00B4;
    uint32_t strFlags;                     //0x00B8
};

class CTrainHandlingData: public CBaseSubHandlingData {};
