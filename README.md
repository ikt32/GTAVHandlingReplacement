GTA V Handling Replacement
==========================

A library/tool for script developers, to manage vehicle handling changes in non-intrusive way. It clones the handling of a vehicle, and assign the copy to the vehicle, so only that vehicle is affected by changes to its handling data.

This is useful for scripts that want to continuously vary certain handling parameters, without affecting other instances of that vehicle model.

An example is [my variable drive bias, which changes fDriveBiasFront constantly](https://www.youtube.com/watch?v=XpsykqjwiMM).

## End User Requirements

* Grand Theft Auto V (v1868+)

## Development

Check `HandlingReplacement.h` for the three functions. The `asi` loader should already have loaded and initialized things, so one can just start using it right away.

Usage example:

```cpp
void MyFeature::Update() {
    void* handlingDataOrig = nullptr;
    void* handlingDataReplace = nullptr;
    
    if (!HR_GetHandlingData(g_playerVehicle, &handlingDataOrig, &handlingDataReplace)) {
        HR_Enable(g_playerVehicle, &handlingDataReplace);
        return;
    }

    // Write whatever field is needed
    *(float*)((uint8_t*)handlingDataReplace + fInitialDriveForceOffset) = 1.0f;
}
```

## Acknowledgements

[FiveM](https://github.com/citizenfx/fivem) - On some bits on how to properly replace/clone subhandlingdata, and the rage alloc/free.

