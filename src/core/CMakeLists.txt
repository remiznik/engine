set(CORE_SRC 
    types.h
    types.cpp
    value.h
    value.cpp
    logger.h
    logger.cpp
)
add_library( core ${CORE_SRC} )
target_link_libraries(core optimized sfml-graphics)

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})