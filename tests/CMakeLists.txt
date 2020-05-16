set(TESTS_SRC 
    main.cpp
    value_test.cpp
)


set(BINARY ${CMAKE_PROJECT_NAME}_tst)

add_executable(${BINARY} ${TESTS_SRC})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC core script_system gtest)