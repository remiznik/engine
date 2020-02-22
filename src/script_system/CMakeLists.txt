set(SCRIPT_SYSTEM_SRC 
    token.h
    token.cpp
    scanner.h
    scanner.cpp
    script.h
    script.cpp
)
add_library( script_system ${SCRIPT_SYSTEM_SRC} )

target_include_directories (script_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})