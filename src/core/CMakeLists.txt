set(CORE_SRC 
    types.h
    types.cpp
    logger.h
    logger.cpp
    file_system.h
    file_system.cpp
)
add_library( core ${CORE_SRC} )

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(core optimized sfml-graphics)

