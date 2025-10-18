cmake_minimum_required(VERSION 3.10)

set(TARGET Editor)
set(COMMON ${CMAKE_SOURCE_DIR}/Src/Common)

add_executable(${TARGET} WIN32
	${COMMON}/Timer.cpp
	${COMMON}/Timer.h
	${COMMON}/Type.cpp
	${COMMON}/Type.h
	${COMMON}/Util.cpp
	${COMMON}/Util.h
	App.cpp
	App.h
	#Clipboard.cpp
	#Clipboard.h
	Delegates.cpp
	Delegates.h
	#Desktop.cpp
	#Desktop.h
	#Explorer.cpp
	#Explorer.h
	#FbxHandler.cpp
	#FbxHandler.h
	Hierarchy.cpp
	Hierarchy.h
	#Inspector.cpp
	#Inspector.h
	Main.cpp
	#UIEditor.cpp
	#UIEditor.h
	#Viewport.cpp
	#Viewport.h
)

target_precompile_headers(${TARGET} PRIVATE Pch.h)

# 종속성
target_compile_definitions(Graphics PUBLIC _IMGUI _DIRECT2D)
target_link_libraries(${TARGET} PUBLIC Graphics)
target_link_libraries(${TARGET} PUBLIC Resource)
