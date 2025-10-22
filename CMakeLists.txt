cmake_minimum_required(VERSION 3.30)

project(Rewriter)

# C++23 표준 사용
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 출력 파일 경로
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${TARGET_CATEGORY}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${TARGET_CATEGORY}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${TARGET_CATEGORY}/$<CONFIG>)

# 소스 파일 UTF-8 인코딩 설정
if (MSVC)
	add_compile_options("/utf-8")
endif()

# 모든 프로젝트에 정의할 매크로
add_compile_definitions(_UNICODE UNICODE NOMINMAX)

# 프로젝트 소스 경로 단축
set(COMMON_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Src/Common)
set(EXTERNAL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/External)

# 모든 프로젝트에 포함할 디렉토리
include_directories(
	${CMAKE_SOURCE_DIR}/Src
	${EXTERNAL_SOURCE_DIR}
)

# 카테고리에 따른 타겟 설정
if (TARGET_CATEGORY STREQUAL "Client")
	add_compile_definitions(_CLIENT)
elseif (TARGET_CATEGORY STREQUAL "Server")
	add_compile_definitions(_SERVER)
elseif (TARGET_CATEGORY STREQUAL "Tool")
	add_compile_definitions(_TOOL)
	add_compile_definitions(_DIRECT2D _IMGUI)
	add_subdirectory(Src/Library)
	add_subdirectory(Src/Tool)
else()
	message(FATAL_ERROR "Invalid target category: ${TARGET_CATEGORY}")
endif()
