# Microsoft Developer Studio Project File - Name="libf2c" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libf2c - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libf2c.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libf2c.mak" CFG="libf2c - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libf2c - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libf2c - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "libf2c - Win32 Release"

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
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libf2c - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "USE_CLOCK" /D "MSDOS" /D "NO_ONEXIT" /D "KR_headers" /YX /FD /GZ /c
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

# Name "libf2c - Win32 Release"
# Name "libf2c - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\abort_.c
# End Source File
# Begin Source File

SOURCE=.\backspac.c
# End Source File
# Begin Source File

SOURCE=.\c_abs.c
# End Source File
# Begin Source File

SOURCE=.\c_cos.c
# End Source File
# Begin Source File

SOURCE=.\c_div.c
# End Source File
# Begin Source File

SOURCE=.\c_exp.c
# End Source File
# Begin Source File

SOURCE=.\c_log.c
# End Source File
# Begin Source File

SOURCE=.\c_sin.c
# End Source File
# Begin Source File

SOURCE=.\c_sqrt.c
# End Source File
# Begin Source File

SOURCE=.\cabs.c
# End Source File
# Begin Source File

SOURCE=.\close.c
# End Source File
# Begin Source File

SOURCE=.\d_abs.c
# End Source File
# Begin Source File

SOURCE=.\d_acos.c
# End Source File
# Begin Source File

SOURCE=.\d_asin.c
# End Source File
# Begin Source File

SOURCE=.\d_atan.c
# End Source File
# Begin Source File

SOURCE=.\d_atn2.c
# End Source File
# Begin Source File

SOURCE=.\d_cnjg.c
# End Source File
# Begin Source File

SOURCE=.\d_cos.c
# End Source File
# Begin Source File

SOURCE=.\d_cosh.c
# End Source File
# Begin Source File

SOURCE=.\d_dim.c
# End Source File
# Begin Source File

SOURCE=.\d_exp.c
# End Source File
# Begin Source File

SOURCE=.\d_imag.c
# End Source File
# Begin Source File

SOURCE=.\d_int.c
# End Source File
# Begin Source File

SOURCE=.\d_lg10.c
# End Source File
# Begin Source File

SOURCE=.\d_log.c
# End Source File
# Begin Source File

SOURCE=.\d_mod.c
# End Source File
# Begin Source File

SOURCE=.\d_nint.c
# End Source File
# Begin Source File

SOURCE=.\d_prod.c
# End Source File
# Begin Source File

SOURCE=.\d_sign.c
# End Source File
# Begin Source File

SOURCE=.\d_sin.c
# End Source File
# Begin Source File

SOURCE=.\d_sinh.c
# End Source File
# Begin Source File

SOURCE=.\d_sqrt.c
# End Source File
# Begin Source File

SOURCE=.\d_tan.c
# End Source File
# Begin Source File

SOURCE=.\d_tanh.c
# End Source File
# Begin Source File

SOURCE=.\derf_.c
# End Source File
# Begin Source File

SOURCE=.\derfc_.c
# End Source File
# Begin Source File

SOURCE=.\dfe.c
# End Source File
# Begin Source File

SOURCE=.\dolio.c
# End Source File
# Begin Source File

SOURCE=.\dtime_.c
# End Source File
# Begin Source File

SOURCE=.\due.c
# End Source File
# Begin Source File

SOURCE=.\ef1asc_.c
# End Source File
# Begin Source File

SOURCE=.\ef1cmc_.c
# End Source File
# Begin Source File

SOURCE=.\endfile.c
# End Source File
# Begin Source File

SOURCE=.\erf_.c
# End Source File
# Begin Source File

SOURCE=.\erfc_.c
# End Source File
# Begin Source File

SOURCE=.\err.c
# End Source File
# Begin Source File

SOURCE=.\etime_.c
# End Source File
# Begin Source File

SOURCE=.\exit_.c
# End Source File
# Begin Source File

SOURCE=.\f77_aloc.c
# End Source File
# Begin Source File

SOURCE=.\f77vers.c
# End Source File
# Begin Source File

