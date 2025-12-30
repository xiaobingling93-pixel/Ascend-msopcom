file(GLOB ASCEND_HAL
        AscendHalOrigin.cpp
        HijackedFuncOfHalGetSocVersion.cpp
        )
list(SORT ASCEND_HAL)
target_sources(injection_test_obj PUBLIC ${ASCEND_HAL})
target_sources(msopprof_injection PUBLIC ${ASCEND_HAL})
target_sources(mssanitizer_injection PUBLIC ${ASCEND_HAL})
