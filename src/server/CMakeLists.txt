set (SOURCES
    main.cpp
    listen_server.cpp
    client_session.cpp
    protocol_handler.cpp
)

add_executable(main ${SOURCES})

target_link_libraries(main
    Boost::coroutine
    pthread
    core_lib
)

target_include_directories(main PRIVATE ${Boost_INCLUDE_DIRS})
