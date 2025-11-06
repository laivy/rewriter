cmake_minimum_required(VERSION 3.30)

set(target_name "Resource")
add_library(${target_name} SHARED)

include(GenerateExportHeader)
string(TOUPPER ${target_name} target_name_upper)
generate_export_header(${target_name}
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Export.h
	EXPORT_MACRO_NAME ${target_name_upper}_API
)

set(public_sources
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Define.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Export.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Model.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Property.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Resource.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Sprite.h
)
source_group("Include/${target_name}" FILES ${public_sources})
target_sources(${target_name} PUBLIC ${public_sources})

set(sources
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
source_group("Src" FILES ${sources})
target_sources(${target_name} PRIVATE ${sources})

set(common_sources
	${RWT_COMMON_SOURCE_DIR}/Delegate.h
	${RWT_COMMON_SOURCE_DIR}/Singleton.h
	${RWT_COMMON_SOURCE_DIR}/Type.cpp
	${RWT_COMMON_SOURCE_DIR}/Type.h
)
source_group("Src/Common" FILES ${common_sources})
target_sources(${target_name} PRIVATE ${common_sources})

target_precompile_headers(${target_name} PRIVATE "Src/Pch.h")
source_group("Gen" FILES
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.cxx"
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.hxx"
)

target_include_directories(${target_name}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}"
)
