cmake_minimum_required(VERSION 3.12)

set(TARGET Resource)
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
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Define.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Model.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Property.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Resource.h
		${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET}/Sprite.h
	PRIVATE
		${CMAKE_SOURCE_DIR}/Src/Common/Delegate.h
		${CMAKE_SOURCE_DIR}/Src/Common/Singleton.h
		${CMAKE_SOURCE_DIR}/Src/Common/Type.cpp
		${CMAKE_SOURCE_DIR}/Src/Common/Type.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Delegates.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Delegates.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/DllMain.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Manager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Manager.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Model.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Property.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Resource.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Sprite.cpp
)

target_precompile_headers(${TARGET} PRIVATE Src/Pch.h)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
