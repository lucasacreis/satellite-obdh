#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include "obdh.h"
#include "beacon.h"
#include "orbital_env.h"
#include "../../hal/src/socketcan.h"

void printBeaconStatus(const OrbitalEnvironment& env, uint32_t cycle) {
    uint32_t min = env.elapsedSeconds() / 60;
    uint32_t sec = env.elapsedSeconds() % 60;

    std::cout << "\n[BEACON] Ciclo #" << cycle
              << " | Orbita: " << std::setw(2) << min << "m"
              << std::setw(2) << sec << "s"
              << " | " << (env.isEclipse() ? "ECLIPSE  " : "ILUMINADO")
              << " | Temp placa: " << std::setw(4)
              << static_cast<int>(env.boardTemperature()) << "C"
              << " | Bateria: "
              << std::setw(2) << (env.batteryVoltage() / 10) << "."
              << (env.batteryVoltage() % 10) << "V"
              << " | Painel: " << std::setw(4)
              << env.solarCurrent() << "mA"
              << std::endl;
}

int main() {
    std::cout << "╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║   OBDH - Computador de Bordo v1.0        ║" << std::endl;
    std::cout << "║   Simulador Orbital LEO (90min)          ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;

    // Conecta ao CAN
    SocketCan can("vcan0");
    if (!can.isOpen()) {
        std::cerr << "[ERRO] Nao foi possivel abrir vcan0" << std::endl;
        return 1;
    }
    std::cout << "[OBDH] CAN conectado em vcan0" << std::endl;

    // Inicializa ambiente orbital
    // cycle_seconds=10 → cada ciclo representa 10s reais da órbita
    // A órbita de 90min completa em ~9min de simulação
    OrbitalEnvironment env(10);

    // Inicializa subsistemas
    Obdh   obdh(can);
    Beacon beacon(can, 5);  // beacon a cada 5 ciclos
    beacon.setOrbitalEnvironment(&env);

    std::cout << "[OBDH] Loop principal iniciado\n" << std::endl;

    uint32_t cycle = 0;
    while (true) {
        env.tick();
        obdh.update();
        beacon.tick();

        // Imprime status a cada beacon (ciclo 5)
        if (cycle % 5 == 4)
            printBeaconStatus(env, cycle + 1);

        cycle++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
