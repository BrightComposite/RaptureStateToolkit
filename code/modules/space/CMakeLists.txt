#--------------------------------------------------------
#	asd space
#--------------------------------------------------------

project(space VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(INLINE)
	dependencies(
		math	0.*
	)

	sources(modules)
		domain(space)

		group(include Headers)
		files(
			spatial.h
		)
	endsources()
endmodule()

#--------------------------------------------------------