SOURCE=.\fmt.c
# End Source File
# Begin Source File

SOURCE=.\fmtlib.c
# End Source File
# Begin Source File

SOURCE=.\ftell_.c
# End Source File
# Begin Source File

SOURCE=.\getarg_.c
# End Source File
# Begin Source File

SOURCE=.\getenv_.c
# End Source File
# Begin Source File

SOURCE=.\h_abs.c
# End Source File
# Begin Source File

SOURCE=.\h_dim.c
# End Source File
# Begin Source File

SOURCE=.\h_dnnt.c
# End Source File
# Begin Source File

SOURCE=.\h_indx.c
# End Source File
# Begin Source File

SOURCE=.\h_len.c
# End Source File
# Begin Source File

SOURCE=.\h_mod.c
# End Source File
# Begin Source File

SOURCE=.\h_nint.c
# End Source File
# Begin Source File

SOURCE=.\h_sign.c
# End Source File
# Begin Source File

SOURCE=.\hl_ge.c
# End Source File
# Begin Source File

SOURCE=.\hl_gt.c
# End Source File
# Begin Source File

SOURCE=.\hl_le.c
# End Source File
# Begin Source File

SOURCE=.\hl_lt.c
# End Source File
# Begin Source File

SOURCE=.\i77vers.c
# End Source File
# Begin Source File

SOURCE=.\i_abs.c
# End Source File
# Begin Source File

SOURCE=.\i_dim.c
# End Source File
# Begin Source File

SOURCE=.\i_dnnt.c
# End Source File
# Begin Source File

SOURCE=.\i_indx.c
# End Source File
# Begin Source File

SOURCE=.\i_len.c
# End Source File
# Begin Source File

SOURCE=.\i_mod.c
# End Source File
# Begin Source File

SOURCE=.\i_nint.c
# End Source File
# Begin Source File

SOURCE=.\i_sign.c
# End Source File
# Begin Source File

SOURCE=.\iargc_.c
# End Source File
# Begin Source File

SOURCE=.\iio.c
# End Source File
# Begin Source File

SOURCE=.\ilnw.c
# End Source File
# Begin Source File

SOURCE=.\inquire.c
# End Source File
# Begin Source File

SOURCE=.\l_ge.c
# End Source File
# Begin Source File

SOURCE=.\l_gt.c
# End Source File
# Begin Source File

SOURCE=.\l_le.c
# End Source File
# Begin Source File

SOURCE=.\l_lt.c
# End Source File
# Begin Source File

SOURCE=.\lbitbits.c
# End Source File
# Begin Source File

SOURCE=.\lbitshft.c
# End Source File
# Begin Source File

SOURCE=.\lread.c
# End Source File
# Begin Source File

SOURCE=.\lwrite.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\open.c
# End Source File
# Begin Source File

SOURCE=.\pow_ci.c
# End Source File
# Begin Source File

SOURCE=.\pow_dd.c
# End Source File
# Begin Source File

SOURCE=.\pow_di.c
# End Source File
# Begin Source File

SOURCE=.\pow_hh.c
# End Source File
# Begin Source File

SOURCE=.\pow_ii.c
# End Source File
# Begin Source File

SOURCE=.\pow_ri.c
# End Source File
# Begin Source File

SOURCE=.\pow_zi.c
# End Source File
# Begin Source File

SOURCE=.\pow_zz.c
# End Source File
# Begin Source File

SOURCE=.\r_abs.c
# End Source File
# Begin Source File

SOURCE=.\r_acos.c
# End Source File
# Begin Source File

SOURCE=.\r_asin.c
# End Source File
# Begin Source File

SOURCE=.\r_atan.c
# End Source File
# Begin Source File

SOURCE=.\r_atn2.c
# End Source File
# Begin Source File

SOURCE=.\r_cnjg.c
# End Source File
# Begin Source File

SOURCE=.\r_cos.c
# End Source File
# Begin Source File

SOURCE=.\r_cosh.c
# End Source File
# Begin Source File

SOURCE=.\r_dim.c
# End Source File
# Begin Source File

