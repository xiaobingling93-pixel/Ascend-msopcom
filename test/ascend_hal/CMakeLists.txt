file(GLOB SOURCE
        HalGetSocVersionTest.cpp
        AscendHalOriginTest.cpp
)
list(SORT SOURCE)
target_sources(injection_test_obj PUBLIC ${SOURCE})
