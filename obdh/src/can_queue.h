#pragma once

#include "../../hal/src/can_interface.h"
#include <queue>
#include <mutex>
#include <cstdint>

// Fila thread-safe para frames CAN
// Padrão Producer-Consumer com mutex
class CanQueue {
public:

    // Insere frame na fila — thread-safe
    void push(const CanFrame& frame) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(frame);
    }

    // Remove e retorna o próximo frame — thread-safe
    // Retorna false se a fila estiver vazia
    bool pop(CanFrame& frame) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        frame = queue_.front();
        queue_.pop();
        return true;
    }

    // Verifica se está vazia — thread-safe
    bool isEmpty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    // Tamanho atual — thread-safe
    size_t size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    std::queue<CanFrame>  queue_;
    std::mutex            mutex_;
};
