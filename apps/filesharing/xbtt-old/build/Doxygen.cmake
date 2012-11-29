MACRO(Doxygen doxyfile)

	FIND_PACKAGE(Doxygen)
	IF(DOXYGEN_FOUND)

		# click+jump in Emacs and Visual Studio (for Doxyfile) 
		# by Jan Woetzel 2004-2006
		# www.mip.informatik.uni-kiel.de/~jw
		IF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv)")
			SET(DOXY_WARN_FORMAT "\"$file($line) : $text \"")
		ELSE(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv)")
			SET(DOXY_WARN_FORMAT "\"$file:$line: $text \"")
		ENDIF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv)")

		# may need latex to generate formulas
		# by Jan Woetzel 2004-2006
		# www.mip.informatik.uni-kiel.de/~jw
		FIND_PACKAGE(LATEX)
		IF(NOT LATEX_COMPILER)
			MESSAGE(STATUS "latex command LATEX_COMPILER not found but usually required. You will probably get warnings and user inetraction on doxy run.")
		ENDIF(NOT LATEX_COMPILER)
		IF(NOT MAKEINDEX_COMPILER)
			MESSAGE(STATUS "makeindex command MAKEINDEX_COMPILER not found but usually required.")
		ENDIF(NOT MAKEINDEX_COMPILER)
		IF(NOT DVIPS_CONVERTER)
			MESSAGE(STATUS "dvips command DVIPS_CONVERTER not found but usually required.")
		ENDIF(NOT DVIPS_CONVERTER)

		# Gather include directories from the current directory
		GET_DIRECTORY_PROPERTY(CURRENT_INCLUDE_DIRS_LIST INCLUDE_DIRECTORIES)
		SET(CURRENT_INCLUDE_DIRS )
		FOREACH (INC_DIR ${CURRENT_INCLUDE_DIRS_LIST})
			SET(CURRENT_INCLUDE_DIRS "${CURRENT_INCLUDE_DIRS} ${INC_DIR}")
		ENDFOREACH (INC_DIR)

		IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${doxyfile}")

			CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${doxyfile}
					${CMAKE_CURRENT_BINARY_DIR}/${doxyfile}
					@ONLY)
			SET(DOXY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/${doxyfile}")
			MESSAGE(STATUS "Use ${CMAKE_CURRENT_BINARY_DIR}/${doxyfile} as Doxygen configuration")

		ELSE(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${doxyfile}")
			MESSAGE(SEND_ERROR "Please create Doxyfile ${doxyfile}")
		ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${doxyfile}")  

		ADD_CUSTOM_TARGET(${PROJECT_NAME}_doxygen ALL ${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG})

	ENDIF(DOXYGEN_FOUND)

ENDMACRO(Doxygen)