SOURCE=.\r_exp.c
# End Source File
# Begin Source File

SOURCE=.\r_imag.c
# End Source File
# Begin Source File

SOURCE=.\r_int.c
# End Source File
# Begin Source File

SOURCE=.\r_lg10.c
# End Source File
# Begin Source File

SOURCE=.\r_log.c
# End Source File
# Begin Source File

SOURCE=.\r_mod.c
# End Source File
# Begin Source File

SOURCE=.\r_nint.c
# End Source File
# Begin Source File

SOURCE=.\r_sign.c
# End Source File
# Begin Source File

SOURCE=.\r_sin.c
# End Source File
# Begin Source File

SOURCE=.\r_sinh.c
# End Source File
# Begin Source File

SOURCE=.\r_sqrt.c
# End Source File
# Begin Source File

SOURCE=.\r_tan.c
# End Source File
# Begin Source File

SOURCE=.\r_tanh.c
# End Source File
# Begin Source File

SOURCE=.\rdfmt.c
# End Source File
# Begin Source File

SOURCE=.\rewind.c
# End Source File
# Begin Source File

SOURCE=.\rsfe.c
# End Source File
# Begin Source File

SOURCE=.\rsli.c
# End Source File
# Begin Source File

SOURCE=.\rsne.c
# End Source File
# Begin Source File

SOURCE=.\s_cat.c
# End Source File
# Begin Source File

SOURCE=.\s_cmp.c
# End Source File
# Begin Source File

SOURCE=.\s_copy.c
# End Source File
# Begin Source File

SOURCE=.\s_paus.c
# End Source File
# Begin Source File

SOURCE=.\s_rnge.c
# End Source File
# Begin Source File

SOURCE=.\s_stop.c
# End Source File
# Begin Source File

SOURCE=.\sfe.c
# End Source File
# Begin Source File

SOURCE=.\sig_die.c
# End Source File
# Begin Source File

SOURCE=.\signal_.c
# End Source File
# Begin Source File

SOURCE=.\signbit.c
# End Source File
# Begin Source File

SOURCE=.\sue.c
# End Source File
# Begin Source File

SOURCE=.\system_.c
# End Source File
# Begin Source File

SOURCE=.\typesize.c
# End Source File
# Begin Source File

SOURCE=.\uio.c
# End Source File
# Begin Source File

SOURCE=.\uninit.c
# End Source File
# Begin Source File

SOURCE=.\util.c
# End Source File
# Begin Source File

SOURCE=.\wref.c
# End Source File
# Begin Source File

SOURCE=.\wrtfmt.c
# End Source File
# Begin Source File

SOURCE=.\wsfe.c
# End Source File
# Begin Source File

SOURCE=.\wsle.c
# End Source File
# Begin Source File

SOURCE=.\wsne.c
# End Source File
# Begin Source File

SOURCE=.\xwsne.c
# End Source File
# Begin Source File

SOURCE=.\z_abs.c
# End Source File
# Begin Source File

SOURCE=.\z_cos.c
# End Source File
# Begin Source File

SOURCE=.\z_div.c
# End Source File
# Begin Source File

SOURCE=.\z_exp.c
# End Source File
# Begin Source File

SOURCE=.\z_log.c
# End Source File
# Begin Source File

SOURCE=.\z_sin.c
# End Source File
# Begin Source File

SOURCE=.\z_sqrt.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\arith.h
# End Source File
# Begin Source File

SOURCE=.\f2c.h
# End Source File
# Begin Source File

SOURCE=.\fio.h
# End Source File
# Begin Source File

SOURCE=.\fmt.h
# End Source File
# Begin Source File

SOURCE=.\fp.h
# End Source File
# Begin Source File

SOURCE=.\lio.h
# End Source File
# Begin Source File

SOURCE=.\math.h
# End Source File
# Begin Source File

SOURCE=.\rawio.h
# End Source File
# Begin Source File

SOURCE=.\signal1.h
# End Source File
# Begin Source File

SOURCE=.\sysdep1.h
# End Source File
# End Group
# End Target
# End Project
