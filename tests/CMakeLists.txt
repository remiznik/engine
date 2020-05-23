set(TESTS_SRC 
    main.cpp
    value_test.cpp
    script_test.cpp
    vm_test.cpp
    vm_utils.cpp
)


set(BINARY ${CMAKE_PROJECT_NAME}_tst)

add_executable(${BINARY} ${TESTS_SRC})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC script_system core gtest)