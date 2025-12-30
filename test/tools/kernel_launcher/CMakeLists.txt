file(GLOB SOURCE 
    RuntimeApiTest.cpp
    KernelConfigParserTest.cpp
    KernelRunnerTest.cpp
    LauncherTest.cpp
)
list(SORT SOURCE)
target_sources(core_test_obj PRIVATE ${SOURCE})