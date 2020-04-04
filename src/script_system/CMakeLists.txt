set(SCRIPT_SYSTEM_SRC 
    token.h
    token.cpp
    scanner.h
    scanner.cpp
    script.h
    script.cpp
    ast.h
    ast.cpp
    ast_visitor.h
    ast_printer.h
    ast_printer.cpp
    parser.h
    parser.cpp
)
add_library( script_system ${SCRIPT_SYSTEM_SRC} )


target_link_libraries(script_system debug core)

target_include_directories (script_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})