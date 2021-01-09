set(SCRIPT_SYSTEM_SRC     
    vm/vm.h
    vm/vm.cpp
    vm/common.h
    vm/common.cpp
    vm/chunk.h
    vm/chunk.cpp
    vm/memory.h
    vm/memory.cpp
    vm/debug.h
    vm/debug.cpp
    vm/value.h
    vm/value.cpp
    vm/utils.h
    vm/utils.cpp
    vm/run.h
    vm/run.cpp
    vm/compiler.h
    vm/compiler.cpp
    vm/scanner.h
    vm/scanner.cpp
	vm/object.h
	vm/object.cpp
    vm/table.h
    vm/table.cpp
    script.h
    script.cpp
)
add_library( script_system ${SCRIPT_SYSTEM_SRC} )


target_link_libraries(script_system debug core)

target_include_directories (script_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})