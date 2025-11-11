cmake_minimum_required(VERSION 3.30)

set(target_name "Database")
add_library(${target_name} SHARED)

include(GenerateExportHeader)
string(TOUPPER ${target_name} target_name_upper)
generate_export_header(${target_name}
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Export.h
	EXPORT_MACRO_NAME ${target_name_upper}_API
)

set(public_headers
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Database.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Export.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Query.h
	${CMAKE_CURRENT_SOURCE_DIR}/Include/${target_name}/Type.h
)
source_group("Include/${target_name}" FILES ${public_headers})
target_sources(${target_name} PUBLIC ${public_headers})

set(sources
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Database.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/DllMain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Query.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Session.h
)
source_group("Src" FILES ${sources})
target_sources(${target_name} PRIVATE ${sources})

set(common_sources
	${RWT_COMMON_SOURCE_DIR}/Time.h
	${RWT_COMMON_SOURCE_DIR}/Time.cpp
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

find_package(ODBC REQUIRED)
target_link_libraries(${target_name} PRIVATE "ODBC::ODBC" "Resource")
source_group("Library/Resource" "${RWT_SOURCE_DIR}/Library/Resource/Include/Resource/.*\\.h$")
