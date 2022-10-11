set (SOURCES src/record_parser.cpp)

add_library(core_lib ${SOURCES})
target_include_directories(core_lib PUBLIC include)
target_include_directories(core_lib PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(core_lib INTERFACE Boost::program_options)
