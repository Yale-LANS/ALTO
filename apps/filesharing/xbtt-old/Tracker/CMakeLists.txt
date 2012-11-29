cmake_minimum_required(VERSION 2.4)
cmake_policy(SET CMP0003 OLD)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
include_directories(. ../misc)
include(CheckIncludeFileCXX)
check_include_file_cxx(sys/epoll.h HAVE_SYS_EPOLL)
if(HAVE_SYS_EPOLL)
       add_definitions(-DEPOLL)
endif()

# Modified: Define extra P4P sources
SET(P4P_SRCS
	p4p_config.cpp
	tinyxml/tinystr.cpp
	tinyxml/tinyxml.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp
	)

# Modified: Add Windows Service for Win32
SET(WIN32_SRCS)
IF(WIN32)
	SET(WIN32_SRCS
		../misc/windows/nt_service.cpp
		)
ENDIF(WIN32)

# Modified: Add variable to include extra P4P sources and Win32 sources
add_executable(
	xbt_tracker EXCLUDE_FROM_ALL
	../misc/sql/database.cpp
	../misc/sql/sql_query.cpp
	../misc/sql/sql_result.cpp
	../misc/bt_misc.cpp
	../misc/bvalue.cpp
	../misc/sha1.cpp
	../misc/socket.cpp
	../misc/virtual_binary.cpp
	../misc/xcc_z.cpp
	client.cpp
	config.cpp
	connection.cpp
	epoll.cpp
	server.cpp
	tcp_listen_socket.cpp
	tracker_input.cpp
	transaction.cpp
	udp_listen_socket.cpp
	"XBT Tracker.cpp"
	${P4P_SRCS}
	${WIN32_SRCS}
)

# Modified: Changed inclusion mechanism for MySQL library
target_link_libraries(xbt_tracker ${LIBS})

# Modified: Add installation
#INSTALL(TARGETS xbt_tracker
#	RUNTIME DESTINATION bin
#	COMPONENT TrackerDevel
#	)

