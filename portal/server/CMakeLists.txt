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


PROJECT(p4p_portal)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/CommonInclude.cmake)
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/build/Doxygen.cmake)

FIND_PACKAGE(Boost ${BOOST_MIN_VERSION}
	COMPONENTS
		thread
		date_time
		program_options
		system
		filesystem
		iostreams
		random
	)
CheckLibFound(Boost)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

FIND_PACKAGE(p4p_common_cpp)
CheckLibFound(p4p_common_cpp)
INCLUDE_DIRECTORIES(${p4p_common_cpp_INCLUDE_DIR})
LINK_DIRECTORIES(${p4p_common_cpp_LIBRARY_DIR})

FIND_PACKAGE(p4p_common_server)
CheckLibFound(p4p_common_server)
INCLUDE_DIRECTORIES(${p4p_common_server_INCLUDE_DIR})
LINK_DIRECTORIES(${p4p_common_server_LIBRARY_DIR})

CONFIGURE_FILE(src/build_info.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/build_info.h ESCAPE_QUOTES)

INCLUDE_DIRECTORIES(
	src/infores
	src/admin
	src/global_state
	src/jobs
	src/pdist
	src/pdist/mlu
	src/view
	src/protocol
	src/protocol/include
	src/protocol/rest
	src
	${CMAKE_CURRENT_BINARY_DIR}/src
	)

SET(SRCS
	src/infores/info_resource_base.cpp
	src/infores/info_resource_array.cpp
	src/infores/info_resource_entity.cpp
	src/infores/address_group.cpp
	src/infores/alto_directory.cpp
	src/infores/network_map.cpp
	src/infores/cost_map.cpp
	src/infores/post_filters.cpp
	src/infores/endpoints.cpp
	src/infores/error_resource_entity.cpp
	src/admin/admin_state.cpp
	src/admin/admin_view.cpp
	src/admin/admin_net.cpp
	src/global_state/global_state.cpp
	src/jobs/view_update_job.cpp
	src/pdist/plugin_base.cpp
	src/pdist/plugin_registry.cpp
	src/pdist/mlu/mlu.cpp
	src/view/view.cpp
	src/view/view_update.cpp
	src/view/view_registry.cpp
	src/protocol/rest/rest_request_handlers.cpp
	src/protocol/rest/rest_request_handlers_view.cpp
	src/protocol/rest/rest_request_handlers_json.cpp
	src/protocol/rest/rest_request_handlers_admin.cpp
	src/options.cpp
	src/state.cpp
	)

SET(LIBS ${LIBS} ${Boost_LIBRARIES})
SET(LIBS ${LIBS} ${p4p_common_cpp_LIBRARY})
SET(LIBS ${LIBS} ${p4p_common_server_LIBRARY})

ADD_EXECUTABLE(p4p_portal
	${SRCS}
	${CMAKE_CURRENT_BINARY_DIR}/src/build_info.h
	src/main.cpp
	)
TARGET_LINK_LIBRARIES(p4p_portal ${LIBS})

INSTALL(TARGETS p4p_portal
	RUNTIME DESTINATION bin
	COMPONENT PortalServer
	)

INSTALL(FILES resource/conf/p4p-portal.conf resource/conf/p4p-portal-intf.conf
	DESTINATION /etc/p4p
	COMPONENT PortalServer
	)

INSTALL(DIRECTORY src/protocol/include/
	DESTINATION include/
	COMPONENT PortalServerDev
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
	)

InstallExtraLibs(${Boost_LIBRARIES})

Doxygen(Doxyfile)
