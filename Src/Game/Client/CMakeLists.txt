cmake_minimum_required(VERSION 3.30)

set(target_name "Client")
add_executable(${target_name} WIN32)

set(sources
	${CMAKE_CURRENT_SOURCE_DIR}/App.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/App.h
	${CMAKE_CURRENT_SOURCE_DIR}/Delegates.h
	${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Pch.h
)
source_group("Src" FILES ${sources})
target_sources(${target_name} PRIVATE ${sources})

set(common_sources
	${RWT_COMMON_SOURCE_DIR}/Delegate.h
	${RWT_COMMON_SOURCE_DIR}/Singleton.h
	${RWT_COMMON_SOURCE_DIR}/Timer.cpp
	${RWT_COMMON_SOURCE_DIR}/Timer.h
	${RWT_COMMON_SOURCE_DIR}/Type.cpp
	${RWT_COMMON_SOURCE_DIR}/Type.h
)
source_group("Src/Common" FILES ${common_sources})
target_sources(${target_name} PRIVATE ${common_sources})

target_precompile_headers(${target_name} PRIVATE "Pch.h")
source_group("Gen" FILES
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.cxx"
	"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.hxx"
)

include(${RWT_CMAKE_DIR}/Module.cmake)
target_link_modules(${target_name} "Graphics" "Resource")
