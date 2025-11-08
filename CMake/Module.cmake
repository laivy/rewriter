
# <target_name>: 타겟 이름
# <module_name>...: 모듈 이름들
function(target_link_modules target_name)
	foreach(module_name IN LISTS ARGN)
		target_link_libraries(${target_name} PRIVATE ${module_name})
		source_group("Module/${module_name}" "${RWT_MODULE_DIR}/${module_name}/Include/${module_name}/.*\\.h$")
	endforeach()
endfunction()
