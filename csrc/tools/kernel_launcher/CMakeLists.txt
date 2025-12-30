set(BIN_OUTPUT_NAME kernel-launcher)

# kernel_launcher_obj 作为单独的target object承载除main.cpp以外的源码文件，同时服务于构建可执行二进制与UT测试
add_library(kernel_launcher_obj OBJECT
        KernelConfigParser.cpp
        RuntimeApi.cpp
        KernelRunner.cpp
        Launcher.cpp
)

add_dependencies(kernel_launcher_obj core_obj)

target_include_directories(kernel_launcher_obj PUBLIC
        ${JSON_DIR}
        ${HOST_INJECTION_CSRC}
        ${INJECTION_THIRDPARTY_INCLUDE}
)

target_link_libraries(kernel_launcher_obj
        $<TARGET_OBJECTS:core_obj>
        stdc++fs
        kernel_launcher_runtime
        kernel_launcher_acl
        dl
        pthread
)

# kernel_launcher_runtime 构造了一个空实现的runtime库，提供给kernel-launcher用于编译时链接
add_library(
        kernel_launcher_runtime SHARED 
        Runtime.cpp
)

# kernel_launcher_acl 构造了一个空实现的runtime库，提供给kernel-launcher用于编译时链接
add_library(
        kernel_launcher_acl SHARED
        AscendCl.cpp
)
set_target_properties(kernel_launcher_runtime PROPERTIES OUTPUT_NAME runtime)
set_target_properties(kernel_launcher_acl PROPERTIES OUTPUT_NAME acl_rt)

target_include_directories(kernel_launcher_runtime PUBLIC
        ${HOST_INJECTION_CSRC}
        ${INJECTION_THIRDPARTY_INCLUDE}
)

target_include_directories(kernel_launcher_acl PUBLIC
        ${HOST_INJECTION_CSRC}
        ${INJECTION_THIRDPARTY_INCLUDE}
)

# kernel_launcher_bin 用于构建可执行二进制
add_executable(kernel_launcher_bin
        main.cpp
)

add_dependencies(kernel_launcher_bin kernel_launcher_obj)

target_include_directories(kernel_launcher_bin PUBLIC
        ${JSON_DIR}
        ${HOST_INJECTION_CSRC}
        ${INJECTION_THIRDPARTY_INCLUDE}
)

target_link_libraries(kernel_launcher_bin
        $<TARGET_OBJECTS:kernel_launcher_obj>
        $<TARGET_OBJECTS:core_obj>
        stdc++fs
        kernel_launcher_runtime
        kernel_launcher_acl
        dl
        pthread
)

set_target_properties(kernel_launcher_bin PROPERTIES OUTPUT_NAME ${BIN_OUTPUT_NAME})

install(TARGETS         kernel_launcher_bin
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/bin
)
