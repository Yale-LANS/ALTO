IF ( EXISTS "${CMAKE_SOURCE_DIR}/ROOT" )
	# If we are building from the root directory, just add a
	# library dependency on the project instead of searching
	# for the include files

	SET(p4p_aoe_engine_LIBRARY p4p_aoe_engine)

	GET_TARGET_PROPERTY(p4p_aoe_engine_INCLUDE_DIR p4p_aoe_engine SRCTREE_INCLUDE_DIR)
	IF(NOT p4p_aoe_engine_INCLUDE_DIR)
		IF (p4p_aoe_engine_FIND_REQUIRED)
			MESSAGE(FATAL_ERROR "Failed to find p4p_aoe_engine in source tree")
		ENDIF (p4p_aoe_engine_FIND_REQUIRED)
		RETURN()
	ENDIF(NOT p4p_aoe_engine_INCLUDE_DIR)

	SET(p4p_aoe_engine_FOUND TRUE)

	IF (NOT p4p_aoe_engine_FIND_QUIETLY)
		MESSAGE(STATUS "Found p4p_aoe_engine as subproject: ${p4p_aoe_engine_INCLUDE_DIR}")
	ENDIF (NOT p4p_aoe_engine_FIND_QUIETLY)

	RETURN()
ENDIF ( EXISTS "${CMAKE_SOURCE_DIR}/ROOT" )

FIND_PATH(p4p_aoe_engine_INCLUDE_DIR p4paoe/swarm.h)
IF (p4p_aoe_engine_USE_STATIC_LIBS)
	SET( _p4p_aoe_engine_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
	IF(WIN32)
		SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
	ELSE(WIN32)
		SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
	ENDIF(WIN32)
ENDIF (p4p_aoe_engine_USE_STATIC_LIBS)
FIND_LIBRARY(p4p_aoe_engine_LIBRARY NAMES p4p_aoe_engine)
IF (p4p_aoe_engine_USE_STATIC_LIBS)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ${_p4p_aoe_engine_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
ENDIF (p4p_aoe_engine_USE_STATIC_LIBS )

IF (p4p_aoe_engine_INCLUDE_DIR AND p4p_aoe_engine_LIBRARY)
	SET(p4p_aoe_engine_FOUND TRUE)
ENDIF (p4p_aoe_engine_INCLUDE_DIR AND p4p_aoe_engine_LIBRARY)


IF (p4p_aoe_engine_FOUND)
	IF (NOT p4p_aoe_engine_FIND_QUIETLY)
		MESSAGE(STATUS "Found p4p_aoe_engine: ${p4p_aoe_engine_LIBRARY}")
	ENDIF (NOT p4p_aoe_engine_FIND_QUIETLY)
ELSE (p4p_aoe_engine_FOUND)
	IF (p4p_aoe_engine_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find p4p_aoe_engine")
	ENDIF (p4p_aoe_engine_FIND_REQUIRED)
ENDIF (p4p_aoe_engine_FOUND)

