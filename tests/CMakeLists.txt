set(TESTS_SRC 
    main.cpp        
    script_system/script_tests.cpp
    script_system/vm_utils.cpp
    script_system/vm_scanner.cpp
    script_system/vm_interpret.cpp
    script_system/vm_benchmark.cpp
    math/utils_tests.cpp
)


set(BINARY ${CMAKE_PROJECT_NAME}_tst)

add_executable(${BINARY} ${TESTS_SRC})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC script_system core render gtest)