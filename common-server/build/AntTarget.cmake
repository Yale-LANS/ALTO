MACRO(AntTarget target libname)

	# Find location of Ant
	FIND_PROGRAM(ANT_PATH ant)
	IF (NOT ANT_PATH)
		MESSAGE(STATUS "Skipping ${PROJECT_NAME}: Ant not found")
		RETURN()
	ENDIF(NOT ANT_PATH)

	SET(ARG_TYPE )			# Current type of argument
	SET(ANT_EXTRA_ARGS )		# List of extra Ant arguments
	SET(HAS_DEPENDENCIES FALSE)	# True if at least one dependency specified
	SET(DEPENDENCIES )		# List of dependencies
	SET(CLASSPATH )			# Ant classpath (JARs for dependencies)
	FOREACH(ARG ${ARGN})
		IF (ARG STREQUAL "DEPENDS" OR ARG STREQUAL "ANTARGS" )
			SET(ARG_TYPE ${ARG})
		ELSE (ARG STREQUAL "DEPENDS" OR ARG STREQUAL "ANTARGS" )
			IF (ARG_TYPE STREQUAL "DEPENDS" )
				GET_TARGET_PROPERTY(JAR ${ARG} JARFILE)
				IF(JAR)
					SET(CLASSPATH "${CLASSPATH}:${JAR}")
				ENDIF(JAR)
				SET(HAS_DEPENDENCIES TRUE)
				SET(DEPENDENCIES ${DEPENDENCIES} ${ARG})
			ELSEIF (ARG_TYPE STREQUAL "ANTARGS" )
				SET(ANT_EXTRA_ARGS ${ANT_EXTRA_ARGS} ${ARG})
			ENDIF (ARG_TYPE STREQUAL "DEPENDS" )
		ENDIF (ARG STREQUAL "DEPENDS" OR ARG STREQUAL "ANTARGS" )
	ENDFOREACH(ARG)

	SET(ANT_ARGS
		-buildfile ${CMAKE_CURRENT_SOURCE_DIR}/build.xml
		-Ddir.src=${CMAKE_CURRENT_SOURCE_DIR}/src
		-Ddir.test=${CMAKE_CURRENT_SOURCE_DIR}/test
		-Ddir.out=${CMAKE_CURRENT_BINARY_DIR}/bin
		-Ddir.test.out=${CMAKE_CURRENT_BINARY_DIR}/bin-test
		-Ddir.dist=${CMAKE_CURRENT_BINARY_DIR}
		${ANT_EXTRA_ARGS}
		)
	IF(CLASSPATH)
		SET(ANT_ARGS ${ANT_ARGS} -lib ${CLASSPATH})
	ENDIF(CLASSPATH)

	# Build the JAR
	ADD_CUSTOM_COMMAND(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${libname}.jar
		COMMAND ${ANT_PATH} ${ANT_ARGS} jar
		)

	# Add convenient target for building the JAR and encoding dependencies
	ADD_CUSTOM_TARGET(${target} ALL
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${libname}.jar
		)
	IF(HAS_DEPENDENCIES)
		ADD_DEPENDENCIES(${target} ${DEPENDENCIES})
	ENDIF(HAS_DEPENDENCIES)

	ADD_TEST(${target}_test
		${ANT_PATH} ${ANT_ARGS} test
		)

	# Set property so dependent projects know where to find
	# the produced JAR file
	SET_TARGET_PROPERTIES(${target}
		PROPERTIES
			JARFILE ${CMAKE_CURRENT_BINARY_DIR}/${libname}.jar
		)

ENDMACRO(AntTarget)
