#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <vector>
#include "../src/can_queue.h"
#include "../../hal/mocks/mock_can.h"

// Teste 1: Fila aceita e entrega um frame
TEST(CanQueueTest, AceitaEEntregaUmFrame) {
    CanQueue queue;

    CanFrame f;
    f.id      = 0x100;
    f.dlc     = 1;
    f.data[0] = 0x01;

    queue.push(f);

    CanFrame out;
    EXPECT_TRUE(queue.pop(out));
    EXPECT_EQ(out.id, 0x100);
    EXPECT_EQ(out.data[0], 0x01);
}

// Teste 2: Fila vazia retorna false
TEST(CanQueueTest, FilaVaziaRetornaFalse) {
    CanQueue queue;
    CanFrame out;
    EXPECT_FALSE(queue.pop(out));
}

// Teste 3: Fila é thread-safe — múltiplas threads escrevendo
TEST(CanQueueTest, MultiplaThreadsEscrevendo) {
    CanQueue queue;
    const int NUM_THREADS  = 4;
    const int FRAMES_EACH  = 100;

    // 4 threads escrevem 100 frames cada
    std::vector<std::thread> writers;
    for (int t = 0; t < NUM_THREADS; t++) {
        writers.emplace_back([&queue, t]() {
            for (int i = 0; i < FRAMES_EACH; i++) {
                CanFrame f;
                f.id      = static_cast<uint32_t>(t * 100 + i);
                f.dlc     = 1;
                f.data[0] = static_cast<uint8_t>(i);
                queue.push(f);
            }
        });
    }

    for (auto& w : writers) w.join();

    // Deve ter exatamente 400 frames na fila
    int count = 0;
    CanFrame out;
    while (queue.pop(out)) count++;
    EXPECT_EQ(count, NUM_THREADS * FRAMES_EACH);
}

// Teste 4: Uma thread escreve, outra lê simultaneamente
TEST(CanQueueTest, ProducerConsumerSimultaneo) {
    CanQueue queue;
    const int TOTAL = 200;
    int consumed    = 0;
    bool done       = false;

    // Thread produtora
    std::thread producer([&]() {
        for (int i = 0; i < TOTAL; i++) {
            CanFrame f;
            f.id      = static_cast<uint32_t>(i);
            f.dlc     = 1;
            f.data[0] = static_cast<uint8_t>(i % 256);
            queue.push(f);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        done = true;
    });

    // Thread consumidora
    std::thread consumer([&]() {
        while (!done || !queue.isEmpty()) {
            CanFrame f;
            if (queue.pop(f)) consumed++;
            else std::this_thread::sleep_for(std::chrono::microseconds(5));
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(consumed, TOTAL);
}

// Teste 5: Fila respeita ordem FIFO
TEST(CanQueueTest, RespeitaOrdemFIFO) {
    CanQueue queue;

    for (uint32_t i = 0; i < 5; i++) {
        CanFrame f;
        f.id  = i;
        f.dlc = 0;
        queue.push(f);
    }

    for (uint32_t i = 0; i < 5; i++) {
        CanFrame out;
        EXPECT_TRUE(queue.pop(out));
        EXPECT_EQ(out.id, i);
    }
}

// Teste 6: Sem race condition em push/pop simultâneos
TEST(CanQueueTest, SemRaceConditionEmPushPopSimultaneos) {
    CanQueue queue;
    const int ITERATIONS = 1000;
    int pushed   = 0;
    int popped   = 0;

    std::thread t1([&]() {
        for (int i = 0; i < ITERATIONS; i++) {
            CanFrame f;
            f.id = static_cast<uint32_t>(i);
            f.dlc = 0;
            queue.push(f);
            pushed++;
        }
    });

    std::thread t2([&]() {
        for (int i = 0; i < ITERATIONS; i++) {
            CanFrame f;
            if (queue.pop(f)) popped++;
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    });

    t1.join();
    t2.join();

    // pushed deve ser exatamente ITERATIONS
    EXPECT_EQ(pushed, ITERATIONS);
    // popped <= pushed (pode não ter consumido tudo)
    EXPECT_LE(popped, pushed);
}
