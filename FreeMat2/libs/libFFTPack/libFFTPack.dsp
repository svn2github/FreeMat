# Microsoft Developer Studio Project File - Name="libFFTPack" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libFFTPack - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libFFTPack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libFFTPack.mak" CFG="libFFTPack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libFFTPack - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libFFTPack - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libFFTPack - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /Zd /O2 /I "..\libf2c" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libFFTPack - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\libf2c" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libFFTPack - Win32 Release"
# Name "libFFTPack - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\f2c\cfftb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cfftb1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cfftf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cfftf1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cffti.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cffti1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassb2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassb3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassb4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassb5.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassf2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassf3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassf4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cpassf5.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zfftb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zfftb1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zfftf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zfftf1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zffti.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zffti1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassb2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassb3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassb4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassb5.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassf2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassf3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassf4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zpassf5.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
