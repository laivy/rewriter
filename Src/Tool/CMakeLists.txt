cmake_minimum_required(VERSION 3.30)

set(tools "Editor")
foreach(tool IN LISTS tools)
	add_subdirectory(${tool})
	set_target_properties(${tool} PROPERTIES FOLDER "Tool")
endforeach()
