file(GLOB INJECTION_SRC
    HijackedFuncOfMsprofNotifySetDevice.cpp
    HijackedFuncOfMsprofRegisterCallback.cpp
    HijackedFuncOfMsprofReportAdditionalInfo.cpp
    ProfOriginal.cpp
)

list(SORT INJECTION_SRC)

target_sources(injection_test_obj PUBLIC ${INJECTION_SRC})
target_sources(msopprof_injection PUBLIC ${INJECTION_SRC})
target_sources(mssanitizer_injection PUBLIC ${INJECTION_SRC})