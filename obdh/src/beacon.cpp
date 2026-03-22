#include "beacon.h"

Beacon::Beacon(ICanInterface& can, uint32_t interval_cycles)
    : can_(can)
    , interval_(interval_cycles)
    , counter_(0)
    , uptime_(0)
{}

void Beacon::tick() {
    counter_++;
    if (counter_ >= interval_) {
        counter_ = 0;
        transmit();
    }
}

void Beacon::setMode(SatelliteMode mode) {
    mode_ = mode;
}

void Beacon::setTemperature(uint8_t temp) {
    temperature_ = temp;
}

void Beacon::transmit() {
    CanFrame frame;
    frame.id      = CAN_ID_BEACON;
    frame.dlc     = 4;
    frame.data[0] = buildStatus();
    frame.data[1] = static_cast<uint8_t>(mode_);
    frame.data[2] = temperature_;
    frame.data[3] = static_cast<uint8_t>(uptime_ & 0xFF);
    can_.send(frame);
    uptime_++;
}

uint8_t Beacon::buildStatus() const {
    switch (mode_) {
        case SatelliteMode::NOMINAL: return 0x01;  // OK
        case SatelliteMode::SAFE:    return 0x02;  // alerta
        case SatelliteMode::MISSION: return 0x03;  // em missão
        default:                     return 0xFF;  // desconhecido
    }
}
