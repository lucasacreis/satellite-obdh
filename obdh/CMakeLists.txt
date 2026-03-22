# Testes do OBDH
add_library(obdh_lib
    src/obdh.cpp
    src/beacon.cpp
)

target_include_directories(obdh_lib PUBLIC src)
target_link_libraries(obdh_lib PUBLIC hal)

# Executável principal para o QEMU
add_executable(obdh_main src/main.cpp)
target_link_libraries(obdh_main obdh_lib socketcan_lib)

# Testes unitários (rodam no PC)
add_executable(test_obdh tests/test_obdh.cpp)
target_link_libraries(test_obdh obdh_lib GTest::GTest GTest::Main)
add_test(NAME ObdhTests COMMAND test_obdh)

add_executable(test_beacon tests/test_beacon.cpp)
target_link_libraries(test_beacon obdh_lib GTest::GTest GTest::Main)
add_test(NAME BeaconTests COMMAND test_beacon)
