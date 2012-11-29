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


PROJECT(p4p_common_server)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/CommonInclude.cmake)
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/Doxygen.cmake)

FIND_PACKAGE(Boost ${BOOST_MIN_VERSION}
	COMPONENTS
		graph
		thread
		date_time
		filesystem
		iostreams
		serialization
	)
CheckLibFound(Boost)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
SET(LIBS ${LIBS} ${Boost_LIBRARIES})

FIND_PACKAGE(ZLIB)
CheckLibFound(ZLIB)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
LINK_DIRECTORIES(${ZLIB_LIBRARY_DIR})
SET(LIBS ${LIBS} ${ZLIB_LIBRARY})

FIND_PACKAGE(Microhttpd)
CheckLibFound(Microhttpd)
INCLUDE_DIRECTORIES(${Microhttpd_INCLUDE_DIR})
LINK_DIRECTORIES(${Microhttpd_LIBRARY_DIR})
SET(LIBS ${LIBS} ${Microhttpd_LIBRARY})

FIND_PACKAGE(Log4cpp)
CheckLibFound(Log4cpp)
INCLUDE_DIRECTORIES(${Log4cpp_INCLUDE_DIR})
LINK_DIRECTORIES(${Log4cpp_LIBRARY_DIR})
SET(LIBS ${LIBS} ${Log4cpp_LIBRARY})

FIND_PACKAGE(p4p_common_cpp)
CheckLibFound(p4p_common_cpp)
INCLUDE_DIRECTORIES(${p4p_common_cpp_INCLUDE_DIR})
LINK_DIRECTORIES(${p4p_common_cpp_LIBRARY_DIR})
SET(LIBS ${LIBS} ${p4p_common_cpp_LIBRARY})

INCLUDE_DIRECTORIES(src/include)

SET(SRCS
	src/lib/boost_random_device.cpp
	src/lib/dist_obj.cpp
	src/lib/local_obj.cpp
	src/lib/locking.cpp
	src/lib/net_state.cpp
	src/lib/pid_aggregation.cpp
	src/lib/pid_map.cpp
	src/lib/pid_routing.cpp
	src/lib/pid_matrix.cpp
	src/lib/job_queue.cpp
	src/lib/temp_file.cpp
	src/lib/logging.cpp
	src/lib/protocol_server_base.cpp
	src/lib/protocol_server_rest.cpp
	src/lib/rest_request_state.cpp
	src/lib/marked_stream.cpp
	)

ADD_LIBRARY(p4p_common_server SHARED ${SRCS})
TARGET_LINK_LIBRARIES(p4p_common_server ${LIBS})
SET_TARGET_PROPERTIES(p4p_common_server
	PROPERTIES
	VERSION ${P4P_VERSION}
	SOVERSION 1
	SRCTREE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include
	)

INSTALL(TARGETS p4p_common_server
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	COMPONENT Library
	)

INSTALL(DIRECTORY src/include/
	DESTINATION include
	COMPONENT LibraryDevel
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
	)

InstallExtraLibs(${Boost_LIBRARIES} ${Log4cpp_LIBRARY} ${Microhttpd_LIBRARY} ${ZLIB_LIBRARY})

Doxygen(Doxyfile)

IF (P4P_TESTING)
	FIND_PACKAGE(Boost ${BOOST_MIN_VERSION}
		COMPONENTS
			unit_test_framework
		REQUIRED)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

	SET(LIBS ${LIBS} ${Boost_LIBRARIES})

	ADD_EXECUTABLE(p4p_common_server_unittest
		test/main.cpp
		test/data/pid_matrix.cpp
	)
	TARGET_LINK_LIBRARIES(p4p_common_server_unittest ${LIBS} p4p_common_server)
	AddUnitTest(p4p_common_server_unittest)
ENDIF (P4P_TESTING)

