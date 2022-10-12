add_library(core_lib INTERFACE)
target_include_directories(core_lib INTERFACE include)
target_include_directories(core_lib INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(core_lib INTERFACE Boost::program_options)
