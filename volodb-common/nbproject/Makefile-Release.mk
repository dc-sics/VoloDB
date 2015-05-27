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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AttributeUtils/ColumnsValueBuilder.o \
	${OBJECTDIR}/AttributeUtils/ColumnsValueDecoder.o \
	${OBJECTDIR}/Config/ConfigFileReader.o \
	${OBJECTDIR}/Frame/FrameBuilder.o \
	${OBJECTDIR}/Frame/FrameDecoder.o \
	${OBJECTDIR}/Frame/FrameUtils.o \
	${OBJECTDIR}/OperationUtils/OperationUtils.o \
	${OBJECTDIR}/ProtoBuf/Column.pb.o \
	${OBJECTDIR}/ProtoBuf/DefinitionOperation.pb.o \
	${OBJECTDIR}/ProtoBuf/Filter.pb.o \
	${OBJECTDIR}/ProtoBuf/Frame.pb.o \
	${OBJECTDIR}/ProtoBuf/ManipulationOperations.pb.o \
	${OBJECTDIR}/ProtoBuf/Operation.pb.o \
	${OBJECTDIR}/ProtoBuf/Result.pb.o \
	${OBJECTDIR}/Test/TestBase.o \
	${OBJECTDIR}/Utils/Utils.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-common.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-common.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-common.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-common.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-common.a

${OBJECTDIR}/AttributeUtils/ColumnsValueBuilder.o: AttributeUtils/ColumnsValueBuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/AttributeUtils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AttributeUtils/ColumnsValueBuilder.o AttributeUtils/ColumnsValueBuilder.cpp

${OBJECTDIR}/AttributeUtils/ColumnsValueDecoder.o: AttributeUtils/ColumnsValueDecoder.cpp 
	${MKDIR} -p ${OBJECTDIR}/AttributeUtils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AttributeUtils/ColumnsValueDecoder.o AttributeUtils/ColumnsValueDecoder.cpp

${OBJECTDIR}/Config/ConfigFileReader.o: Config/ConfigFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/Config
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Config/ConfigFileReader.o Config/ConfigFileReader.cpp

${OBJECTDIR}/Frame/FrameBuilder.o: Frame/FrameBuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/Frame
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Frame/FrameBuilder.o Frame/FrameBuilder.cpp

${OBJECTDIR}/Frame/FrameDecoder.o: Frame/FrameDecoder.cpp 
	${MKDIR} -p ${OBJECTDIR}/Frame
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Frame/FrameDecoder.o Frame/FrameDecoder.cpp

${OBJECTDIR}/Frame/FrameUtils.o: Frame/FrameUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/Frame
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Frame/FrameUtils.o Frame/FrameUtils.cpp

${OBJECTDIR}/OperationUtils/OperationUtils.o: OperationUtils/OperationUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/OperationUtils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OperationUtils/OperationUtils.o OperationUtils/OperationUtils.cpp

${OBJECTDIR}/ProtoBuf/Column.pb.o: ProtoBuf/Column.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/ProtoBuf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProtoBuf/Column.pb.o ProtoBuf/Column.pb.cc

${OBJECTDIR}/ProtoBuf/DefinitionOperation.pb.o: ProtoBuf/DefinitionOperation.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/ProtoBuf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProtoBuf/DefinitionOperation.pb.o ProtoBuf/DefinitionOperation.pb.cc

${OBJECTDIR}/ProtoBuf/Filter.pb.o: ProtoBuf/Filter.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/ProtoBuf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProtoBuf/Filter.pb.o ProtoBuf/Filter.pb.cc

${OBJECTDIR}/ProtoBuf/Frame.pb.o: ProtoBuf/Frame.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/ProtoBuf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProtoBuf/Frame.pb.o ProtoBuf/Frame.pb.cc

${OBJECTDIR}/ProtoBuf/ManipulationOperations.pb.o: ProtoBuf/ManipulationOperations.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/ProtoBuf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProtoBuf/ManipulationOperations.pb.o ProtoBuf/ManipulationOperations.pb.cc

${OBJECTDIR}/ProtoBuf/Operation.pb.o: ProtoBuf/Operation.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/ProtoBuf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProtoBuf/Operation.pb.o ProtoBuf/Operation.pb.cc

${OBJECTDIR}/ProtoBuf/Result.pb.o: ProtoBuf/Result.pb.cc 
	${MKDIR} -p ${OBJECTDIR}/ProtoBuf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProtoBuf/Result.pb.o ProtoBuf/Result.pb.cc

${OBJECTDIR}/Test/TestBase.o: Test/TestBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/Test
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Test/TestBase.o Test/TestBase.cpp

${OBJECTDIR}/Utils/Utils.o: Utils/Utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/Utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utils/Utils.o Utils/Utils.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libvolodb-common.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
