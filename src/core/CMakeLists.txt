set(CORE_SRC 
    types.h
    types.cpp
    value.h
    value.cpp
    logger.h
    logger.cpp
    any_value.h
    value_impl.h
    value_impl.cpp
    m_assert.h
    file_system.h
    file_system.cpp
    object.h
    object.cpp
    callabel.h
)
add_library( core ${CORE_SRC} )

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(core optimized sfml-graphics)

