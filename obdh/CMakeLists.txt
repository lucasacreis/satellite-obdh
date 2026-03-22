# Testes do OBDH
add_executable(test_obdh tests/test_obdh.cpp)

target_link_libraries(test_obdh
    hal
    GTest::GTest
    GTest::Main
)

add_test(NAME ObdhTests COMMAND test_obdh)
