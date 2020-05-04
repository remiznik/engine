cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "EngineSFML")
# set the project name
project(${PROJECT_NAME})
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )


set(LIB_NAME "lib")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LIB_NAME "lib")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory ("src/core")
add_subdirectory ("src/graphics")
add_subdirectory ("src/script_system")
add_subdirectory ("src/render")


#tests 
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("third_party/googletest")
add_subdirectory ("tests")

# setup project
set ( MAIN_SRCS
	src/main.cpp    
	src/fps_lock.cpp    
	src/application.h
	src/application.cpp
)
source_group( "main" FILES ${MAIN_SRCS} )

# add the executable
add_executable(${PROJECT_NAME} ${MAIN_SRCS})


target_link_libraries(${PROJECT_NAME} optimized core script_system graphics )
target_link_libraries(${PROJECT_NAME} debug core script_system graphics)



