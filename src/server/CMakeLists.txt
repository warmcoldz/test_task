set (
    SOURCES
        main.cpp
        listen_server.cpp
        client_session.cpp
        protocol_handler.cpp
        record_parser.cpp
)

add_executable(server ${SOURCES})

target_link_libraries(
    server
        core_lib
        pthread
        Boost::coroutine
)

target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
