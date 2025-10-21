cmake_minimum_required(VERSION 3.30)

set(TARGET Graphics)
string(TOUPPER ${TARGET} TARGET_NAME_UPPER)

add_library(${TARGET} SHARED)

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

# 소스 파일 설정
set(PUBLIC_EXTERNALS
	${EXTERNAL_SOURCE_DIR}/Imgui/imconfig.h
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui.h
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_demo.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_draw.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_internal.h
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_stdlib.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_stdlib.h
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_tables.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_widgets.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imstb_rectpack.h
	${EXTERNAL_SOURCE_DIR}/Imgui/imstb_textedit.h
	${EXTERNAL_SOURCE_DIR}/Imgui/imstb_truetype.h
)
set(PRIVATE_EXTERNALS
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_impl_dx12.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_impl_dx12.h
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_impl_win32.cpp
	${EXTERNAL_SOURCE_DIR}/Imgui/imgui_impl_win32.h
)
set(PUBLIC_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Camera.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Graphics.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/GraphicsImGui.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Graphics2D.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Graphics3D.h
)
set(PRIVATE_HEADERS
	${COMMON_SOURCE_DIR}/Type.h
	${EXTERNAL_SOURCE_DIR}/DirectX/d3dx12.h
	${EXTERNAL_SOURCE_DIR}/DirectX/DDSTextureLoader12.h
	${EXTERNAL_SOURCE_DIR}/DirectX/WICTextureLoader12.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/ConstantBuffer.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Descriptor.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/DescriptorManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Global.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics3DUtil.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Pch.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/PipelineState.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/RenderTarget.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/SwapChain.h
)
set(SOURCES
	${COMMON_SOURCE_DIR}/Type.cpp
	${EXTERNAL_SOURCE_DIR}/DirectX/DDSTextureLoader12.cpp
	${EXTERNAL_SOURCE_DIR}/DirectX/WICTextureLoader12.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Camera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/ConstantBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Descriptor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/DescriptorManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/DllMain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Global.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/GraphicsImGui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics2D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics3D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics3DUtil.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/PipelineState.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/RenderTarget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/SwapChain.cpp
)
source_group(External ${PUBLIC_EXTERNALS} ${PRIVATE_EXTERNALS})
source_group(Header ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
source_group(Source ${SOURCES})
target_sources(${TARGET}
	PUBLIC
		${PUBLIC_EXTERNALS}
		${PUBLIC_HEADERS}
	PRIVATE
		${PRIVATE_EXTERNALS}
		${PRIVATE_HEADERS}
		${SOURCES}
)

# 미리 컴파일된 헤더
target_precompile_headers(${TARGET} PRIVATE Src/Pch.h)

# 추가 포함 디렉토리
target_include_directories(${TARGET}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Include
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}
)

# 종속성
target_link_libraries(${TARGET} PUBLIC
	d2d1 d3d11 d3d12 d3dcompiler dwrite dxgi
)
target_link_libraries(${TARGET} PUBLIC Resource)
