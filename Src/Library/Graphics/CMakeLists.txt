cmake_minimum_required(VERSION 3.30)

set(TARGET Graphics)
string(TOUPPER ${TARGET} TARGET_NAME_UPPER)

add_library(${TARGET} SHARED)

# 빌드 구성 확인
# if (BUILD_CONFIG STREQUAL NONE)
# 	message(FATAL_ERROR "BUILD_CONFIG가 설정되지 않았습니다. (CLIENT, SERVER, TOOL)")
# endif()

# 셰이더 컴파일
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader/Compile.cmake)
set(SHADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader/Model.vs
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader/Model.ps
)
compile_shader(${TARGET} SHADER_FILES)

# 내보내기 매크로
include(GenerateExportHeader)
generate_export_header(${TARGET}
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Export.h
	EXPORT_MACRO_NAME ${TARGET_NAME_UPPER}_API
)

# Imgui
target_sources(${TARGET}
	PUBLIC
		${CMAKE_SOURCE_DIR}/External/Imgui/imconfig.h
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui.h
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_demo.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_draw.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_internal.h
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_stdlib.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_stdlib.h
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_tables.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_widgets.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imstb_rectpack.h
		${CMAKE_SOURCE_DIR}/External/Imgui/imstb_textedit.h
		${CMAKE_SOURCE_DIR}/External/Imgui/imstb_truetype.h
	PRIVATE
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_impl_dx12.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_impl_dx12.h
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_impl_win32.cpp
		${CMAKE_SOURCE_DIR}/External/Imgui/imgui_impl_win32.h
)

target_sources(${TARGET}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Camera.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Graphics.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/GraphicsImGui.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Graphics2D.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Graphics3D.h
	PRIVATE
		${CMAKE_SOURCE_DIR}/Src/Common/Type.cpp
		${CMAKE_SOURCE_DIR}/Src/Common/Type.h
		${CMAKE_SOURCE_DIR}/External/DirectX/d3dx12.h
		${CMAKE_SOURCE_DIR}/External/DirectX/DDSTextureLoader12.cpp
		${CMAKE_SOURCE_DIR}/External/DirectX/DDSTextureLoader12.h
		${CMAKE_SOURCE_DIR}/External/DirectX/WICTextureLoader12.cpp
		${CMAKE_SOURCE_DIR}/External/DirectX/WICTextureLoader12.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Camera.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/ConstantBuffer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/ConstantBuffer.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Descriptor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Descriptor.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/DescriptorManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/DescriptorManager.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/DllMain.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Global.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Global.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/GraphicsImGui.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics2D.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics3D.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics3DUtil.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics3DUtil.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/PipelineState.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/PipelineState.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/RenderTarget.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/RenderTarget.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/SwapChain.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/SwapChain.h
)

target_precompile_headers(${TARGET} PRIVATE Src/Pch.h)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

# 종속성
target_link_libraries(${TARGET} PUBLIC
	d2d1 d3d11 d3d12 d3dcompiler dwrite dxgi
)
target_link_libraries(${TARGET} PUBLIC Resource)
