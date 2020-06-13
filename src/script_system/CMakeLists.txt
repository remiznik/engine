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
    interpreter.h
    interpreter.cpp
    environment.h
    environment.cpp
    resolver.h
    resolver.cpp
    class_def.h
    class_def.cpp
    vm/vm.h
    vm/vm.cpp
    vm/common.h
    vm/common.cpp
    vm/chunk.h
    vm/chunk.cpp
    vm/vm_memory.h
    vm/vm_memory.cpp
    vm/vm_debug.h
    vm/vm_debug.cpp
    vm/vm_value.h
    vm/vm_value.cpp
    vm/utils.h
    vm/utils.cpp
    vm/vm_run.h
    vm/vm_run.cpp
    vm/compiler.h
    vm/compiler.cpp
    vm/scanner.h
    vm/scanner.cpp
)
add_library( script_system ${SCRIPT_SYSTEM_SRC} )


target_link_libraries(script_system debug core)

target_include_directories (script_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})