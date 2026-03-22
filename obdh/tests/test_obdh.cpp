#include <gtest/gtest.h>
#include "../src/obdh.h"
#include "../../hal/mocks/mock_can.h"

// Teste 1: OBDH envia telemetria ao receber comando
TEST(ObdhTest, EnviaTelemetriaAoReceberComando) {
    MockCan can;
    Obdh obdh(can);

    CanFrame comando;
    comando.id      = CAN_ID_COMMAND;
    comando.dlc     = 1;
    comando.data[0] = static_cast<uint8_t>(ObdhCommand::REQUEST_TELEMETRY);
    can.injectFrame(comando);

    obdh.update();

    EXPECT_TRUE(can.hasSentFrames());
    CanFrame telemetria = can.getLastSent();
    EXPECT_EQ(telemetria.id,      CAN_ID_TELEMETRY);
    EXPECT_EQ(telemetria.data[0], 0x01);
    EXPECT_EQ(telemetria.data[2], 24);  // temperatura padrão 24°C
}

// Teste 2: OBDH muda modo ao receber SET_MODE
TEST(ObdhTest, MudaModoAoReceberComando) {
    MockCan can;
    Obdh obdh(can);

    CanFrame comando;
    comando.id      = CAN_ID_COMMAND;
    comando.dlc     = 2;
    comando.data[0] = static_cast<uint8_t>(ObdhCommand::SET_MODE);
    comando.data[1] = static_cast<uint8_t>(SatelliteMode::SAFE);
    can.injectFrame(comando);

    obdh.update();

    EXPECT_EQ(obdh.getMode(), SatelliteMode::SAFE);
}

// Teste 3: OBDH reseta para modo nominal
TEST(ObdhTest, ResetaParaModoNominal) {
    MockCan can;
    Obdh obdh(can);

    // Primeiro muda para SAFE
    CanFrame set_mode;
    set_mode.id      = CAN_ID_COMMAND;
    set_mode.dlc     = 2;
    set_mode.data[0] = static_cast<uint8_t>(ObdhCommand::SET_MODE);
    set_mode.data[1] = static_cast<uint8_t>(SatelliteMode::SAFE);
    can.injectFrame(set_mode);
    obdh.update();
    EXPECT_EQ(obdh.getMode(), SatelliteMode::SAFE);

    // Depois reseta
    CanFrame reset;
    reset.id      = CAN_ID_COMMAND;
    reset.dlc     = 1;
    reset.data[0] = static_cast<uint8_t>(ObdhCommand::RESET);
    can.injectFrame(reset);
    obdh.update();

    EXPECT_EQ(obdh.getMode(),       SatelliteMode::NOMINAL);
    EXPECT_EQ(obdh.getTemperature(), 24);
}

// Teste 4: OBDH ignora frames com ID desconhecido
TEST(ObdhTest, IgnoraFrameComIdDesconhecido) {
    MockCan can;
    Obdh obdh(can);

    CanFrame ruido;
    ruido.id      = 0x999;  // ID desconhecido
    ruido.dlc     = 1;
    ruido.data[0] = 0xFF;
    can.injectFrame(ruido);

    obdh.update();

    EXPECT_FALSE(can.hasSentFrames());
}

// Teste 5: OBDH não faz nada quando barramento está vazio
TEST(ObdhTest, NaoFazNadaQuandoBusVazio) {
    MockCan can;
    Obdh obdh(can);

    obdh.update();

    EXPECT_FALSE(can.hasSentFrames());
}
