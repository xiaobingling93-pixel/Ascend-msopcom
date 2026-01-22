find_program(BISHENG_COMPILER bisheng)
if(NOT BISHENG_COMPILER)
    message(FATAL_ERROR "Compiler bisheng is not installed or not in PATH")
endif()

# setup toolchain for dfx kernel compiling
set(CMAKE_CXX_COMPILER bisheng)
set(CMAKE_LINKER llvm-ar)
set(CMAKE_CXX_LINK_FLAGS "")
set(CMAKE_CXX_FLAGS "")

# set aicore arch list for DFX kernel
set(CCE_AICORE_ARCH_LIST_CLEAR_L2CACHE
    "dav-c220-cube"
    "dav-m200"
    "dav-c310-cube"
)
set(CCE_AICORE_ARCH_EMPTY_KERNEL "dav-c220-vec")

set_property(GLOBAL PROPERTY DFX_KERNEL_PATH ${CMAKE_CURRENT_BINARY_DIR})
get_property(KERNEL_OUTPUT_PATH GLOBAL PROPERTY DFX_KERNEL_PATH)

message(STATUS "DFX L2cache kernel built for: ${CCE_AICORE_ARCH_LIST_CLEAR_L2CACHE}")
message(STATUS "DFX empty kernel built for: ${CCE_AICORE_ARCH_LIST_EMPTY_KERNEL}")

file(GLOB_RECURSE CLEAR_L2CACHE_KERNEL_FILES ClearL2Cache.cpp)
file(GLOB_RECURSE EMPTY_KERNEL_FILES EmptyKernel.cpp)

# @description compile dfx kernel files to virtual object.....................
function(compile_cce_object CCE_AICORE_ARCH DFX_KERNEL_OBJECT KERNEL_FILES)
    set(BISHENG_COMPILE_FLAGS
        "-xcce -std=c++17 -O2 --cce-aicore-only --cce-aicore-arch=${CCE_AICORE_ARCH} \
        -mllvm -cce-aicore-function-stack-size=16000 -mllvm -cce-aicore-record-overflow=false \
        --cce-auto-sync=on"
    )
    add_library(${DFX_KERNEL_OBJECT} OBJECT ${KERNEL_FILES})
    set_target_properties(${DFX_KERNEL_OBJECT} PROPERTIES COMPILE_OPTIONS "")
    set_target_properties(${DFX_KERNEL_OBJECT} PROPERTIES COMPILE_FLAGS ${BISHENG_COMPILE_FLAGS})
endfunction()

# @description link dfx kernel object to executable object
function(link_cce_object_to_executable MERGED_OBJECT DFX_KERNEL_OBJECT)
    add_custom_command(
        OUTPUT ${MERGED_OBJECT}
        VERBATIM
        COMMENT "Build merged dfx kernel object ${MERGED_OBJECT} from DFX kernel source files"
        DEPENDS ${DFX_KERNEL_OBJECT}
        DEPENDS ${KERNEL_FILES}
        COMMAND_EXPAND_LISTS
        COMMAND ld.lld -m aicorelinux -Ttext=0 $<TARGET_OBJECTS:${DFX_KERNEL_OBJECT}> -static -o ${KERNEL_OUTPUT_PATH}/${MERGED_OBJECT}
    )
endfunction()

foreach(CCE_AICORE_ARCH ${CCE_AICORE_ARCH_LIST_CLEAR_L2CACHE})
    set(DFX_KERNEL_OBJECT dfx_kernel_${CCE_AICORE_ARCH}_tmp)
    set(MERGED_OBJECT dfx_kernel_tmp_${CCE_AICORE_ARCH}.o)
    set(MERGED_TARGET dfx_kernel_tmp_${CCE_AICORE_ARCH})
    compile_cce_object(${CCE_AICORE_ARCH} ${DFX_KERNEL_OBJECT} "${CLEAR_L2CACHE_KERNEL_FILES}")
    link_cce_object_to_executable(${MERGED_OBJECT} ${DFX_KERNEL_OBJECT})
    add_custom_target(${MERGED_TARGET} ALL DEPENDS ${MERGED_OBJECT})
    install(FILES ${KERNEL_OUTPUT_PATH}/${MERGED_OBJECT} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib64)
endforeach()

function(delete_section_to_obj LAST_OBJ NEXT_OBJ SECTION_NAME)
    add_custom_command(
        OUTPUT ${NEXT_OBJ}
        VERBATIM
        COMMENT "Delete section ${SECTION_NAME} to ${LAST_OBJ}"
        DEPENDS ${LAST_OBJ}
        COMMAND llvm-objcopy --remove-section ${SECTION_NAME} ${LAST_OBJ}
        COMMAND cp ${LAST_OBJ} ${NEXT_OBJ}
    )
endfunction()

foreach(CCE_AICORE_ARCH ${CCE_AICORE_ARCH_LIST_CLEAR_L2CACHE})
    delete_section_to_obj(dfx_kernel_tmp_${CCE_AICORE_ARCH}.o dfx_kernel_${CCE_AICORE_ARCH}.o .ascend.meta.ClearL2Cache)
    set(DFX_TARGET dfx_kernel_${CCE_AICORE_ARCH})
    add_custom_target(${DFX_TARGET} ALL DEPENDS dfx_kernel_${CCE_AICORE_ARCH}.o)
    install(FILES ${KERNEL_OUTPUT_PATH}/${DFX_TARGET}.o DESTINATION ${CMAKE_INSTALL_PREFIX}/lib64)
endforeach()

set(DFX_EMPTY_KERNEL_OBJECT empty_kernel_${CCE_AICORE_ARCH_EMPTY_KERNEL}_tmp)
set(EMPTY_OBJECT empty_kernel_${CCE_AICORE_ARCH_EMPTY_KERNEL}.o)
set(EMPTY_TARGET empty_kernel_${CCE_AICORE_ARCH_EMPTY_KERNEL})
compile_cce_object(${CCE_AICORE_ARCH_EMPTY_KERNEL} ${DFX_EMPTY_KERNEL_OBJECT} "${EMPTY_KERNEL_FILES}")
link_cce_object_to_executable(${EMPTY_OBJECT} ${DFX_EMPTY_KERNEL_OBJECT})
add_custom_target(${EMPTY_TARGET} ALL DEPENDS ${EMPTY_OBJECT})
install(FILES ${KERNEL_OUTPUT_PATH}/${EMPTY_OBJECT} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib64)
