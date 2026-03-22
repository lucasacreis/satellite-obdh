#include <iostream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <chrono>
#include <csignal>

#include "obdh.h"
#include "beacon.h"
#include "orbital_env.h"
#include "can_queue.h"
#include "../../hal/src/socketcan.h"

// Flag global de shutdown — atomic para ser thread-safe
std::atomic<bool> running{true};

// Handler de sinal para desligar graciosamente com Ctrl+C
void signalHandler(int) {
    std::cout << "\n[OBDH] Sinal recebido — desligando..." << std::endl;
    running = false;
}

// ─────────────────────────────────────────────
// Thread 1 — CAN Receiver
// Lê frames do barramento e coloca na fila
// ─────────────────────────────────────────────
void canReceiverThread(SocketCan& can, CanQueue& queue) {
    std::cout << "[RX] Thread iniciada" << std::endl;
    while (running) {
        CanFrame frame;
        if (can.receive(frame)) {
            queue.push(frame);
            std::cout << "[RX] Frame recebido — ID: 0x"
                      << std::hex << frame.id << std::dec << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "[RX] Thread encerrada" << std::endl;
}

// ─────────────────────────────────────────────
// Thread 2 — Command Processor
// Consome frames da fila e processa comandos
// ─────────────────────────────────────────────
void commandProcessorThread(Obdh& obdh, CanQueue& queue) {
    std::cout << "[CMD] Thread iniciada" << std::endl;
    while (running) {
        CanFrame frame;
        if (queue.pop(frame)) {
            std::cout << "[CMD] Processando comando ID: 0x"
                      << std::hex << frame.id << std::dec << std::endl;
            obdh.processExternalFrame(frame);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::cout << "[CMD] Thread encerrada" << std::endl;
}

// ─────────────────────────────────────────────
// Thread 3 — Beacon
// Transmite telemetria periodicamente
// ─────────────────────────────────────────────
void beaconThread(Beacon& beacon, OrbitalEnvironment& env) {
    std::cout << "[BCN] Thread iniciada" << std::endl;
    uint32_t cycle = 0;
    while (running) {
        env.tick();
        beacon.tick();

        if (cycle % 5 == 4) {
            uint32_t min = env.elapsedSeconds() / 60;
            uint32_t sec = env.elapsedSeconds() % 60;
            std::cout << "\n[BCN] Ciclo #" << (cycle + 1)
                      << " | Orbita: " << std::setw(2) << min << "m"
                      << std::setw(2) << sec << "s"
                      << " | " << (env.isEclipse() ? "ECLIPSE  " : "ILUMINADO")
                      << " | Temp: " << std::setw(3)
                      << static_cast<int>(env.boardTemperature()) << "C"
                      << " | Bat: "
                      << (env.batteryVoltage() / 10) << "."
                      << (env.batteryVoltage() % 10) << "V"
                      << " | Painel: " << std::setw(4)
                      << env.solarCurrent() << "mA"
                      << std::endl;
        }
        cycle++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "[BCN] Thread encerrada" << std::endl;
}

// ─────────────────────────────────────────────
// Main — inicializa e lança as threads
// ─────────────────────────────────────────────
int main() {
    std::signal(SIGINT, signalHandler);

    std::cout << "╔══════════════════════════════════════════╗\n"
              << "║   OBDH - Computador de Bordo v2.0        ║\n"
              << "║   Arquitetura Multi-Thread               ║\n"
              << "║   Simulador Orbital LEO (90min)          ║\n"
              << "╚══════════════════════════════════════════╝\n";

    // Conecta ao CAN
    SocketCan can("vcan0");
    if (!can.isOpen()) {
        std::cerr << "[ERRO] Nao foi possivel abrir vcan0" << std::endl;
        return 1;
    }
    std::cout << "[OBDH] CAN conectado em vcan0\n";

    // Inicializa subsistemas
    CanQueue           queue;
    OrbitalEnvironment env(10);
    Obdh               obdh(can);
    Beacon             beacon(can, 5);
    beacon.setOrbitalEnvironment(&env);

    std::cout << "[OBDH] Lancando threads...\n\n";

    // Lança as 3 threads
    std::thread rx  (canReceiverThread,      std::ref(can),    std::ref(queue));
    std::thread cmd (commandProcessorThread, std::ref(obdh),   std::ref(queue));
    std::thread bcn (beaconThread,           std::ref(beacon), std::ref(env));

    // Aguarda encerramento (Ctrl+C)
    rx.join();
    cmd.join();
    bcn.join();

    std::cout << "[OBDH] Desligado com sucesso\n";
    return 0;
}
