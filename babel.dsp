# Microsoft Developer Studio Project File - Name="babel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=babel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "babel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "babel.mak" CFG="babel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "babel - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "babel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "babel - Win32 Release"

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
# ADD CPP /nologo /G6 /Zp4 /W3 /O2 /I "include\babel" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "UNICODE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "babel - Win32 Debug"

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
# ADD CPP /nologo /G6 /Zp4 /W3 /Gm /ZI /Od /I "include\babel" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "UNICODE" /Fr /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "babel - Win32 Release"
# Name "babel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\algo.c
# End Source File
# Begin Source File

SOURCE=.\src\Arr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bit.c
# End Source File
# Begin Source File

SOURCE=.\src\cpg_unicode.c
# End Source File
# Begin Source File

SOURCE=.\src\cpg_unicode_iswide.inl
# End Source File
# Begin Source File

SOURCE=.\src\dir.c
# End Source File
# Begin Source File

SOURCE=.\src\enc.c
# End Source File
# Begin Source File

SOURCE=.\src\enc_sbc.c
# End Source File
# Begin Source File

SOURCE=.\src\enc_utf16.c
# End Source File
# Begin Source File

SOURCE=.\src\enc_utf8.c
# End Source File
# Begin Source File

SOURCE=.\src\err.c
# End Source File
# Begin Source File

SOURCE=.\src\file.c
# End Source File
# Begin Source File

SOURCE=.\src\fixmath.c
# End Source File
# Begin Source File

SOURCE=.\src\heap.c
# End Source File
# Begin Source File

SOURCE=.\src\log.c
# End Source File
# Begin Source File

SOURCE=.\src\map.c
# End Source File
# Begin Source File

SOURCE=.\src\mem.c
# End Source File
# Begin Source File

SOURCE=.\src\str.c
# End Source File
# Begin Source File

SOURCE=.\src\StrBuf.cpp
# End Source File
# Begin Source File

SOURCE=.\src\timedate.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\babel\algo.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\Arr.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\babel.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\bit.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\cpg_unicode.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\cpg_unicode_blocks.inl
# End Source File
# Begin Source File

SOURCE=.\include\babel\defs.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\dir.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\enc.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\enc_dbc.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\enc_sbc.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\enc_utf16.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\enc_utf8.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\file.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\fixmath.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\heap.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\log.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\map.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\mem.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\str.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\StrBuf.h
# End Source File
# Begin Source File

SOURCE=.\include\babel\timedate.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\babel.txt
# End Source File
# Begin Source File

SOURCE=.\bbconfig.h
# End Source File
# Begin Source File

SOURCE=.\Doxyfile
# End Source File
# Begin Source File

SOURCE=.\Makefile
# End Source File
# Begin Source File

SOURCE=.\unicode_blocks.pl
# End Source File
# Begin Source File

SOURCE=.\unicode_iswide.pl
# End Source File
# End Target
# End Project
