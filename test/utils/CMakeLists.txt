file(GLOB SOURCE 
    PipeCallTest.cpp
    FileSystemTest.cpp
    KernelDumperTest.cpp
    ElfLoaderTest.cpp
    UstringTest.cpp
    InjectLoggerTest.cpp
    EnvironmentTest.cpp
    PathTest.cpp
)
list(SORT SOURCE)
target_sources(core_test_obj PRIVATE ${SOURCE})
