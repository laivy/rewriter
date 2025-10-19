cmake_minimum_required(VERSION 3.12)

project(Rewriter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 소스 파일 UTF-8 인코딩 설정
if (MSVC)
	add_compile_options("/utf-8")
endif()

# 모든 프로젝트에 정의할 매크로
add_compile_definitions(_UNICODE UNICODE NOMINMAX)

# 모든 프로젝트에 포함할 디렉토리
include_directories(
	${CMAKE_SOURCE_DIR}/Src
	${CMAKE_SOURCE_DIR}/External
)

add_subdirectory(Src/Library)
add_subdirectory(Src/Tool)
