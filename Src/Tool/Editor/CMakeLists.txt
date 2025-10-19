cmake_minimum_required(VERSION 3.12)

set(TARGET Editor)
set(COMMON ${CMAKE_SOURCE_DIR}/Src/Common)

add_executable(${TARGET} WIN32
	${COMMON}/Delegate.h
	${COMMON}/Singleton.h
	${COMMON}/Timer.cpp
	${COMMON}/Timer.h
	${COMMON}/Type.cpp
	${COMMON}/Type.h
	${COMMON}/Util.cpp
	${COMMON}/Util.h
	App.cpp
	App.h
	# Clipboard.cpp
	# Clipboard.h
	Delegates.cpp
	Delegates.h
	# Desktop.cpp
	# Desktop.h
	# Explorer.cpp
	# Explorer.h
	# FbxHandler.cpp
	# FbxHandler.h
	Hierarchy.cpp
	Hierarchy.h
	# Inspector.cpp
	# Inspector.h
	Main.cpp
	# UIEditor.cpp
	# UIEditor.h
	# Viewport.cpp
	# Viewport.h
)

target_precompile_headers(${TARGET} PRIVATE Pch.h)

# 출력 디렉토리
foreach(T Graphics Resource ${TARGET})
	set_target_properties(${T} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/$<CONFIG>_Tool
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/$<CONFIG>_Tool
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/$<CONFIG>_Tool
	)
endforeach()

# 데이터 링크
get_target_property(OUT_DIR ${TARGET} RUNTIME_OUTPUT_DIRECTORY)
add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND PY "${CMAKE_SOURCE_DIR}/Script/MakeDataSymlink.py"
		"${OUT_DIR}/${TARGET}"
		"${CMAKE_SOURCE_DIR}/Data/Tool/${TARGET}"
	COMMAND CMAKE -E copy_if_different
		"${CMAKE_SOURCE_DIR}/External/FBXSDK/lib/x64/debug/libfbxsdk.dll"
		"${OUT_DIR}"
)

# 종속성
target_compile_definitions(Graphics PUBLIC _IMGUI _DIRECT2D)
target_link_libraries(${TARGET} PUBLIC Graphics)
target_link_libraries(${TARGET} PUBLIC Resource)
