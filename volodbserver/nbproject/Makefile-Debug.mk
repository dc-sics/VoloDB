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
	${OBJECTDIR}/Cache/TableInfoCache.o \
	${OBJECTDIR}/Store/NDBMessageExecutor.o \
	${OBJECTDIR}/Store/NDBMessageProcessor.o \
	${OBJECTDIR}/Store/Store.o \
	${OBJECTDIR}/Test/TestExecutor.o \
	${OBJECTDIR}/Test/TestStats.o \
	${OBJECTDIR}/Transport/ServerTransport.o \
	${OBJECTDIR}/Utils/TransportUtils.o \
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
LDLIBSOPTIONS=/usr/local/mysql/lib/libndbclient_static.a /usr/local/mysql/lib/libmysqlclient.a /usr/local/mysql/lib/libmysqld.a ../volodb-common/dist/Release/GNU-MacOSX/libvolodb-common.a /usr/local/lib/libzmq.a /usr/local/lib/libprotobuf.a /usr/local/lib/libboost_program_options.a /usr/local/lib/libboost_thread.a /usr/local/lib/libboost_system.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/mysql/lib/libndbclient_static.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/mysql/lib/libmysqlclient.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/mysql/lib/libmysqld.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: ../volodb-common/dist/Release/GNU-MacOSX/libvolodb-common.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/lib/libzmq.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/lib/libprotobuf.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/lib/libboost_program_options.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/lib/libboost_thread.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: /usr/local/lib/libboost_system.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver ${OBJECTFILES} ${LDLIBSOPTIONS} -lpthread -ldl -lrt

${OBJECTDIR}/Cache/TableInfoCache.o: Cache/TableInfoCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/Cache
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Cache/TableInfoCache.o Cache/TableInfoCache.cpp

${OBJECTDIR}/Store/NDBMessageExecutor.o: Store/NDBMessageExecutor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Store
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Store/NDBMessageExecutor.o Store/NDBMessageExecutor.cpp

${OBJECTDIR}/Store/NDBMessageProcessor.o: Store/NDBMessageProcessor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Store
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Store/NDBMessageProcessor.o Store/NDBMessageProcessor.cpp

${OBJECTDIR}/Store/Store.o: Store/Store.cpp 
	${MKDIR} -p ${OBJECTDIR}/Store
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Store/Store.o Store/Store.cpp

${OBJECTDIR}/Test/TestExecutor.o: Test/TestExecutor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Test
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Test/TestExecutor.o Test/TestExecutor.cpp

${OBJECTDIR}/Test/TestStats.o: Test/TestStats.cpp 
	${MKDIR} -p ${OBJECTDIR}/Test
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Test/TestStats.o Test/TestStats.cpp

${OBJECTDIR}/Transport/ServerTransport.o: Transport/ServerTransport.cpp 
	${MKDIR} -p ${OBJECTDIR}/Transport
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Transport/ServerTransport.o Transport/ServerTransport.cpp

${OBJECTDIR}/Utils/TransportUtils.o: Utils/TransportUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/Utils
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utils/TransportUtils.o Utils/TransportUtils.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/mysql/include -I/usr/local/mysql/include/storage/ndb/ndbapi -I/usr/local/mysql/include/storage/ndb -I../volodb-common -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd ../volodb-common && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/volodbserver

# Subprojects
.clean-subprojects:
	cd ../volodb-common && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
