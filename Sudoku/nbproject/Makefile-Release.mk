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
CND_PLATFORM=Cygwin-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/Engine/Engine.o \
	${OBJECTDIR}/src/Engine/Log.o \
	${OBJECTDIR}/src/Game/Controller.o \
	${OBJECTDIR}/src/Game/Sudoku.o \
	${OBJECTDIR}/src/Game/View.o \
	${OBJECTDIR}/src/Graphics/Frame.o \
	${OBJECTDIR}/src/main.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sudoku.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sudoku.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sudoku ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/Engine/Engine.o: src/Engine/Engine.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Engine
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Isrc -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Engine/Engine.o src/Engine/Engine.cpp

${OBJECTDIR}/src/Engine/Log.o: src/Engine/Log.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Engine
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Isrc -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Engine/Log.o src/Engine/Log.cpp

${OBJECTDIR}/src/Game/Controller.o: src/Game/Controller.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Isrc -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Game/Controller.o src/Game/Controller.cpp

${OBJECTDIR}/src/Game/Sudoku.o: src/Game/Sudoku.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Isrc -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Game/Sudoku.o src/Game/Sudoku.cpp

${OBJECTDIR}/src/Game/View.o: src/Game/View.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Game
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Isrc -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Game/View.o src/Game/View.cpp

${OBJECTDIR}/src/Graphics/Frame.o: src/Graphics/Frame.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Graphics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Isrc -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Graphics/Frame.o src/Graphics/Frame.cpp

${OBJECTDIR}/src/main.o: src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Isrc -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
