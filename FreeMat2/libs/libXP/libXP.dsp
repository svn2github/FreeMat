# Microsoft Developer Studio Project File - Name="libXP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libXP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libXP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libXP.mak" CFG="libXP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libXP - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libXP - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "libXP - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libXP - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\libFreeMat" /I "..\lpng125" /I "..\libz" /I "..\libjpeg6b" /I "..\libtiff361\libtiff" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "HAVE_PNG" /D "HAVE_JPEG" /D "HAVE_TIFF" /YX /FD /GZ /c
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

# Name "libXP - Win32 Release"
# Name "libXP - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BitmapFont.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapPrinterGC.cpp
# End Source File
# Begin Source File

SOURCE=.\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\DynLib.cpp
# End Source File
# Begin Source File

SOURCE=.\GrayscaleImage.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyManager.cpp
# End Source File
# Begin Source File

SOURCE=.\PathSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\Point2D.cpp
# End Source File
# Begin Source File

SOURCE=.\PostScriptGC.cpp
# End Source File
# Begin Source File

SOURCE=.\Rect2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Reducer.cpp
# End Source File
# Begin Source File

SOURCE=.\RGBImage.cpp
# End Source File
# Begin Source File

SOURCE=.\RGBImageGC.cpp
# End Source File
# Begin Source File

SOURCE=.\System.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BitmapFont.hpp
# End Source File
# Begin Source File

SOURCE=.\Color.hpp
# End Source File
# Begin Source File

SOURCE=.\DynLib.hpp
# End Source File
# Begin Source File

SOURCE=.\GraphicsContext.hpp
# End Source File
# Begin Source File

SOURCE=.\GrayscaleImage.hpp
# End Source File
# Begin Source File

SOURCE=.\KeyManager.hpp
# End Source File
# Begin Source File

SOURCE=.\PathSearch.hpp
# End Source File
# Begin Source File

SOURCE=.\Point2D.hpp
# End Source File
# Begin Source File

SOURCE=.\PostScriptGC.hpp
# End Source File
# Begin Source File

SOURCE=.\Rect2D.hpp
# End Source File
# Begin Source File

SOURCE=.\Reducer.hpp
# End Source File
# Begin Source File

SOURCE=.\RGBImage.hpp
# End Source File
# Begin Source File

SOURCE=.\RGBImageGC.hpp
# End Source File
# Begin Source File

SOURCE=.\System.hpp
# End Source File
# End Group
# End Target
# End Project
