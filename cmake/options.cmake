
# 安全规范编译命令
add_compile_options("-fPIE")
add_compile_options("-std=c++11")
add_compile_options("-fPIC")
add_compile_options("-fstack-protector-all")
add_compile_options("-Wall")
add_compile_options("-D_FORTIFY_SOURCE=2")
add_compile_options("-fno-strict-aliasing")
add_compile_options("-O2")

if(NOT CMAKE_VERSION VERSION_LESS "3.13")
    add_link_options ("-Wl,-z,now")
    add_link_options ("-Wl,-z,relro")
    add_link_options ("-Wl,-z,noexecstack")
    add_link_options ("-pie")
    add_link_options ("-s") #strip
endif()

set(CMAKE_CXX_FLAGS "-Wno-builtin-macro-redefined -U__FILE__ -D__FILE__='\"$(notdir $(abspath $<))\"'")
