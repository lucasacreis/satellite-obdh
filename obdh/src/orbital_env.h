#pragma once

#include <cstdint>
#include <cmath>

// Simula o ambiente orbital LEO
// Órbita completa: 90 minutos = 5400 segundos
// Eclipse: ~35 min | Iluminação: ~55 min
class OrbitalEnvironment {
public:
    explicit OrbitalEnvironment(uint32_t cycle_seconds = 1)
        : cycle_seconds_(cycle_seconds)
        , elapsed_seconds_(0)
    {}

    // Avança um ciclo
    void tick() {
        elapsed_seconds_ += cycle_seconds_;
        if (elapsed_seconds_ >= ORBIT_PERIOD_S)
            elapsed_seconds_ = 0;
    }

    // Posição na órbita (0.0 a 1.0)
    float orbitalPhase() const {
        return static_cast<float>(elapsed_seconds_) / ORBIT_PERIOD_S;
    }

    // true se estiver na zona de eclipse
    bool isEclipse() const {
        float phase = orbitalPhase();
        return phase >= ECLIPSE_START && phase < ECLIPSE_END;
    }

    // Temperatura do painel solar (-40°C a +80°C)
    int8_t solarPanelTemperature() const {
        float phase = orbitalPhase();
        if (isEclipse()) {
            // Resfriamento durante eclipse
            float t = (phase - ECLIPSE_START) / (ECLIPSE_END - ECLIPSE_START);
            return static_cast<int8_t>(-40 + (1.0f - t) * 60.0f);
        }
        // Aquecimento durante iluminação
        float t = phase / ECLIPSE_START;
        float sine = std::sin(t * M_PI);
        return static_cast<int8_t>(-10 + sine * 90.0f);
    }

    // Temperatura interna do computador de bordo (15°C a 45°C)
    int8_t boardTemperature() const {
        float solar = static_cast<float>(solarPanelTemperature());
        // Temperatura interna varia menos — amortecida pelo chassi
        return static_cast<int8_t>(25 + solar * 0.15f);
    }

    // Tensão da bateria (6.5V a 8.4V — representada em décimos)
    // Ex: retorna 84 = 8.4V
    uint8_t batteryVoltage() const {
        if (isEclipse()) {
            // Descarga durante eclipse
            float t = (orbitalPhase() - ECLIPSE_START)
                    / (ECLIPSE_END - ECLIPSE_START);
            return static_cast<uint8_t>(84 - t * 19);  // 8.4V → 6.5V
        }
        // Recarga durante iluminação
        float t = orbitalPhase() / ECLIPSE_START;
        return static_cast<uint8_t>(65 + t * 19);      // 6.5V → 8.4V
    }

    // Corrente dos painéis solares em mA (0 ou até 1200mA)
    uint16_t solarCurrent() const {
        if (isEclipse()) return 0;
        float phase = orbitalPhase() / ECLIPSE_START;
        float sine  = std::sin(phase * M_PI);
        return static_cast<uint16_t>(sine * 1200);
    }

    uint32_t elapsedSeconds() const { return elapsed_seconds_; }

private:
    static constexpr uint32_t ORBIT_PERIOD_S = 5400;  // 90 min
    static constexpr float    ECLIPSE_START  = 0.61f; // 55 min
    static constexpr float    ECLIPSE_END    = 1.00f; // 90 min

    uint32_t cycle_seconds_;
    uint32_t elapsed_seconds_;
};
