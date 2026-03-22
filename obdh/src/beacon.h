#pragma once

#include "../../hal/src/can_interface.h"
#include "obdh.h"
#include "orbital_env.h"
#include <cstdint>

constexpr uint32_t CAN_ID_BEACON = 0x300;

class Beacon {
public:
    Beacon(ICanInterface& can, uint32_t interval_cycles);

    void tick();
    void setMode(SatelliteMode mode);
    void setTemperature(uint8_t temp);

    // Injeta ambiente orbital
    void setOrbitalEnvironment(OrbitalEnvironment* env) { env_ = env; }

private:
    ICanInterface&       can_;
    uint32_t             interval_;
    uint32_t             counter_;
    uint32_t             uptime_;
    SatelliteMode        mode_        = SatelliteMode::NOMINAL;
    uint8_t              temperature_ = 24;
    OrbitalEnvironment*  env_         = nullptr;

    void transmit();
    uint8_t buildStatus() const;
};
