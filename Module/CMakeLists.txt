cmake_minimum_required(VERSION 3.30)

set(modules
	"Database"
	"Graphics"
	"Resource"
)
foreach(module IN LISTS modules)
	add_subdirectory(${module})
	set_target_properties(${module} PROPERTIES FOLDER "Module")
endforeach()
