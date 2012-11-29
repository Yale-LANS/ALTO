# Copyright (c) 2008,2009, Yale Laboratory of Networked Systems
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice,
#       this list of conditions and the following disclaimer in the documentation
#       and/or other materials provided with the distribution.
#     * Neither the name of Yale University nor the names of its contributors may
#       be used to endorse or promote products derived from this software without
#       specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


PROJECT(p4p_common_com)

IF(NOT MSVC)
	MESSAGE(STATUS "Skipping ${PROJECT_NAME} on compiler other than Microsoft Visual Studio")
	RETURN()
ENDIF(NOT MSVC)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/CommonInclude.cmake)

SET(IDL_CC		midl)
SET(IDL_FILE		${CMAKE_CURRENT_SOURCE_DIR}/src/include/p4pcom/IP4P.idl)
SET(OUTPUT_DIR		${CMAKE_CURRENT_BINARY_DIR}/generated)
SET(OUTPUT_RAW		${OUTPUT_DIR}/raw)
SET(OUTPUT_LIB		${OUTPUT_DIR}/src/lib)
SET(OUTPUT_INC		${OUTPUT_DIR}/src/include)
SET(OUTPUT_INC_INTERNAL	${OUTPUT_DIR}/src/include/p4pcom)

FIND_PACKAGE(p4p_common_cpp)
CheckLibFound(p4p_common_cpp)
INCLUDE_DIRECTORIES(${p4p_common_cpp_INCLUDE_DIR})
LINK_DIRECTORIES(${p4p_common_cpp_LIBRARY_DIR})
SET(LIBS ${LIBS} ${p4p_common_cpp_LIBRARY})

INCLUDE_DIRECTORIES(src/include)

SET(SRCS
	src/lib/ISPManagerObj.cpp
	src/lib/ISPObj.cpp
	src/lib/p4pcomdll.cpp
	src/lib/P4PFactory.cpp
	src/lib/P4PPeerListObj.cpp
	src/lib/P4PPeerObj.cpp
	src/lib/PGMObj.cpp
	src/lib/PGMOptionsObj.cpp
	src/lib/PGMSelectionManagerObj.cpp
	src/lib/P4PUpdateManagerObj.cpp
	src/lib/stdafx.cpp
	)

SET(GEN_SRCS
	${OUTPUT_INC_INTERNAL}/IP4P.h
	${OUTPUT_LIB}/IP4P_i.c
	)
SET_SOURCE_FILES_PROPERTIES(${GEN_SRCS}
	PROPERTIES GENERATED TRUE
	)

ADD_CUSTOM_COMMAND(OUTPUT ${GEN_SRCS}
	COMMAND cmake -E make_directory ${OUTPUT_RAW}
	COMMAND cmake -E make_directory ${OUTPUT_LIB}
	COMMAND cmake -E make_directory ${OUTPUT_INC_INTERNAL}
	COMMAND ${IDL_CC} /out ${OUTPUT_RAW} ${IDL_FILE}
	COMMAND cmake -E copy ${OUTPUT_RAW}/IP4P.h	${OUTPUT_INC_INTERNAL}
	COMMAND cmake -E copy ${OUTPUT_RAW}/IP4P_i.c	${OUTPUT_LIB}
	)
INCLUDE_DIRECTORIES(${OUTPUT_INC})

ADD_LIBRARY(p4p_common_com SHARED ${SRCS} ${GEN_SRCS})
TARGET_LINK_LIBRARIES(p4p_common_com ${LIBS})
SET_TARGET_PROPERTIES(p4p_common_com
	PROPERTIES
	VERSION ${P4P_VERSION}
	SOVERSION 1
	SRCTREE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include
	)

INSTALL(TARGETS p4p_common_com
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	COMPONENT Library
	)

INSTALL(DIRECTORY src/include/
	DESTINATION include
	COMPONENT Library
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
	)
INSTALL(DIRECTORY src/include/
	DESTINATION include
	COMPONENT Library
	FILES_MATCHING PATTERN "*.idl"
	PATTERN ".svn" EXCLUDE
	)
INSTALL(DIRECTORY ${OUTPUT_INC}/
	DESTINATION include
	COMPONENT Library
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
	)


# Example Programs:
ADD_EXECUTABLE(p4p_common_com_example_tracker
	examples/tracker_example/p4pcomtest.cpp
	examples/tracker_example/stdafx.cpp
	)
TARGET_LINK_LIBRARIES(p4p_common_com_example_tracker	p4p_common_com)

