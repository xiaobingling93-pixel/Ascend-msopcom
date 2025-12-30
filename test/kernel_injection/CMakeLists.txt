file(GLOB SOURCE
    MSBitTest.cpp
)
list(SORT SOURCE)
target_sources(core_test_obj PRIVATE ${SOURCE})
