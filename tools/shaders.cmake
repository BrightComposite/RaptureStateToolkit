cmake_minimum_required(VERSION 3.0)

#if(NOT DEFINED RAPTURE_SHADERS_INC)
#	set(RAPTURE_SHADERS_INC 1 CACHE INTERNAL "shaders.cmake included" FORCE)
	
	function(escape_regular OUT_STR STR)
		string(REPLACE "\\" "\\\\" BUF "${STR}")
		string(REPLACE "/" "\\/" BUF "${BUF}")
		string(REPLACE "+" "\\+" BUF "${BUF}")
		string(REPLACE "." "\\." BUF "${BUF}")
		string(REPLACE ":" "\\:" BUF "${BUF}")
		
		set(${OUT_STR} ${BUF} PARENT_SCOPE)
	endfunction()
	
	if(WIN32)
		function(add_shaders OUT_SHADERS_LIST OUT_INCLUDES_LIST ROOT_DIR RES_DIR RES_GROUP INC_DIR INC_GROUP SHADERS_ROOT)
			set_project_sources(OWN_SHADERS_LIST ${ROOT_DIR}
				SRC_GROUP ${RES_DIR} ${RES_GROUP}
					START_SECTION ${SHADERS_ROOT}
						${ARGN}
					END_SECTION
			)
			
			set(INCLUDE_ENTRIES)
			
			foreach(src_entry ${ARGN})
				string(REPLACE ".fx" ".inc" inc_entry ${src_entry})
				list(APPEND INCLUDE_ENTRIES ${inc_entry})
			endforeach()
			
			foreach(shader ${OWN_SHADERS_LIST})
				get_filename_component(FileName ${shader} NAME_WE)
				get_filename_component(FileDir ${shader} DIRECTORY)
				
				escape_regular(res_root ${ROOT_DIR}/${RES_DIR})
				string(REGEX REPLACE ${res_root} "" ShaderPath ${FileDir})
				
				set(InputFile ${RES_DIR}${ShaderPath}/${FileName}.fx) # Input shader
				set(OutputFile ${INC_DIR}${ShaderPath}/${FileName}.inc) # Output .inc file
				
				# Get type of shader to compile (vs, ps)
				
				string(LENGTH ${FileName} FileLen)
				math(EXPR FileLen "${FileLen} - 2")
				string(SUBSTRING ${FileName} ${FileLen} -1 ShaderType)
				string(TOLOWER ${ShaderType} ShaderType) 
				
				# Add the command that will process each single file
				
				escape_regular(shaders_root ${SHADERS_ROOT})
				string(REGEX REPLACE ${shaders_root} "" ShaderPath ${ShaderPath})
				
				if("${ShaderPath}" MATCHES "^[\\/]+")
					string(REGEX REPLACE "^[\\/]+" "" ShaderPath ${ShaderPath})
				endif()
				
				string(REGEX REPLACE "[\\/]" _ ShaderId ${ShaderPath}/${FileName})
				string(REGEX REPLACE "[^a-zA-Z0-9_]" "" ShaderId ${ShaderId})
				
				add_custom_command(
					OUTPUT ${OutputFile}
					COMMAND call "${RAPTURE_TOOLS}\\compile-shader" "${InputFile}" "${OutputFile}" "${ShaderType}" "${ShaderId}"
					MAIN_DEPENDENCY ${InputFile}
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					VERBATIM
					)
			endforeach()
		
			set_project_sources(OWN_INCLUDES_LIST ${ROOT_DIR}
				SRC_GROUP ${INC_DIR} ${INC_GROUP}
					START_SECTION ${SHADERS_ROOT}
						${INCLUDE_ENTRIES}
					END_SECTION
			)
			
			set(${OUT_SHADERS_LIST} ${OWN_SHADERS_LIST} PARENT_SCOPE)
			set(${OUT_INCLUDES_LIST} ${OWN_INCLUDES_LIST} PARENT_SCOPE)
		endfunction()
	else()
		function(add_shaders OUT_SHADERS_LIST OUT_INCLUDES_LIST ROOT_DIR RES_DIR RES_GROUP INC_DIR INC_GROUP SHADERS_ROOT)
			set_project_sources(OWN_SHADERS_LIST ${ROOT_DIR}
				SRC_GROUP ${RES_DIR} ${RES_GROUP}
					${ARGN}
			)
			
			set(${OUT_SHADERS_LIST} ${OWN_SHADERS_LIST} PARENT_SCOPE)
		endfunction()
	endif()
#endif()