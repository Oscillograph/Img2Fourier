function(savannah_add_common_dependencies target link_dir)
	target_link_libraries(${target} PUBLIC 
    savannah
    glfw3
    glad
    dear-imgui
    yaml-cpp
    fftw
    )
#    target_link_libraries(${target} PUBLIC -lpthread "-Wl,-rpath,${link_dir}")
    message("Added dependencies for target ${target}")
endfunction()

function(savannah_add_target target type include_dir source_dir install_dir link_dir compiler_flags)
	file(GLOB_RECURSE target_sources 
		${source_dir}/*.cpp)
	separate_arguments(append_target_sources UNIX_COMMAND "${source_dir}")
	list(APPEND target_sources ${append_target_sources})
		
	if (${type} MATCHES "(EXE|STATIC|SHARED)")
		if (${type} STREQUAL "EXE")
			add_executable(${target} ${target_sources})
		elseif (${type} STREQUAL "STATIC")
			add_library(${target} STATIC ${target_sources})
		elseif (${type} STREQUAL "SHARED")
			add_library(${target} SHARED ${target_sources})
		endif()
	else()
		message(FATAL_ERROR "Target <type> should be equal to EXE, STATIC or SHARED.")
	endif()
	
	target_include_directories(${target} PRIVATE ${PROJECT_BASE_SOURCE_DIR}/include)
	target_include_directories(${target} PRIVATE ${include_dir})
#	target_compile_options(${target} PRIVATE ${compiler_flags})
	separate_arguments(cxx_compiler_flags UNIX_COMMAND "${compiler_flags}")
	target_compile_options(${target} PRIVATE ${cxx_compiler_flags})
#	target_compile_options(${target} PRIVATE -Wall -Werror -Wfatal-errors -DSAVANNAH_LOGGER_CLASS)
	savannah_add_common_dependencies(${target} $link_dir)
	
	install(TARGETS ${target} 
		RUNTIME DESTINATION ${install_dir}
		LIBRARY DESTINATION ${install_dir}
		ARCHIVE DESTINATION ${install_dir}
	)
endfunction()

function(savannah_include_dirs VARIABLE)
endfunction()

function(savannah_exclude_dir VARIABLE)
	set(files_collection ${SAVANNAH_SOURCES})
	foreach($dir ${ARGN})
		file(GLOB_RECURSE files_collection "${dir}/*.*")		
	endforeach()
	list (FILTER files_collection EXCLUDE REGEX "${dir}/")
	set(${SAVANNAH_SOURCES} ${files_collection} PARENT_SCOPE)
endfunction()