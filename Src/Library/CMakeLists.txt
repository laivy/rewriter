cmake_minimum_required(VERSION 3.30)

add_subdirectory(Database)
add_subdirectory(Graphics)
add_subdirectory(Resource)
set_target_properties(
	Database
	Graphics
	Resource
	PROPERTIES
	FOLDER "Library"
)
