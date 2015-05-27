#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Executor/Executor.o \
	${OBJECTDIR}/Transport/ClientTransport.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-D_GLIBCXX_USE_NANOSLEEP
CXXFLAGS=-D_GLIBCXX_USE_NANOSLEEP

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-client.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-client.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-client.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-client.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-client.a

${OBJECTDIR}/Executor/Executor.o: Executor/Executor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Executor
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Executor/Executor.o Executor/Executor.cpp

${OBJECTDIR}/Transport/ClientTransport.o: Transport/ClientTransport.cpp 
	${MKDIR} -p ${OBJECTDIR}/Transport
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Transport/ClientTransport.o Transport/ClientTransport.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-client.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
