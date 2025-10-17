cmake_minimum_required(VERSION 3.10)

project(Rewriter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)

# 소스 파일 UTF-8 인코딩 설정
if (MSVC)
    add_compile_options("/utf-8")
endif()

# 유니코드 매크로 정의
add_definitions(-DUNICODE -D_UNICODE)

# 모든 하위 프로젝트에 ${CMAKE_SOURCE_DIR}/Src 디렉토리를 포함
include_directories(${CMAKE_SOURCE_DIR}/Src)

add_subdirectory(Src/Library/Database)
add_subdirectory(Src/Library/Graphics)
add_subdirectory(Src/Library/Resource)
