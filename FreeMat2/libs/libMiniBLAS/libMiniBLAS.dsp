# Microsoft Developer Studio Project File - Name="libMiniBLAS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libMiniBLAS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libMiniBLAS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libMiniBLAS.mak" CFG="libMiniBLAS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libMiniBLAS - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libMiniBLAS - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libMiniBLAS - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libMiniBLAS - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "\Freemat2\libs\libf2c" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
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

# Name "libMiniBLAS - Win32 Release"
# Name "libMiniBLAS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\f2c\caxpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ccopy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cdotc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cdotu.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgemm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgemv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgerc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgeru.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cscal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\csscal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cswap.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctrmm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctrmv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctrsm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctrsv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dasum.c
# End Source File
# Begin Source File

SOURCE=.\f2c\daxpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dcabs1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dcopy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ddot.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgemm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgemv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dger.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlamch.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dnrm2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\drot.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dscal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dswap.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtrmm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtrmv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtrsm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtrsv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dzasum.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dznrm2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\icamax.c
# End Source File
# Begin Source File

SOURCE=.\f2c\idamax.c
# End Source File
# Begin Source File

SOURCE=.\f2c\isamax.c
# End Source File
# Begin Source File

SOURCE=.\f2c\izamax.c
# End Source File
# Begin Source File

SOURCE=.\f2c\lsame.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sasum.c
# End Source File
# Begin Source File

SOURCE=.\f2c\saxpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\scasum.c
# End Source File
# Begin Source File

SOURCE=.\f2c\scnrm2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\scopy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sdot.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgemm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgemv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sger.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slamch.c
# End Source File
# Begin Source File

SOURCE=.\f2c\snrm2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\srot.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sscal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sswap.c
# End Source File
# Begin Source File

SOURCE=.\f2c\strmm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\strmv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\strsm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\strsv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\xerbla.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zaxpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zcopy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zdotc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zdotu.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zdscal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgemm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgemv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgerc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgeru.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zscal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zswap.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztrmm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztrmv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztrsm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztrsv.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
