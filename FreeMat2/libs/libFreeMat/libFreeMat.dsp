# Microsoft Developer Studio Project File - Name="libFreeMat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libFreeMat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libFreeMat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libFreeMat.mak" CFG="libFreeMat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libFreeMat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libFreeMat - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "libFreeMat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zd /O2 /I "..\libffi\msvc_build" /I "..\libXP" /I "..\libf2c" /I "..\..\src\win32" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "YY_NEVER_INTERACTIVE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libFreeMat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\libffi\msvc_build" /I "..\libXP" /I "..\libf2c" /I "..\..\src\win32" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "YY_NEVER_INTERACTIVE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libFreeMat - Win32 Release"
# Name "libFreeMat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Array.cpp
# End Source File
# Begin Source File

SOURCE=.\AST.cpp
# End Source File
# Begin Source File

SOURCE=.\Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Context.cpp
# End Source File
# Begin Source File

SOURCE=.\Data.cpp
# End Source File
# Begin Source File

SOURCE=.\Dimensions.cpp
# End Source File
# Begin Source File

SOURCE=.\EigenDecompose.cpp
# End Source File
# Begin Source File

SOURCE=.\Exception.cpp
# End Source File
# Begin Source File

SOURCE=.\File.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionDef.cpp
# End Source File
# Begin Source File

SOURCE=.\IEEEFP.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\LAPACK.cpp
# End Source File
# Begin Source File

SOURCE=.\LeastSquaresSolver.cpp
# End Source File
# Begin Source File

SOURCE=.\Lexer.cxx
# End Source File
# Begin Source File

SOURCE=.\LinearEqSolver.cpp
# End Source File
# Begin Source File

SOURCE=.\Malloc.cpp
# End Source File
# Begin Source File

SOURCE=.\Math.cpp
# End Source File
# Begin Source File

SOURCE=.\MatrixMultiply.cpp
# End Source File
# Begin Source File

SOURCE=.\Module.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser.cxx
# End Source File
# Begin Source File

SOURCE=.\Reserved.cpp
# End Source File
# Begin Source File

SOURCE=.\Scope.cpp
# End Source File
# Begin Source File

SOURCE=.\Serialize.cpp
# End Source File
# Begin Source File

SOURCE=.\SingularValueDecompose.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorOps.cpp
# End Source File
# Begin Source File

SOURCE=.\f2c\VectorOps_assist.c
# End Source File
# Begin Source File

SOURCE=.\WalkTree.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Array.hpp
# End Source File
# Begin Source File

SOURCE=.\AST.hpp
# End Source File
# Begin Source File

SOURCE=.\Command.hpp
# End Source File
# Begin Source File

SOURCE=.\Context.hpp
# End Source File
# Begin Source File

SOURCE=.\Data.hpp
# End Source File
# Begin Source File

SOURCE=.\Dimensions.hpp
# End Source File
# Begin Source File

SOURCE=.\EigenDecompose.hpp
# End Source File
# Begin Source File

SOURCE=.\Exception.hpp
# End Source File
# Begin Source File

SOURCE=.\File.hpp
# End Source File
# Begin Source File

SOURCE=.\FunctionDef.hpp
# End Source File
# Begin Source File

SOURCE=.\HandleList.hpp
# End Source File
# Begin Source File

SOURCE=.\IEEEFP.hpp
# End Source File
# Begin Source File

SOURCE=.\Interface.hpp
# End Source File
# Begin Source File

SOURCE=.\LAPACK.hpp
# End Source File
# Begin Source File

SOURCE=.\LeastSquaresSolver.hpp
# End Source File
# Begin Source File

SOURCE=.\LexerInterface.hpp
# End Source File
# Begin Source File

SOURCE=.\LinearEqSolver.hpp
# End Source File
# Begin Source File

SOURCE=.\Malloc.hpp
# End Source File
# Begin Source File

SOURCE=.\Math.hpp
# End Source File
# Begin Source File

SOURCE=.\MatrixMultiply.hpp
# End Source File
# Begin Source File

SOURCE=.\Module.hpp
# End Source File
# Begin Source File

SOURCE=.\Parser.h
# End Source File
# Begin Source File

SOURCE=.\ParserInterface.hpp
# End Source File
# Begin Source File

SOURCE=.\Reserved.hpp
# End Source File
# Begin Source File

SOURCE=.\Scope.hpp
# End Source File
# Begin Source File

SOURCE=.\Serialize.hpp
# End Source File
# Begin Source File

SOURCE=.\ServerSocket.hpp
# End Source File
# Begin Source File

SOURCE=.\SingularValueDecompose.hpp
# End Source File
# Begin Source File

SOURCE=.\Socket.hpp
# End Source File
# Begin Source File

SOURCE=.\Stream.hpp
# End Source File
# Begin Source File

SOURCE=.\SymbolTable.hpp
# End Source File
# Begin Source File

SOURCE=.\Types.hpp
# End Source File
# Begin Source File

SOURCE=.\VectorOps.hpp
# End Source File
# Begin Source File

SOURCE=.\WalkTree.hpp
# End Source File
# End Group
# End Target
# End Project
