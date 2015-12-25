# Microsoft Developer Studio Project File - Name="MultiDx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MultiDx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MultiDx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MultiDx.mak" CFG="MultiDx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MultiDx - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MultiDx - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MultiDx - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3d9.lib d3dx9.lib dxerr9.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MultiDx - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3d9.lib d3dx9.lib dxerr9.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MultiDx - Win32 Release"
# Name "MultiDx - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\MultiDx.ico
# End Source File
# Begin Source File

SOURCE=.\MultiDx.rc
# End Source File
# Begin Source File

SOURCE=.\res\MultiDx.rc2
# End Source File
# Begin Source File

SOURCE=.\res\MultiDxDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "MFC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MultiDx.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiDx.h
# End Source File
# Begin Source File

SOURCE=.\MultiDxDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiDxDoc.h
# End Source File
# Begin Source File

SOURCE=.\MultiDxView.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiDxView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "rendering"

# PROP Default_Filter ""
# Begin Group "dx9"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rendering\dx9\RendererDX9.cpp
# End Source File
# Begin Source File

SOURCE=.\rendering\dx9\RendererDX9.h
# End Source File
# Begin Source File

SOURCE=.\rendering\dx9\RendererDX9.inl
# End Source File
# Begin Source File

SOURCE=.\rendering\dx9\SwapChainDX9.cpp
# End Source File
# Begin Source File

SOURCE=.\rendering\dx9\SwapChainDX9.h
# End Source File
# Begin Source File

SOURCE=.\rendering\dx9\SwapChainDX9.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\rendering\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\rendering\Renderer.h
# End Source File
# Begin Source File

SOURCE=.\rendering\Renderer.inl
# End Source File
# Begin Source File

SOURCE=.\rendering\SwapChain.cpp
# End Source File
# Begin Source File

SOURCE=.\rendering\SwapChain.h
# End Source File
# Begin Source File

SOURCE=.\rendering\SwapChain.inl
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "global"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\global\BasicTypes.h
# End Source File
# Begin Source File

SOURCE=.\global\Defines.h
# End Source File
# Begin Source File

SOURCE=.\global\StlIncludes.h
# End Source File
# End Group
# Begin Group "maths"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\maths\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\maths\Quaternion.h
# End Source File
# Begin Source File

SOURCE=.\maths\Quaternion.inl
# End Source File
# Begin Source File

SOURCE=.\maths\VectMat.cpp
# End Source File
# Begin Source File

SOURCE=.\maths\VectMat.h
# End Source File
# Begin Source File

SOURCE=.\maths\VectMat.inl
# End Source File
# End Group
# Begin Group "debugging"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\debugging\MyAssert.cpp
# End Source File
# Begin Source File

SOURCE=.\debugging\MyAssert.h
# End Source File
# End Group
# Begin Group "memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\memory\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=.\memory\SmartPtr.inl
# End Source File
# End Group
# Begin Group "containers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\containers\StdString.h
# End Source File
# End Group
# End Group
# End Target
# End Project
