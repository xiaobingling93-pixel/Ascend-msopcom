if (NOT DEFINED GOOGLETEST)
    # EXCLUDE_FROM_ALL 禁用 gtest 的 install 功能，保持 output 目录干净
    add_subdirectory(googletest EXCLUDE_FROM_ALL)
    set(GOOGLETEST TRUE)
endif()

if (NOT DEFINED MOCKCPP)
    include_directories(boost/)
    # EXCLUDE_FROM_ALL 禁用 mockcpp 的 install 功能，保持 output 目录干净
    add_subdirectory(mockcpp EXCLUDE_FROM_ALL)
    set(MOCKCPP TRUE)
endif()
