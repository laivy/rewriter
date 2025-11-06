cmake_minimum_required(VERSION 3.30)

set(target_name "Editor")
add_executable(${target_name} WIN32)

set(sources
	${CMAKE_CURRENT_SOURCE_DIR}/App.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/App.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/Clipboard.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/Clipboard.h
	${CMAKE_CURRENT_SOURCE_DIR}/Delegates.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Delegates.h
	${CMAKE_CURRENT_SOURCE_DIR}/Desktop.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Desktop.h
	${CMAKE_CURRENT_SOURCE_DIR}/Explorer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Explorer.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/FbxHandler.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/FbxHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/Hierarchy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Hierarchy.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/Inspector.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/Inspector.h
	${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Pch.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/UIEditor.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/UIEditor.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/Viewport.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/Viewport.h
)
source_group("Src" FILES ${sources})
target_sources(${target_name} PRIVATE ${sources})

set(common_sources
	${RWT_COMMON_SOURCE_DIR}/Delegate.h
	${RWT_COMMON_SOURCE_DIR}/Singleton.h
	${RWT_COMMON_SOURCE_DIR}/Timer.cpp
	${RWT_COMMON_SOURCE_DIR}/Timer.h
	${RWT_COMMON_SOURCE_DIR}/Type.cpp
	${RWT_COMMON_SOURCE_DIR}/Type.h
	${RWT_COMMON_SOURCE_DIR}/Util.cpp
	${RWT_COMMON_SOURCE_DIR}/Util.h
)
source_group("Src/Common" FILES ${common_sources})
target_sources(${target_name} PRIVATE ${common_sources})

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
	${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
	${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
)
target_include_directories(${target_name} PRIVATE
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
	${imgui_SOURCE_DIR}/misc/cpp
)

FetchContent_Populate(
	ufbx
	GIT_REPOSITORY https://github.com/ufbx/ufbx.git
	GIT_TAG 6ecd6177af59c82ec363356ac36c3a4245b85321 # v0.21.2
	SOURCE_DIR ${RWT_EXTERNAL_DIR}/ufbx
)
set_source_files_properties(
	${ufbx_SOURCE_DIR}/ufbx.c
	PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)
target_sources(${target_name} PRIVATE
	${ufbx_SOURCE_DIR}/ufbx.c
	${ufbx_SOURCE_DIR}/ufbx.h
)
target_include_directories(${target_name} PRIVATE
	${ufbx_SOURCE_DIR}
)

target_precompile_headers(${target_name} PRIVATE "Pch.h")
source_group("Gen" FILES
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.cxx"
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.hxx"
)

target_link_libraries(${target_name} PRIVATE "Graphics" "Resource")
source_group("Library/Graphics" "${RWT_SOURCE_DIR}/Library/Graphics/Include/Graphics/.*\\.h$")
source_group("Library/Resource" "${RWT_SOURCE_DIR}/Library/Resource/Include/Resource/.*\\.h$")

add_custom_command(
	TARGET ${target_name} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink
		"${CMAKE_SOURCE_DIR}/Data/Tool/${target_name}"
		"$<TARGET_FILE_DIR:${target_name}>/${target_name}"
)
