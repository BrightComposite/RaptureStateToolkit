#--------------------------------------------------------
#	sdl test facility
#--------------------------------------------------------

project(sdl_test VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		application	0.*
		sdlpp 		0.*
	)

	sources(tests)
		group(include Headers)
		files(
			#
		)

		group(src Sources)
		files(
			main.cpp
		)
	endsources()
endmodule()

if(WIN32)
	vendor(vld)
endif()

#--------------------------------------------------------
