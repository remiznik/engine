cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "EngineSFML")
# set the project name
project(${PROJECT_NAME})
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )
set(SFML_PACKAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../SFML-2.5.1")
message(" SFML = ${SFML_PACKAGE_DIR}")
if(EXISTS "${SFML_PACKAGE_DIR}")
   message(" exists ")
else()
   message(" not exists ")
endif()

set(LIB_NAME "lib")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LIB_NAME "lib")
endif()
#include SFML
set(SFML_DIR "${SFML_PACKAGE_DIR}/${LIB_NAME}/cmake")
include_directories("${SFML_PACKAGE_DIR}/include")
link_directories("${SFML_PACKAGE_DIR}/${LIB_NAME}")
message(" lib ${SFML_PACKAGE_DIR}/${LIB_NAME}" )

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory ("src/core")
add_subdirectory ("src/graphics")
add_subdirectory ("src/script_system")

# setup project
set ( MAIN_SRCS
	src/main.cpp    
	src/fps_lock.h
	src/fps_lock.cpp    
)
source_group( "main" FILES ${MAIN_SRCS} )

# add the executable
add_executable(${PROJECT_NAME} ${MAIN_SRCS})


target_link_libraries(${PROJECT_NAME} debug core script_system graphics)
target_link_libraries(${PROJECT_NAME} optimized core script_system graphics sfml-graphics sfml-system sfml-window)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SFML_PACKAGE_DIR}/bin"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()


