#include <gtest/gtest.h>
#include "../src/obdh.h"
#include "../src/beacon.h"
#include "../../hal/mocks/mock_can.h"

// Teste 1: Beacon é transmitido após intervalo configurado
TEST(BeaconTest, TransmiteAposIntervalo) {
    MockCan can;
    Beacon beacon(can, 3);  // intervalo de 3 ciclos

    beacon.tick();  // ciclo 1
    EXPECT_FALSE(can.hasSentFrames());

    beacon.tick();  // ciclo 2
    EXPECT_FALSE(can.hasSentFrames());

    beacon.tick();  // ciclo 3 — deve transmitir
    EXPECT_TRUE(can.hasSentFrames());
}

// Teste 2: Frame do beacon tem ID correto
TEST(BeaconTest, FrameTemIdCorreto) {
    MockCan can;
    Beacon beacon(can, 1);

    beacon.tick();

    CanFrame frame = can.getLastSent();
    EXPECT_EQ(frame.id, CAN_ID_BEACON);
}

// Teste 3: Beacon repete periodicamente
TEST(BeaconTest, RepetePeriodicamente) {
    MockCan can;
    Beacon beacon(can, 2);

    beacon.tick(); beacon.tick();  // 1ª transmissão
    EXPECT_TRUE(can.hasSentFrames());
    can.getLastSent();  // consome o frame

    beacon.tick(); beacon.tick();  // 2ª transmissão
    EXPECT_TRUE(can.hasSentFrames());
}

// Teste 4: Beacon carrega modo do satélite
TEST(BeaconTest, CarregaModoDoSatelite) {
    MockCan can;
    Beacon beacon(can, 1);

    beacon.setMode(SatelliteMode::SAFE);
    beacon.tick();

    CanFrame frame = can.getLastSent();
    EXPECT_EQ(frame.data[1], static_cast<uint8_t>(SatelliteMode::SAFE));
}

// Teste 5: Beacon carrega temperatura
TEST(BeaconTest, CarregaTemperatura) {
    MockCan can;
    Beacon beacon(can, 1);

    beacon.setTemperature(36);
    beacon.tick();

    CanFrame frame = can.getLastSent();
    EXPECT_EQ(frame.data[2], 36);
}

// Teste 6: Beacon incrementa uptime a cada transmissão
TEST(BeaconTest, IncrementaUptime) {
    MockCan can;
    Beacon beacon(can, 1);

    beacon.tick();
    CanFrame f1 = can.getLastSent();

    beacon.tick();
    CanFrame f2 = can.getLastSent();

    EXPECT_EQ(f2.data[3], f1.data[3] + 1);
}

// Teste 7: Beacon em modo SAFE tem status de alerta
TEST(BeaconTest, ModoSafeTéemStatusAlerta) {
    MockCan can;
    Beacon beacon(can, 1);

    beacon.setMode(SatelliteMode::SAFE);
    beacon.tick();

    CanFrame frame = can.getLastSent();
    EXPECT_EQ(frame.data[0], 0x02);  // 0x02 = alerta
}
