EXECUTE_PROCESS(COMMAND uname -m OUTPUT_VARIABLE MACHINE_ARCH)
STRING(REGEX REPLACE "\n" "" MACHINE_ARCH "${MACHINE_ARCH}")
IF(MACHINE_ARCH MATCHES "i.86")
	SET(MACHINE_ARCH "x86")
ENDIF(MACHINE_ARCH MATCHES "i.86")
IF(MACHINE_ARCH MATCHES "x86.64")
	SET(MACHINE_ARCH "x86-64")
ENDIF(MACHINE_ARCH MATCHES "x86.64")

FIND_PATH(Cplex_INCLUDE_DIR ilcplex/ilocplex.h /opt/ilog/cplex100/include)
FIND_PATH(Cplex_concert_INCLUDE_DIR ilconcert/iloenv.h /opt/ilog/concert22/include)
FIND_LIBRARY(Cplex_LIBRARY NAMES cplex PATHS /opt/ilog/cplex100/lib/${MACHINE_ARCH}_rhel4.0_3.4/static_pic)
FIND_LIBRARY(Cplex_ilo_LIBRARY NAMES ilocplex PATHS /opt/ilog/cplex100/lib/${MACHINE_ARCH}_rhel4.0_3.4/static_pic)
FIND_LIBRARY(Cplex_concert_LIBRARY NAMES concert PATHS /opt/ilog/concert22/lib/${MACHINE_ARCH}_rhel4.0_3.4/static_pic)

IF (Cplex_INCLUDE_DIR AND Cplex_LIBRARY AND Cplex_ilo_LIBRARY AND Cplex_concert_INCLUDE_DIR AND Cplex_concert_LIBRARY)
	SET(Cplex_FOUND TRUE)

	SET(Cplex_LIBRARY ${Cplex_ilo_LIBRARY} ${Cplex_LIBRARY} ${Cplex_concert_LIBRARY})
	SET(Cplex_INCLUDE_DIR ${Cplex_INCLUDE_DIR} ${Cplex_concert_INCLUDE_DIR})

ENDIF (Cplex_INCLUDE_DIR AND Cplex_LIBRARY AND Cplex_ilo_LIBRARY AND Cplex_concert_INCLUDE_DIR AND Cplex_concert_LIBRARY)


IF (Cplex_FOUND)
	IF (NOT Cplex_FIND_QUIETLY)
		MESSAGE(STATUS "Found cplex: ${Cplex_LIBRARY}")
	ENDIF (NOT Cplex_FIND_QUIETLY)
ELSE (Cplex_FOUND)
	IF (Cplex_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find cplex")
	ENDIF (Cplex_FIND_REQUIRED)
ENDIF (Cplex_FOUND)

