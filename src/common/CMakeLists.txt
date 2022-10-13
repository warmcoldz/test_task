add_library(core_lib INTERFACE)
target_include_directories(core_lib INTERFACE include)
target_include_directories(core_lib INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(core_lib INTERFACE Boost::program_options)

add_executable(
    common_test
        test/core/endian_test.cpp
        test/core/network_reader_test.cpp
        test/protocol/record_builder_test.cpp
)

target_link_libraries(
    common_test
        core_lib
        gmock_main
)
