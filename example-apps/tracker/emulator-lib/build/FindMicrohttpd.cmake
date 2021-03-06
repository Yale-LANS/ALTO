FIND_PATH(Microhttpd_INCLUDE_DIR microhttpd.h)
IF (Microhttpd_USE_STATIC_LIBS)
	SET( _microhttpd_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
	IF(WIN32)
		SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
	ELSE(WIN32)
		SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
	ENDIF(WIN32)
ENDIF (Microhttpd_USE_STATIC_LIBS)
FIND_LIBRARY(Microhttpd_LIBRARY NAMES microhttpd)
FIND_LIBRARY(Microhttpd_gcrypt_LIBRARY NAMES gcrypt)
FIND_LIBRARY(Microhttpd_gpgerror_LIBRARY NAMES gpg-error)
IF (Microhttpd_USE_STATIC_LIBS)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ${_microhttpd_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
ENDIF (Microhttpd_USE_STATIC_LIBS )

IF (Microhttpd_INCLUDE_DIR AND Microhttpd_LIBRARY)
	SET(Microhttpd_FOUND TRUE)

	IF (Microhttpd_gcrypt_LIBRARY)
		SET(Microhttpd_LIBRARY ${Microhttpd_LIBRARY} ${Microhttpd_gcrypt_LIBRARY})
	ENDIF (Microhttpd_gcrypt_LIBRARY)
	IF (Microhttpd_gpgerror_LIBRARY)
		SET(Microhttpd_LIBRARY ${Microhttpd_LIBRARY} ${Microhttpd_gpgerror_LIBRARY})
	ENDIF (Microhttpd_gpgerror_LIBRARY)

ENDIF (Microhttpd_INCLUDE_DIR AND Microhttpd_LIBRARY)


IF (Microhttpd_FOUND)
	IF (NOT Microhttpd_FIND_QUIETLY)
		MESSAGE(STATUS "Found libmicrohttpd: ${Microhttpd_LIBRARY}")
	ENDIF (NOT Microhttpd_FIND_QUIETLY)
ELSE (Microhttpd_FOUND)
	IF (Microhttpd_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find libmicrohttpd")
	ENDIF (Microhttpd_FIND_REQUIRED)
ENDIF (Microhttpd_FOUND)

