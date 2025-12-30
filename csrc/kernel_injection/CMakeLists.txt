
file(GLOB MSBIT_CPP MSBit.cpp)
add_library(msbit STATIC ${MSBIT_CPP})

target_include_directories(msbit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
install(TARGETS msbit
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib64)
target_sources(injection_test_obj PUBLIC ${MSBIT_CPP})
