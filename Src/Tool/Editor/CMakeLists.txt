cmake_minimum_required(VERSION 3.30)

set(TARGET Editor)
add_executable(${TARGET} WIN32)

# 소스 파일 설정
set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/App.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/Clipboard.h
	${CMAKE_CURRENT_SOURCE_DIR}/Delegates.h
	${CMAKE_CURRENT_SOURCE_DIR}/Desktop.h
	${CMAKE_CURRENT_SOURCE_DIR}/Explorer.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/FbxHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/Hierarchy.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/Inspector.h
	${CMAKE_CURRENT_SOURCE_DIR}/Pch.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/UIEditor.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/Viewport.h
	${COMMON_SOURCE_DIR}/Delegate.h
	${COMMON_SOURCE_DIR}/Singleton.h
	${COMMON_SOURCE_DIR}/Timer.h
	${COMMON_SOURCE_DIR}/Type.h
	${COMMON_SOURCE_DIR}/Util.h
)
set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/App.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/Clipboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Delegates.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Desktop.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Explorer.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/FbxHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Hierarchy.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/Inspector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/UIEditor.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/Viewport.cpp
	${COMMON_SOURCE_DIR}/Timer.cpp
	${COMMON_SOURCE_DIR}/Type.cpp
	${COMMON_SOURCE_DIR}/Util.cpp
)
source_group(Header FILES ${HEADERS})
source_group(Source FILES ${SOURCES})
target_sources(${TARGET}
	PRIVATE
		${HEADERS}
		${SOURCES}
)

# 미리 컴파일된 헤더
target_precompile_headers(${TARGET} PRIVATE Pch.h)

# 종속성
target_link_libraries(${TARGET} PRIVATE Graphics Resource)

# 데이터 링크
add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink
		"${CMAKE_SOURCE_DIR}/Data/Tool/${TARGET}"
		"$<TARGET_FILE_DIR:${TARGET}>/${TARGET}"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/External/FBXSDK/lib/x64/debug/libfbxsdk.dll"
		"$<TARGET_FILE_DIR:${TARGET}>"
)
