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


PROJECT(p4p_common_cpp)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/CommonInclude.cmake)
#INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/Doxygen.cmake)

FIND_PACKAGE(CURL)
CheckLibFound(CURL)
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
LINK_DIRECTORIES(${CURL_LIBRARY_DIR})
SET(LIBS ${LIBS} ${CURL_LIBRARY})

INCLUDE_DIRECTORIES(src/include)

SET(SRCS
	src/lib/ip_addr.cpp
	src/lib/inet_service.cpp
	src/lib/pid.cpp
	src/lib/patricia.cpp
	src/lib/patricia_trie.cpp
	src/lib/pid_matrix_generic.cpp
	src/lib/isp.cpp
	src/lib/isp_pidmap.cpp
	src/lib/isp_pdistancemap.cpp
	src/lib/isp_manager.cpp
	src/lib/util.cpp
	src/lib/init.cpp
	src/lib/mutex.cpp
	src/lib/temp_file_stream.cpp
	src/lib/heap_with_delete.cpp
	src/lib/random_access_set.cpp
	src/lib/protocol/protobase.cpp
	src/lib/protocol/parsing.cpp
	src/lib/protocol/dataconv.cpp
	src/lib/protocol/metainfo.cpp
	src/lib/protocol-portal/parsing.cpp
	src/lib/protocol-portal/admin.cpp
	src/lib/protocol-portal/location.cpp
	src/lib/protocol-portal/pdistance.cpp
	src/lib/protocol-portal/metainfo.cpp
	src/lib/protocol-portal/pdistance_matrix.cpp
	src/lib/protocol-portal/pid_prefixes.cpp
	src/lib/peering_guidance_matrix.cpp
	src/lib/peering_guidance_matrix_manager.cpp
	src/lib/peering_guidance_matrix_opts.cpp
	src/lib/pgm_selection_manager.cpp
	src/lib/client_peer_selection_location.cpp
	src/lib/p4p_selection_manager.cpp
	src/lib/perisp_peer_distribution.cpp
	src/lib/peer_distribution_manager.cpp
	src/lib/pid_peer_selection_helper.cpp
	src/lib/swarm_state.cpp
	src/lib/peer.cpp
	src/lib/update_manager.cpp
	src/lib/aoe.cpp
	src/lib/pid_class_pcts.cpp
	src/lib/weights.cpp
	src/lib/parsing.cpp
	src/lib/logging.cpp
	)

ADD_LIBRARY(p4p_common_cpp SHARED ${SRCS})
TARGET_LINK_LIBRARIES(p4p_common_cpp ${LIBS})
SET_TARGET_PROPERTIES(p4p_common_cpp
	PROPERTIES
	VERSION ${P4P_VERSION}
	SOVERSION 1
	SRCTREE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include
	)

INSTALL(TARGETS p4p_common_cpp
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

InstallExtraLibs(${CURL_LIBRARY})

#Doxygen(Doxyfile)
#IF (DOXYGEN_FOUND)
#	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/
#		DESTINATION share/doc/${PROJECT_NAME}
#		COMPONENT LibraryDevel
#		PATTERN ".svn" EXCLUDE
#		)
#ENDIF (DOXYGEN_FOUND)

# Example Programs:
ADD_EXECUTABLE(p4p_common_cpp_example_tracker		examples/p4ptracker_walkthrough.cpp)
TARGET_LINK_LIBRARIES(p4p_common_cpp_example_tracker	p4p_common_cpp)

ADD_EXECUTABLE(p4p_common_cpp_example_isp		examples/isp_object_hello_world.cpp)
TARGET_LINK_LIBRARIES(p4p_common_cpp_example_isp	p4p_common_cpp)

ADD_EXECUTABLE(p4p_common_cpp_example_isp_ds		examples/isp_object_datasources.cpp)
TARGET_LINK_LIBRARIES(p4p_common_cpp_example_isp_ds	p4p_common_cpp)

ADD_EXECUTABLE(p4p_common_cpp_example_pgm		examples/pgm_object_hello_world.cpp)
TARGET_LINK_LIBRARIES(p4p_common_cpp_example_pgm	p4p_common_cpp)

ADD_EXECUTABLE(p4p_common_cpp_example_log		examples/logging_override.cpp)
TARGET_LINK_LIBRARIES(p4p_common_cpp_example_log	p4p_common_cpp)

ADD_EXECUTABLE(p4p_common_cpp_example_pgm_test		examples/pgm_test.cpp)
TARGET_LINK_LIBRARIES(p4p_common_cpp_example_pgm_test	p4p_common_cpp)

IF(P4P_TESTING)
	FIND_PACKAGE(Boost ${BOOST_MIN_VERSION}
		COMPONENTS
			unit_test_framework
			thread
			program_options
		REQUIRED)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

	SET(LIBS ${LIBS} ${Boost_LIBRARIES})

	ADD_EXECUTABLE(p4p_common_cpp_unittest
		unittest/main.cpp
		unittest/data/pid.cpp
		unittest/data/isp.cpp
		unittest/data/heap_with_delete.cpp
		unittest/data/ip_addr.cpp
		unittest/data/patricia.cpp
		)
	TARGET_LINK_LIBRARIES(p4p_common_cpp_unittest ${LIBS} p4p_common_cpp)
	AddUnitTest(p4p_common_cpp_unittest)

ENDIF(P4P_TESTING)

