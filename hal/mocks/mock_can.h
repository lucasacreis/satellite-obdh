#pragma once

#include "../src/can_interface.h"
#include <queue>

// Mock do CAN para testes unitários
// Simula o hardware sem precisar do QEMU
class MockCan : public ICanInterface {
public:
    // Simula envio — guarda o frame numa fila
    bool send(const CanFrame& frame) override {
        sent_frames.push(frame);
        return true;
    }

    // Simula recebimento — retira da fila de frames injetados
    bool receive(CanFrame& frame) override {
        if (incoming_frames.empty()) return false;
        frame = incoming_frames.front();
        incoming_frames.pop();
        return true;
    }

    bool isAvailable() override {
        return !incoming_frames.empty();
    }

    // Métodos auxiliares para os testes
    void injectFrame(const CanFrame& frame) {
        incoming_frames.push(frame);
    }

    CanFrame getLastSent() {
        CanFrame f = sent_frames.front();
        sent_frames.pop();
        return f;
    }

    bool hasSentFrames() {
        return !sent_frames.empty();
    }

private:
    std::queue<CanFrame> sent_frames;
    std::queue<CanFrame> incoming_frames;
};
