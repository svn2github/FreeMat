# Microsoft Developer Studio Project File - Name="FreeMat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FreeMat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FreeMat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FreeMat.mak" CFG="FreeMat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FreeMat - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FreeMat - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreeMat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zd /O2 /I "libs\libXP" /I "libs\libFreeMat" /I "libs\libffi\msvc_build" /I "libs\libCore" /I "libs\libGraphics" /I "C:/Program\ Files/HTML\ Help\ Workshop/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib libs\libCore\Release\libCore.lib libs\libf2c\Release\libf2c.lib libs\libFreeMat\Release\libFreeMat.lib libs\libGraphics\Release\libGraphics.lib libs\libMiniBLAS\Release\libMiniBLAS.lib libs\libMiniLAPACK\Release\libMiniLAPACK.lib libs\libXP\Release\libXP.lib libs\libFFTPack\Release\libFFTPack.lib libs\libffi\Release\libffi.lib libs\lpng125\Release\lpng125.lib libs\libz\Release\libz.lib src\win32\htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "FreeMat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "libs\libXP" /I "libs\libFreeMat" /I "libs\libffi\msvc_build" /I "libs\libCore" /I "libs\libGraphics" /I "C:/Program\ Files/HTML\ Help\ Workshop/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib libs\libCore\Debug\libCore.lib libs\libf2c\Debug\libf2c.lib libs\libFreeMat\Debug\libFreeMat.lib libs\libGraphics\Debug\libGraphics.lib libs\libMiniBLAS\Debug\libMiniBLAS.lib libs\libMiniLAPACK\Debug\libMiniLAPACK.lib libs\libXP\Debug\libXP.lib libs\libFFTPack\Debug\libFFTPack.lib libs\libffi\Debug\libffi.lib libs\lpng125\Debug\lpng125.lib libs\libz\Debug\libz.lib src\win32\htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "FreeMat - Win32 Release"
# Name "FreeMat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\win32\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\win32\Script1.rc
# End Source File
# Begin Source File

SOURCE=.\src\win32\WinGC.cpp
# End Source File
# Begin Source File

SOURCE=.\src\win32\WinTerminal.cpp
# End Source File
# Begin Source File

SOURCE=.\src\win32\XWindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\win32\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\win32\WinGC.hpp
# End Source File
# Begin Source File

SOURCE=.\src\win32\WinTerminal.hpp
# End Source File
# Begin Source File

SOURCE=.\src\win32\XWindow.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\win32\freemat.ico
# End Source File
# Begin Source File

SOURCE=.\src\win32\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\src\win32\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\src\win32\phi_logo_bmp.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\win32\htmlhelp.lib
# End Source File
# End Target
# End Project
