set (SOURCES
    source/listen_server.cpp
    source/client_session.cpp
    source/protocol_handler.cpp
    source/record_parser.cpp
)

add_library(server_lib ${SOURCES})

target_link_libraries(server_lib PUBLIC
    core_lib
    pthread
    Boost::coroutine)

target_include_directories(server_lib PUBLIC ${Boost_INCLUDE_DIRS})

add_executable(server source/main.cpp)
target_link_libraries(server server_lib)

add_executable(server_test
    test/record_parser_test.cpp
    test/protocol_handler_test.cpp
)

target_link_libraries(server_test server_lib gmock_main)
target_include_directories(server_test PRIVATE .)
