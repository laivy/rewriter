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

target_sources(${TARGET}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Database.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Query.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Type.h
	PRIVATE
		${CMAKE_SOURCE_DIR}/Src/Common/Time.cpp
		${CMAKE_SOURCE_DIR}/Src/Common/Time.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Database.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/DllMain.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Query.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Session.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Session.h
)

target_precompile_headers(${TARGET} PRIVATE Src/Pch.h)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

# 종속성
find_package(ODBC)
target_link_libraries(${TARGET} PRIVATE ODBC::ODBC)
target_link_libraries(${TARGET} PUBLIC Resource)
