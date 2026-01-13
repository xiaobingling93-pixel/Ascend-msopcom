file(GLOB SOURCE
    BinaryInstrumentation.cpp
    Communication.cpp
    FuncSelector.cpp
    FunctionLoader.cpp
    LocalProcess.cpp
    RemoteProcess.cpp
    DomainSocket.cpp
    HijackedLayerManager.cpp
    Revision.cpp
)
list(SORT SOURCE)
target_sources(core_obj PUBLIC ${SOURCE})
