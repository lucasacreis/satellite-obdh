#pragma once

#include "../../hal/src/can_interface.h"
#include "obdh.h"
#include <cstdint>

constexpr uint32_t CAN_ID_BEACON = 0x300;

class Beacon {
public:
    Beacon(ICanInterface& can, uint32_t interval_cycles);

    // Chama a cada ciclo do sistema
    void tick();

    // Atualiza estado do satélite no beacon
    void setMode(SatelliteMode mode);
    void setTemperature(uint8_t temp);

private:
    ICanInterface& can_;
    uint32_t       interval_;   // ciclos entre transmissões
    uint32_t       counter_;    // contador atual
    uint32_t       uptime_;     // número de transmissões
    SatelliteMode  mode_        = SatelliteMode::NOMINAL;
    uint8_t        temperature_ = 24;

    void transmit();
    uint8_t buildStatus() const;
};
