set(CORE_SRC 
    types.h
    types.cpp
)
add_library( core ${CORE_SRC} )

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})