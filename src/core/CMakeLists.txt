set(CORE_SRC 
    types.h
    types.cpp
    logger.h
    logger.cpp
    assert.h
    file_reader.h
    file_reader.cpp
    callabel.h
	m_math.h
	m_math.cpp
)
add_library( core ${CORE_SRC} )

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(core optimized sfml-graphics)

