# Microsoft Developer Studio Project File - Name="libMiniLAPACK" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libMiniLAPACK - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libMiniLAPACK.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libMiniLAPACK.mak" CFG="libMiniLAPACK - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libMiniLAPACK - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libMiniLAPACK - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libMiniLAPACK - Win32 Release"

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

!ELSEIF  "$(CFG)" == "libMiniLAPACK - Win32 Debug"

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

# Name "libMiniLAPACK - Win32 Release"
# Name "libMiniLAPACK - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\f2c\lsame.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgebak.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgebal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgebd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgebrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgecon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgeequ.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgeevx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgehd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgehrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgelq2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgelqf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgelsy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgeqp3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgeqr2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgeqrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgerfs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgesdd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgesvx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgetf2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgetrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cgetrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\chseqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clabrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clacgv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clacon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clacp2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clacpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clacrm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cladiv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clahqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clahrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\claic1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clange.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clanhs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clantr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\claqge.c
# End Source File
# Begin Source File

SOURCE=.\f2c\claqp2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\claqps.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarcm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarfb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarfg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarft.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarfx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clartg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarzb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clarzt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clascl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\claset.c
# End Source File
# Begin Source File

SOURCE=.\f2c\classq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\claswp.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clatrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\clatrz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\crot.c
# End Source File
# Begin Source File

SOURCE=.\f2c\csrscl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctrevc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctrexc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctrsna.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ctzrzf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cung2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cungbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunghr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cungl2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunglq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cungqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunm2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunmbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunml2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunmlq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunmqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunmr3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\cunmrz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dbdsdc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dbdsqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgebak.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgebal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgebd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgebrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgecon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgeequ.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgeevx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgehd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgehrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgelq2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgelqf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgelsy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgeqp3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgeqr2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgeqrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgerfs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgesdd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgesvx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgetf2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgetrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dgetrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dhseqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlabad.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlabrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlacon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlacpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dladiv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaed6.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaexc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlahqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlahrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaic1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaln2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlamch.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlamrg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlange.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlanhs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlanst.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlantr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlanv2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlapy2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlapy3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaqge.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaqp2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaqps.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaqtr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarfb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarfg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarft.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarfx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlartg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarzb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlarzt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlas2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlascl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd0.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd5.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd6.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd7.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasd8.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasda.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasdq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasdt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaset.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasq1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasq2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasq3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasq4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasq5.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasq6.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasrt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlassq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasv2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlaswp.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlasy2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlatrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dlatrz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorg2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorgbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorghr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorgl2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorglq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorgqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorm2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dormbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dorml2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dormlq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dormqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dormr3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dormrz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\drscl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtrevc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtrexc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtrsna.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dtzrzf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\dzsum1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\icmax1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ieeeck.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ilaenv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\izmax1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sbdsdc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sbdsqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\scsum1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgebak.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgebal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgebd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgebrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgecon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgeequ.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgeevx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgehd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgehrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgelq2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgelqf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgelsy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgeqp3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgeqr2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgeqrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgerfs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgesdd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgesvx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgetf2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgetrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sgetrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\shseqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slabad.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slabrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slacon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slacpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sladiv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaed6.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaexc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slahqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slahrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaic1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaln2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slamch.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slamrg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slange.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slanhs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slanst.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slantr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slanv2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slapy2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slapy3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaqge.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaqp2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaqps.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaqtr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarfb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarfg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarft.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarfx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slartg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarzb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slarzt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slas2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slascl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd0.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd5.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd6.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd7.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasd8.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasda.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasdq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasdt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaset.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasq1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasq2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasq3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasq4.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasq5.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasq6.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasrt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slassq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasv2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slaswp.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slasy2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slatrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\slatrz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorg2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorgbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorghr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorgl2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorglq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorgqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorm2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sormbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sorml2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sormlq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sormqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sormr3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\sormrz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\srscl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\strevc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\strexc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\strsna.c
# End Source File
# Begin Source File

SOURCE=.\f2c\stzrzf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zdrscl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgebak.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgebal.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgebd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgebrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgecon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgeequ.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgeevx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgehd2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgehrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgelq2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgelqf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgelsy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgeqp3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgeqr2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgeqrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgerfs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgesdd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgesvx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgetf2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgetrf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zgetrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zhseqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlabrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlacgv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlacon.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlacp2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlacpy.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlacrm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zladiv.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlahqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlahrd.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlaic1.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlange.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlanhs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlantr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlaqge.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlaqp2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlaqps.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarcm.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarfb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarfg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarft.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarfx.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlartg.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarzb.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlarzt.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlascl.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlaset.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlassq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlaswp.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlatrs.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zlatrz.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zrot.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztrevc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztrexc.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztrsna.c
# End Source File
# Begin Source File

SOURCE=.\f2c\ztzrzf.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zung2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zungbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunghr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zungl2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunglq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zungqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunm2r.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunmbr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunml2.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunmlq.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunmqr.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunmr3.c
# End Source File
# Begin Source File

SOURCE=.\f2c\zunmrz.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
