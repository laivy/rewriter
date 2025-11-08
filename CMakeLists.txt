cmake_minimum_required(VERSION 3.30)

set(project_name "Rewriter")
project(${project_name})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${RWT_TARGET_CATEGORY}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${RWT_TARGET_CATEGORY}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${RWT_TARGET_CATEGORY}/$<CONFIG>)

set(RWT_CMAKE_DIR ${CMAKE_SOURCE_DIR}/CMake)
set(RWT_EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/External)
set(RWT_MODULE_DIR ${CMAKE_SOURCE_DIR}/Module)
set(RWT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Src)
set(RWT_COMMON_SOURCE_DIR ${RWT_SOURCE_DIR}/Common)

if(MSVC)
	add_compile_options("/utf-8")
else()
	message(WARNING "source charset, execution charset must be utf-8")
endif()

add_compile_definitions(
	_UNICODE
	NOMINMAX
	UNICODE
	WIN32_LEAN_AND_MEAN
)

include_directories(
	${RWT_SOURCE_DIR}
	${RWT_EXTERNAL_DIR}
)

if(${RWT_TARGET_CATEGORY} STREQUAL "Client")
	add_compile_definitions(_CLIENT)
elseif(${RWT_TARGET_CATEGORY} STREQUAL "Server")
	add_compile_definitions(_SERVER)
elseif(${RWT_TARGET_CATEGORY} STREQUAL "Tool")
	add_compile_definitions(_TOOL)
	add_compile_definitions(_DIRECT2D _IMGUI)
	add_subdirectory(Module EXCLUDE_FROM_ALL)
	add_subdirectory(Src/Tool)
else()
	message(FATAL_ERROR "invalid target category: ${RWT_TARGET_CATEGORY}")
endif()
