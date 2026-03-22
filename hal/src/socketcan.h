#pragma once

#include "can_interface.h"
#include <string>

// Implementação real do CAN via SocketCAN (Linux)
// Usada no QEMU e no hardware físico do satélite
class SocketCan : public ICanInterface {
public:
    explicit SocketCan(const std::string& interface);
    ~SocketCan();

    bool send(const CanFrame& frame) override;
    bool receive(CanFrame& frame) override;
    bool isAvailable() override;

    bool isOpen() const { return fd_ >= 0; }

private:
    int fd_ = -1;  // file descriptor do socket CAN
};
