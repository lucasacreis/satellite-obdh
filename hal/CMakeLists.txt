# HAL é uma biblioteca de interfaces 
add_library(hal INTERFACE)
target_include_directories(hal INTERFACE src mocks)

# Biblioteca SocketCAN — só compila para Linux ARM
add_library(socketcan_lib src/socketcan.cpp)
target_include_directories(socketcan_lib PUBLIC src)
target_link_libraries(socketcan_lib PUBLIC hal)
