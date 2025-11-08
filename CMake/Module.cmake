
macro(target_add_modules target_name)
	foreach(module_name IN LISTS ARGV)
		target_link_libraries(${target_name} PRIVATE ${module_name})
		source_group("Module/${module_name}" "${RWT_MODULE_DIR}/${module_name}/Include/${module_name}/.*\\.h$")
	endforeach()
endmacro()
