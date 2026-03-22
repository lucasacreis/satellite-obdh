# Testes do OBDH
add_library(obdh_lib
    src/obdh.cpp
)

target_include_directories(obdh_lib PUBLIC src)
target_link_libraries(obdh_lib PUBLIC hal)

add_executable(test_obdh tests/test_obdh.cpp)

target_link_libraries(test_obdh
    obdh_lib
    GTest::GTest
    GTest::Main
)

add_test(NAME ObdhTests COMMAND test_obdh)