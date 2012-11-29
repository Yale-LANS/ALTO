###############
# P4P Version #
###############

IF (NOT P4P_VERSION)
	# Get version number from VERSION file
	FILE(READ "${CMAKE_SOURCE_DIR}/VERSION" P4P_VERSION)
	STRING(STRIP ${P4P_VERSION} P4P_VERSION)
ENDIF (NOT P4P_VERSION)

# Fail if still not set
IF (NOT P4P_VERSION)
	MESSAGE(FATAL_ERROR "Failed to determine version number!")
ENDIF (NOT P4P_VERSION)

########################
# Common CMake Options #
########################

# Import our custom modules
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/")

# Enable testing and Boost Unit testing
INCLUDE(BoostUnitTest)

# Enable Ant building
INCLUDE(AntTarget)

# Initialize threading
FIND_PACKAGE(Threads REQUIRED)
SET(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
SET(BOOST_MIN_VERSION "1.35")
SET(Boost_ADDITIONAL_VERSIONS "1.37" "1.37.0" "1.38" "1.38.0")
SET(Boost_USE_MULTITHREAD ON)
SET(Boost_USE_STATIC_LIBS OFF)

# Define a macro for also installing libraries
MACRO(InstallExtraLibs)
	IF (WIN32)
		FOREACH(lib ${ARGV})
			STRING(REGEX MATCH "\\.lib$" libmatch ${lib})
			IF (NOT "${libmatch}" STREQUAL "")
				STRING(REPLACE ".lib" ".dll" lib_dll ${lib})
				INSTALL(FILES ${lib_dll}
					DESTINATION bin
					)
				INSTALL(FILES ${lib}
					DESTINATION lib
					)
			ENDIF (NOT "${libmatch}" STREQUAL "")
		ENDFOREACH(lib)
	ENDIF (WIN32)
ENDMACRO(InstallExtraLibs)

# Define a macro for checking if a library was found
MACRO(CheckLibFound libname)
	IF (NOT ${libname}_FOUND)
		MESSAGE(STATUS "Skipping ${PROJECT_NAME}: ${libname} not found")
		# Returns from current file, NOT just this macro
		RETURN()
	ENDIF (NOT ${libname}_FOUND)
ENDMACRO(CheckLibFound)

##########################
# Compiler Configuration #
##########################

SET (STD_COMPILE_FLAGS "-DBOOST_ALL_DYN_LINK -DBOOST_NO_HASH")

IF (MSVC)
	SET (COMPILE_OPT_NONE "/Od")
	SET (COMPILE_OPT_FULL "/O2 /Oy")
	SET (COMPILE_DBG_INFO "/Zi")
	SET (COMPILE_FLAGS "/MD /GR /EHsc /wd4290 /wd4251 -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX")
	SET (LINKER_FLAGS "")
ELSEIF (${CMAKE_COMPILER_IS_GNUCXX})
	SET (COMPILE_OPT_NONE "-O0")
	SET (COMPILE_OPT_FULL "-O3 -fomit-frame-pointer")
	SET (COMPILE_DBG_INFO "-g3")
	SET (COMPILE_FLAGS "-Wall -fvisibility=hidden")
	SET (LINKER_FLAGS "")
ELSE ()
	MESSAGE(FATAL_ERROR "Unknown compiler; failed to set compiler options")

ENDIF (MSVC)

SET(CMAKE_C_FLAGS_DEBUG   "${STD_COMPILE_FLAGS} ${COMPILE_FLAGS} ${COMPILE_OPT_NONE} ${COMPILE_DBG_INFO} -DDEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "${STD_COMPILE_FLAGS} ${COMPILE_FLAGS} ${COMPILE_OPT_NONE} ${COMPILE_DBG_INFO} -DDEBUG")

SET(CMAKE_C_FLAGS_RELEASE   "${STD_COMPILE_FLAGS} ${COMPILE_FLAGS} ${COMPILE_OPT_FULL} -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "${STD_COMPILE_FLAGS} ${COMPILE_FLAGS} ${COMPILE_OPT_FULL} -DNDEBUG")

SET(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELEASE} ${COMPILE_DBG_INFO}")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILE_DBG_INFO}")

SET(CMAKE_C_FLAGS_MINSIZEREL   "${CMAKE_C_FLAGS_RELEASE}")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE}")

SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# On windows, link with common DLLs
IF (WIN32)
	SET(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ws2_32.lib")
ENDIF (WIN32)

## Statically-link C++ libraries and libgcc
#IF (${CMAKE_COMPILER_IS_GNUCXX})
#	ADD_DEFINITIONS(-static-libgcc)
#	SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic -lm -static-libgcc")
#ENDIF (${CMAKE_COMPILER_IS_GNUCXX})

