cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "EngineSFML")
# set the project name
project(${PROJECT_NAME})
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )
set(SFML_PACKAGE_DIR "D:/trash/SFML-2.5.1")

#include SFML
set(SFML_DIR "${SFML_PACKAGE_DIR}/lib/cmake")
include_directories("${SFML_PACKAGE_DIR}/include")
link_directories("${SFML_PACKAGE_DIR}/lib")

# setup project
set ( MAIN_SRCS
	src/main.cpp
    src/shape.h
    src/shape.cpp
    src/scene.h
    src/scene.cpp
    src/rectangle.h
    src/rectangle.cpp    
	src/fps_lock.h
	src/fps_lock.cpp
)
source_group( "src" FILES ${MAIN_SRCS} )

# add the executable
add_executable(${PROJECT_NAME} ${MAIN_SRCS})

target_link_libraries(${PROJECT_NAME} debug sfml-graphics-d sfml-system-d sfml-window-d)
#target_link_libraries(${PROJECT_NAME} optimized sfml-graphics sfml-system sfml-window)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_PACKAGE_DIR}/bin"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)


