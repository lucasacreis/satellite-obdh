#include <gtest/gtest.h>
#include "../../hal/mocks/mock_can.h"

// Teste 1: OBDH envia telemetria quando recebe comando
TEST(ObdhTest, EnviaTelemetriaAoReceberComando) {
    MockCan can;

    // Injeta um frame de comando no barramento
    CanFrame comando;
    comando.id      = 0x100;  // ID do comando de telemetria
    comando.dlc     = 1;
    comando.data[0] = 0x01;   // 0x01 = solicitar telemetria
    can.injectFrame(comando);

    // Verifica que o frame foi injetado
    EXPECT_TRUE(can.isAvailable());

    // Lê o comando
    CanFrame recebido;
    EXPECT_TRUE(can.receive(recebido));
    EXPECT_EQ(recebido.id, 0x100);
    EXPECT_EQ(recebido.data[0], 0x01);

    // Simula resposta de telemetria do OBDH
    CanFrame telemetria;
    telemetria.id      = 0x200;  // ID da telemetria
    telemetria.dlc     = 4;
    telemetria.data[0] = 0x01;   // status OK
    telemetria.data[1] = 0x00;   // modo nominal
    telemetria.data[2] = 0x18;   // temperatura: 24°C
    telemetria.data[3] = 0x00;
    can.send(telemetria);

    // Verifica que a telemetria foi enviada corretamente
    EXPECT_TRUE(can.hasSentFrames());
    CanFrame enviado = can.getLastSent();
    EXPECT_EQ(enviado.id, 0x200);
    EXPECT_EQ(enviado.data[0], 0x01);
    EXPECT_EQ(enviado.data[2], 0x18);
}

// Teste 2: OBDH não recebe nada quando barramento está vazio
TEST(ObdhTest, NaoRecebeQuandoBusVazio) {
    MockCan can;
    CanFrame frame;
    EXPECT_FALSE(can.receive(frame));
    EXPECT_FALSE(can.isAvailable());
}

// Teste 3: OBDH envia múltiplos frames em sequência
TEST(ObdhTest, EnviaMultiplosFrames) {
    MockCan can;

    CanFrame f1, f2;
    f1.id = 0x201; f1.dlc = 1; f1.data[0] = 0xAA;
    f2.id = 0x202; f2.dlc = 1; f2.data[0] = 0xBB;

    can.send(f1);
    can.send(f2);

    EXPECT_TRUE(can.hasSentFrames());
    EXPECT_EQ(can.getLastSent().id, 0x201);
    EXPECT_EQ(can.getLastSent().id, 0x202);
}
