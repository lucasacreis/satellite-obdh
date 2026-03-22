#pragma once

#include <cstdint>
#include <vector>

// Frame CAN padrão
struct CanFrame {
    uint32_t id;           // ID do frame (11 ou 29 bits)
    uint8_t  dlc;          // Data Length Code (0-8 bytes)
    uint8_t  data[8];      // Dados do frame
};

// Interface abstrata do CAN
// O HAL real implementa via SocketCAN
// Os mocks implementam para testes
class ICanInterface {
public:
    virtual ~ICanInterface() = default;

    virtual bool send(const CanFrame& frame) = 0;
    virtual bool receive(CanFrame& frame) = 0;
    virtual bool isAvailable() = 0;
};
