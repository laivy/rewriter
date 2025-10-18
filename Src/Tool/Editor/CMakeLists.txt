cmake_minimum_required(VERSION 3.10)

set(TARGET Editor)
set(COMMON ${CMAKE_SOURCE_DIR}/Src/Common)
#set(EXTERNAL ${CMAKE_SOURCE_DIR}/External)

add_executable(${TARGET}
	${COMMON}/Timer.cpp
	${COMMON}/Timer.h
	${COMMON}/Type.cpp
	${COMMON}/Type.h
	${COMMON}/Util.cpp
	${COMMON}/Util.h
	#${EXTERNAL}/Imgui/imgui_widgets.cpp
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
target_compile_definitions(${TARGET} PRIVATE _IMGUI _DIRECT2D)
target_link_libraries(${TARGET} PUBLIC Graphics)
target_link_libraries(${TARGET} PUBLIC Resource)
