# HAL é uma biblioteca de interfaces — só headers por enquanto
add_library(hal INTERFACE)
target_include_directories(hal INTERFACE src mocks)
