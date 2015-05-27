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
	${OBJECTDIR}/_ext/334230207/TableInfoCache.o \
	${OBJECTDIR}/_ext/349584414/NDBMessageExecutor.o \
	${OBJECTDIR}/_ext/349584414/NDBMessageProcessor.o \
	${OBJECTDIR}/_ext/349584414/Store.o \
	${OBJECTDIR}/_ext/1812407733/TestExecutor.o \
	${OBJECTDIR}/_ext/1812407733/TestStats.o \
	${OBJECTDIR}/_ext/485469210/ServerTransport.o \
	${OBJECTDIR}/_ext/351425518/TransportUtils.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a

${OBJECTDIR}/_ext/334230207/TableInfoCache.o: ../volodbserver/Cache/TableInfoCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/334230207
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/334230207/TableInfoCache.o ../volodbserver/Cache/TableInfoCache.cpp

${OBJECTDIR}/_ext/349584414/NDBMessageExecutor.o: ../volodbserver/Store/NDBMessageExecutor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/349584414
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/349584414/NDBMessageExecutor.o ../volodbserver/Store/NDBMessageExecutor.cpp

${OBJECTDIR}/_ext/349584414/NDBMessageProcessor.o: ../volodbserver/Store/NDBMessageProcessor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/349584414
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/349584414/NDBMessageProcessor.o ../volodbserver/Store/NDBMessageProcessor.cpp

${OBJECTDIR}/_ext/349584414/Store.o: ../volodbserver/Store/Store.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/349584414
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/349584414/Store.o ../volodbserver/Store/Store.cpp

${OBJECTDIR}/_ext/1812407733/TestExecutor.o: ../volodbserver/Test/TestExecutor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1812407733
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1812407733/TestExecutor.o ../volodbserver/Test/TestExecutor.cpp

${OBJECTDIR}/_ext/1812407733/TestStats.o: ../volodbserver/Test/TestStats.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1812407733
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1812407733/TestStats.o ../volodbserver/Test/TestStats.cpp

${OBJECTDIR}/_ext/485469210/ServerTransport.o: ../volodbserver/Transport/ServerTransport.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/485469210
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/485469210/ServerTransport.o ../volodbserver/Transport/ServerTransport.cpp

${OBJECTDIR}/_ext/351425518/TransportUtils.o: ../volodbserver/Utils/TransportUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/351425518
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/351425518/TransportUtils.o ../volodbserver/Utils/TransportUtils.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
