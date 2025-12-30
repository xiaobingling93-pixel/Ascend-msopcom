file(GLOB SOURCE
    MsprofNotifySetDeviceTest.cpp
    MsprofRegisterCallbackTest.cpp
    MsprofReportAdditionalInfoTest.cpp
)
list(SORT SOURCE)
target_sources(injection_test_obj PUBLIC ${SOURCE})