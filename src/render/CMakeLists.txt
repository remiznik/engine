set(RENDER_SRC 
    render.h
	render.cpp
	d3d12Utils/d3dUtil.h
	d3d12Utils/d3dUtil.cpp	
	d3d12Utils/d3dx12.h
	d3d12Utils/MathHelper.h
	d3d12Utils/MathHelper.cpp
	d3d12Utils/DDSTextureLoader.h
	d3d12Utils/DDSTextureLoader.cpp
	d3d12Utils/UploadBuffer.h
)
add_library( render ${RENDER_SRC} )


target_include_directories (render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(render debug core)