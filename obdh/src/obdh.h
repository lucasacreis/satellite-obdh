#pragma once

#include "../../hal/src/can_interface.h"
#include <cstdint>

// Comandos reconhecidos pelo OBDH
enum class ObdhCommand : uint8_t {
    REQUEST_TELEMETRY = 0x01,
    SET_MODE          = 0x02,
    RESET             = 0x03
};

// Modos de operação do satélite
enum class SatelliteMode : uint8_t {
    NOMINAL  = 0x00,
    SAFE     = 0x01,
    MISSION  = 0x02
};

// IDs do barramento CAN
constexpr uint32_t CAN_ID_COMMAND   = 0x100;
constexpr uint32_t CAN_ID_TELEMETRY = 0x200;

class Obdh {
public:
    explicit Obdh(ICanInterface& can);

    // Loop principal — chama em cada ciclo
    void update();

    // Processa um frame recebido externamente (usado pelas threads)
    void processExternalFrame(const CanFrame& frame) {
        processCommand(frame);
    }

    // Getters para os testes inspecionarem estado interno
    SatelliteMode getMode() const { return mode_; }
    uint8_t getTemperature() const { return temperature_; }

    void processCommand(const CanFrame& frame);

private:
    ICanInterface& can_;
    SatelliteMode  mode_        = SatelliteMode::NOMINAL;
    uint8_t        temperature_ = 24;  // °C

    void sendTelemetry();
};
