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


PROJECT(trackeremulator)

# This project is currently not compatible with Windows
IF (WIN32)
	MESSAGE(STATUS "Skipping ${PROJECT_NAME} on Windows")
	RETURN()
ENDIF (WIN32)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/CommonInclude.cmake)

FIND_PACKAGE(p4p_common_cpp)
CheckLibFound(p4p_common_cpp)
INCLUDE_DIRECTORIES(${p4p_common_cpp_INCLUDE_DIR})
LINK_DIRECTORIES(${p4p_common_cpp_LIBRARY_DIR})
SET(LIBS ${LIBS} ${p4p_common_cpp_LIBRARY})

INCLUDE_DIRECTORIES(
	src
	)

SET(SRCS
	src/main.cpp
	src/meminfo.cpp
	src/options.cpp
	src/tracker.cpp
	src/channel.cpp
	src/peer.cpp
	)

ADD_EXECUTABLE(tracker_emulator_p4p ${SRCS})
TARGET_LINK_LIBRARIES(tracker_emulator_p4p ${LIBS})

ADD_EXECUTABLE(tracker_emulator_native ${SRCS})
TARGET_LINK_LIBRARIES(tracker_emulator_native)
SET_TARGET_PROPERTIES(tracker_emulator_native
	PROPERTIES COMPILE_DEFINITIONS "NO_P4P"
	)

INSTALL(TARGETS tracker_emulator_native tracker_emulator_p4p
        RUNTIME DESTINATION bin
	COMPONENT TrackerDevel
	)

# Install the raw source code
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
	DESTINATION share/${PROJECT_NAME}
	COMPONENT TrackerDevel
	PATTERN ".svn" EXCLUDE
	PATTERN "cmake_install.cmake" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	)
