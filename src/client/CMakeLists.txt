set (
    SOURCES
        main.cpp
        client.cpp
        session.cpp
)

add_executable(client ${SOURCES})

target_link_libraries(
    client
        core_lib
        pthread
        Boost::coroutine
)

target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS})
