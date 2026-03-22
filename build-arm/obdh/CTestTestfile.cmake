# CMake generated Testfile for 
# Source directory: /home/lucas/satellite/obdh
# Build directory: /home/lucas/satellite/build-arm/obdh
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ObdhTests "/home/lucas/satellite/build-arm/obdh/test_obdh")
set_tests_properties(ObdhTests PROPERTIES  _BACKTRACE_TRIPLES "/home/lucas/satellite/obdh/CMakeLists.txt;17;add_test;/home/lucas/satellite/obdh/CMakeLists.txt;0;")
add_test(BeaconTests "/home/lucas/satellite/build-arm/obdh/test_beacon")
set_tests_properties(BeaconTests PROPERTIES  _BACKTRACE_TRIPLES "/home/lucas/satellite/obdh/CMakeLists.txt;21;add_test;/home/lucas/satellite/obdh/CMakeLists.txt;0;")
add_test(ThreadingTests "/home/lucas/satellite/build-arm/obdh/test_threading")
set_tests_properties(ThreadingTests PROPERTIES  _BACKTRACE_TRIPLES "/home/lucas/satellite/obdh/CMakeLists.txt;29;add_test;/home/lucas/satellite/obdh/CMakeLists.txt;0;")
