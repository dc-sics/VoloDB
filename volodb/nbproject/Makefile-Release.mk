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
	${OBJECTDIR}/_ext/13ebf2bf/TableInfoCache.o \
	${OBJECTDIR}/_ext/14d63c1e/NDBMessageExecutor.o \
	${OBJECTDIR}/_ext/14d63c1e/NDBMessageProcessor.o \
	${OBJECTDIR}/_ext/14d63c1e/Store.o \
	${OBJECTDIR}/_ext/6c0725b5/TestExecutor.o \
	${OBJECTDIR}/_ext/6c0725b5/TestStats.o \
	${OBJECTDIR}/_ext/e31053e6/ServerTransport.o \
	${OBJECTDIR}/_ext/14f253ee/TransportUtils.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb.a

${OBJECTDIR}/_ext/13ebf2bf/TableInfoCache.o: ../volodbserver/Cache/TableInfoCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/13ebf2bf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/13ebf2bf/TableInfoCache.o ../volodbserver/Cache/TableInfoCache.cpp

${OBJECTDIR}/_ext/14d63c1e/NDBMessageExecutor.o: ../volodbserver/Store/NDBMessageExecutor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/14d63c1e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/14d63c1e/NDBMessageExecutor.o ../volodbserver/Store/NDBMessageExecutor.cpp

${OBJECTDIR}/_ext/14d63c1e/NDBMessageProcessor.o: ../volodbserver/Store/NDBMessageProcessor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/14d63c1e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/14d63c1e/NDBMessageProcessor.o ../volodbserver/Store/NDBMessageProcessor.cpp

${OBJECTDIR}/_ext/14d63c1e/Store.o: ../volodbserver/Store/Store.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/14d63c1e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/14d63c1e/Store.o ../volodbserver/Store/Store.cpp

${OBJECTDIR}/_ext/6c0725b5/TestExecutor.o: ../volodbserver/Test/TestExecutor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6c0725b5
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6c0725b5/TestExecutor.o ../volodbserver/Test/TestExecutor.cpp

${OBJECTDIR}/_ext/6c0725b5/TestStats.o: ../volodbserver/Test/TestStats.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6c0725b5
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6c0725b5/TestStats.o ../volodbserver/Test/TestStats.cpp

${OBJECTDIR}/_ext/e31053e6/ServerTransport.o: ../volodbserver/Transport/ServerTransport.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e31053e6
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e31053e6/ServerTransport.o ../volodbserver/Transport/ServerTransport.cpp

${OBJECTDIR}/_ext/14f253ee/TransportUtils.o: ../volodbserver/Utils/TransportUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/14f253ee
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I/usr/local/mysql/include/storage/ndb/ -I/usr/local/include -I../volodb-common -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/14f253ee/TransportUtils.o ../volodbserver/Utils/TransportUtils.cpp

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
