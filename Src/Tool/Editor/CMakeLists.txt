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
	${CMAKE_CURRENT_SOURCE_DIR}/Inspector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Inspector.h
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

include(FetchContent)
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

include(${RWT_CMAKE_DIR}/Module.cmake)
target_link_modules(${target_name} "Graphics" "Resource")

add_custom_command(
	TARGET ${target_name} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink
		"${CMAKE_SOURCE_DIR}/Data/Tool/${target_name}"
		"$<TARGET_FILE_DIR:${target_name}>/${target_name}"
)
