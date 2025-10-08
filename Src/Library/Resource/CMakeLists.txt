cmake_minimum_required(VERSION 3.12)

add_library(Resource SHARED)

target_sources(Resource
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
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/Define.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/Model.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/Property.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/Resource.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/Sprite.h
)

# 내보내기 매크로 생성
include(GenerateExportHeader)
generate_export_header(Resource
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/Export.h
    EXPORT_MACRO_NAME RESOURCE_API
)

target_precompile_headers(Resource PRIVATE Src/Pch.h)

target_include_directories(Resource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource)
