file(GLOB SOURCE
    AscendclOriginTest.cpp
    AclrtSetDeviceImplTest.cpp
    AclrtResetDeviceImplTest.cpp
    AclrtMallocImplTest.cpp
    AclrtMallocCachedImplTest.cpp
    AclrtFreeImplTest.cpp
    AclrtMemsetImplTest.cpp
    AclrtMemsetAsyncImplTest.cpp
    AclrtMemcpyImplTest.cpp
    AclrtMemcpyAsyncImplTest.cpp
    AclrtMemcpy2dImplTest.cpp
    AclrtMemcpy2dAsyncImplTest.cpp
    AclrtMapMemImplTest.cpp
    AclrtUnmapMemImplTest.cpp
    AclmdlRICaptureBeginImplTest.cpp
    AclmdlRICaptureEndImplTest.cpp
)
list(SORT SOURCE)
target_sources(injection_test_obj PUBLIC ${SOURCE})
