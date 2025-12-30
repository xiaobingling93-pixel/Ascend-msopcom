file(GLOB SOURCE
        AdumpGetDFXInfoAddrForDynamicTest.cpp
)
list(SORT SOURCE)
target_sources(injection_test_obj PUBLIC ${SOURCE})
