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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Test/TestExecutor.o \
	${OBJECTDIR}/main.o


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
LDLIBSOPTIONS=../volodb-client/dist/Release/GNU-Linux/libvolodb-client.a ../volodb-common/dist/Release/GNU-Linux/libvolodb-common.a /usr/lib/x86_64-linux-gnu/libzmq.a /usr/lib/x86_64-linux-gnu/libprotobuf.a /usr/lib/x86_64-linux-gnu/libboost_program_options.a /usr/lib/x86_64-linux-gnu/libpgm.a /usr/lib/x86_64-linux-gnu/libsodium.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: ../volodb-client/dist/Release/GNU-Linux/libvolodb-client.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: ../volodb-common/dist/Release/GNU-Linux/libvolodb-common.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: /usr/lib/x86_64-linux-gnu/libzmq.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: /usr/lib/x86_64-linux-gnu/libprotobuf.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: /usr/lib/x86_64-linux-gnu/libboost_program_options.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: /usr/lib/x86_64-linux-gnu/libpgm.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: /usr/lib/x86_64-linux-gnu/libsodium.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp ${OBJECTFILES} ${LDLIBSOPTIONS} -lpthread -ldl -lrt

${OBJECTDIR}/Test/TestExecutor.o: Test/TestExecutor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Test
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -I../volodb-common -I../volodb-client -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Test/TestExecutor.o Test/TestExecutor.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -I../volodb-common -I../volodb-client -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd ../volodb-client && ${MAKE}  -f Makefile CONF=Release
	cd ../volodb-common && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbclientapp

# Subprojects
.clean-subprojects:
	cd ../volodb-client && ${MAKE}  -f Makefile CONF=Release clean
	cd ../volodb-common && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
