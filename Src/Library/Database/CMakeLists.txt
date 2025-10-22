cmake_minimum_required(VERSION 3.30)

set(TARGET Database)
string(TOUPPER ${TARGET} TARGET_NAME_UPPER)

add_library(${TARGET} SHARED)

# 내보내기 매크로
include(GenerateExportHeader)
generate_export_header(${TARGET}
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Export.h
	EXPORT_MACRO_NAME ${TARGET_NAME_UPPER}_API
)

# 소스 파일 설정
set(PUBLIC_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Database.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Query.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Type.h
)
set(PRIVATE_HEADERS
	${CMAKE_SOURCE_DIR}/Src/Common/Time.h
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Session.h
)
set(SOURCES
	${CMAKE_SOURCE_DIR}/Src/Common/Time.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Database.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/DllMain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Query.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Session.cpp
)
source_group(Header FILES ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
source_group(Source FILES ${SOURCES})
target_sources(${TARGET}
	PUBLIC
		${PUBLIC_HEADERS}
	PRIVATE
		${PRIVATE_HEADERS}
		${SOURCES}
)

# 미리 컴파일된 헤더
target_precompile_headers(${TARGET} PRIVATE Src/Pch.h)

# 추가 포함 디렉토리
target_include_directories(${TARGET}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Include
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}
)

# 종속성
find_package(ODBC)
if(NOT ODBC_FOUND)
	message(FATAL_ERROR "ODBC library not found")
endif()
target_link_libraries(${TARGET}
	PRIVATE
		ODBC::ODBC
		Resource
)
