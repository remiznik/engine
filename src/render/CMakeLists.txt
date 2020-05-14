set(RENDER_SRC     
	d3d12Utils/d3dUtil.h
	d3d12Utils/d3dUtil.cpp	
	d3d12Utils/d3dx12.h
	d3d12Utils/MathHelper.h
	d3d12Utils/MathHelper.cpp
	d3d12Utils/DDSTextureLoader.h
	d3d12Utils/DDSTextureLoader.cpp
	d3d12Utils/UploadBuffer.h
	d3d12Utils/FrameResource.h
	d3d12Utils/FrameResource.cpp
	d3d12Utils/GeometryGenerator.h
	d3d12Utils/GeometryGenerator.cpp
	render.h
	render.cpp
	scene.h
	scene.cpp
	model.h
	model.cpp
	resource_manager.h
	resource_manager.cpp
)
add_library( render ${RENDER_SRC} )


target_include_directories (render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(render debug core)