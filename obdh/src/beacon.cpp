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

void Beacon::setMode(SatelliteMode mode)     { mode_ = mode; }
void Beacon::setTemperature(uint8_t temp)    { temperature_ = temp; }

void Beacon::transmit() {
    // Se tiver ambiente orbital, usa dados reais
    if (env_) {
        temperature_ = static_cast<uint8_t>(env_->boardTemperature());
    }

    CanFrame frame;
    frame.id      = CAN_ID_BEACON;
    frame.dlc     = 8;
    frame.data[0] = buildStatus();
    frame.data[1] = static_cast<uint8_t>(mode_);
    frame.data[2] = temperature_;
    frame.data[3] = static_cast<uint8_t>(uptime_ & 0xFF);
    frame.data[4] = env_ ? env_->batteryVoltage()          : 84;
    frame.data[5] = env_ ? (env_->isEclipse() ? 0x00:0x01) : 0x01;
    frame.data[6] = env_ ? static_cast<uint8_t>(env_->solarCurrent() >> 8)   : 0;
    frame.data[7] = env_ ? static_cast<uint8_t>(env_->solarCurrent() & 0xFF) : 0;

    can_.send(frame);
    uptime_++;
}

uint8_t Beacon::buildStatus() const {
    switch (mode_) {
        case SatelliteMode::NOMINAL: return 0x01;
        case SatelliteMode::SAFE:    return 0x02;
        case SatelliteMode::MISSION: return 0x03;
        default:                     return 0xFF;
    }
}
