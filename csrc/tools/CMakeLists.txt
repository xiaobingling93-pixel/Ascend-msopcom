if(NOT ${BUILD_TESTS})
  add_compile_options("-Werror")
endif()

add_subdirectory(kernel_launcher)
add_subdirectory(dfx_kernel)
