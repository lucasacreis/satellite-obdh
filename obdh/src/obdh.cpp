#include "obdh.h"

Obdh::Obdh(ICanInterface& can) : can_(can) {}

void Obdh::update() {
    CanFrame frame;
    if (can_.receive(frame)) {
        processCommand(frame);
    }
}

void Obdh::processCommand(const CanFrame& frame) {
    if (frame.id != CAN_ID_COMMAND) return;

    switch (static_cast<ObdhCommand>(frame.data[0])) {
        case ObdhCommand::REQUEST_TELEMETRY:
            sendTelemetry();
            break;

        case ObdhCommand::SET_MODE:
            mode_ = static_cast<SatelliteMode>(frame.data[1]);
            break;

        case ObdhCommand::RESET:
            mode_        = SatelliteMode::NOMINAL;
            temperature_ = 24;
            break;

        default:
            break;
    }
}

void Obdh::sendTelemetry() {
    CanFrame telemetria;
    telemetria.id      = CAN_ID_TELEMETRY;
    telemetria.dlc     = 4;
    telemetria.data[0] = 0x01;                         // status OK
    telemetria.data[1] = static_cast<uint8_t>(mode_);  // modo atual
    telemetria.data[2] = temperature_;                  // temperatura
    telemetria.data[3] = 0x00;
    can_.send(telemetria);
}
