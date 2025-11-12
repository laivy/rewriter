cmake_minimum_required(VERSION 3.30)

set(target_name "Graphics")
add_library(${target_name} SHARED)

include(${RWT_CMAKE_DIR}/CompileShader.cmake)
set(shaders
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader/Model.vs
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader/Model.ps
)
target_compile_shader(${target_name} ${shaders})

include(GenerateExportHeader)
string(TOUPPER ${target_name} target_name_upper)
generate_export_header(${target_name}
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Export.h
	EXPORT_MACRO_NAME ${target_name_upper}_API
)

set(public_sources
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Export.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Graphics.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/ImGui.h
)
source_group("Include/${target_name}" FILES ${public_sources})
target_sources(${target_name} PUBLIC ${public_sources})

set(sources
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Context.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Context.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Descriptor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Descriptor.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/ImGui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Pch.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/SwapChain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/SwapChain.h
)
target_sources(${target_name} PRIVATE ${sources})
source_group("Src" FILES ${sources})

set(common_sources
	${RWT_COMMON_SOURCE_DIR}/Type.cpp
	${RWT_COMMON_SOURCE_DIR}/Type.h
)
target_sources(${target_name} PRIVATE ${common_sources})
source_group("Src/Common" FILES ${common_sources})

include(FetchContent)
FetchContent_Populate(
	DirectX-Headers
	GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
	GIT_TAG dde59d560da2760fec612d6634124edc2a26b82f # 1.618.2
	SOURCE_DIR ${RWT_EXTERNAL_DIR}/DirectX-Headers
)
target_sources(${target_name} PRIVATE
	${directx-headers_SOURCE_DIR}/include/directx/d3dx12.h
)
target_include_directories(${target_name} PRIVATE
	${directx-headers_SOURCE_DIR}/include
)
source_group("External/DirectX-Headers" FILES
	${directx-headers_SOURCE_DIR}/include/directx/d3dx12.h
)

FetchContent_Populate(
	DirectXTex
	GIT_REPOSITORY https://github.com/microsoft/DirectXTex.git
	GIT_TAG 232bbf2e39a37ea2e9b7b8331653fa69bd4797a2 # oct2025
	SOURCE_DIR ${RWT_EXTERNAL_DIR}/DirectXTex
)
target_compile_definitions(${target_name} PRIVATE USING_DIRECTX_HEADERS)
target_sources(${target_name} PRIVATE
	# ${directxtex_SOURCE_DIR}/DDSTextureLoader/DDSTextureLoader12.cpp
	# ${directxtex_SOURCE_DIR}/DDSTextureLoader/DDSTextureLoader12.h
	${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader12.cpp
	${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader12.h
)
target_include_directories(${target_name} PRIVATE
	${directxtex_SOURCE_DIR}
)
source_group("External/DirectXTex" FILES
	# ${directxtex_SOURCE_DIR}/DirectXTex/DDSTextureLoader/DDSTextureLoader12.cpp
	# ${directxtex_SOURCE_DIR}/DirectXTex/DDSTextureLoader/DDSTextureLoader12.h
	${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader12.cpp
	${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader12.h
)

FetchContent_Populate(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG e7d2d636affeeac1fc7175fb13472ca02d1bc0d1 # v1.92.4-docking
	SOURCE_DIR ${RWT_EXTERNAL_DIR}/imgui
)
target_compile_definitions(${target_name} PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
target_sources(${target_name} PRIVATE
	${imgui_SOURCE_DIR}/imconfig.h
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui.h
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_internal.h
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imstb_rectpack.h
	${imgui_SOURCE_DIR}/imstb_textedit.h
	${imgui_SOURCE_DIR}/imstb_truetype.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_win32.h
)
target_include_directories(${target_name} PRIVATE
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)
target_precompile_headers(${target_name} PRIVATE "Src/Pch.h")
source_group("Gen" FILES
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.cxx"
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.hxx"
)

target_include_directories(${target_name}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}"
)

target_link_libraries(${target_name}
	PRIVATE "d2d1" "d3d11" "d3d12" "dwrite" "dxgi" "dxguid" "Resource"
)
source_group("Library/Resource" "${RWT_SOURCE_DIR}/Library/Resource/Include/Resource/.*\\.h$")
