set(GRAPHICS_SRC 
    shape.h
    shape.cpp
    scene.h
    scene.cpp
    rectangle.h
    rectangle.cpp
)
add_library( graphics ${GRAPHICS_SRC} )


target_include_directories (graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

