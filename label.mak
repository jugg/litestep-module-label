# Microsoft Developer Studio Generated NMAKE File, Based on label.dsp
!IF "$(CFG)" == ""
CFG=label - Win32 Debug
!MESSAGE No configuration specified. Defaulting to label - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "label - Win32 Release" && "$(CFG)" != "label - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "label.mak" CFG="label - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "label - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "label - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "label - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\label.dll"


CLEAN :
	-@erase "$(INTDIR)\bangCommands.obj"
	-@erase "$(INTDIR)\DefaultTexture.obj"
	-@erase "$(INTDIR)\Font.obj"
	-@erase "$(INTDIR)\GradientTexture.obj"
	-@erase "$(INTDIR)\ImageTexture.obj"
	-@erase "$(INTDIR)\Label.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\label.dll"
	-@erase "$(OUTDIR)\label.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LABEL_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\label.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib lsapi.lib /nologo /dll /pdb:none /machine:I386 /def:".\exports.def" /out:"$(OUTDIR)\label.dll" /implib:"$(OUTDIR)\label.lib" 
DEF_FILE= \
	".\exports.def"
LINK32_OBJS= \
	"$(INTDIR)\bangCommands.obj" \
	"$(INTDIR)\DefaultTexture.obj" \
	"$(INTDIR)\Font.obj" \
	"$(INTDIR)\GradientTexture.obj" \
	"$(INTDIR)\ImageTexture.obj" \
	"$(INTDIR)\Label.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\utility.obj"

"$(OUTDIR)\label.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "label - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\label.dll"


CLEAN :
	-@erase "$(INTDIR)\bangCommands.obj"
	-@erase "$(INTDIR)\DefaultTexture.obj"
	-@erase "$(INTDIR)\Font.obj"
	-@erase "$(INTDIR)\GradientTexture.obj"
	-@erase "$(INTDIR)\ImageTexture.obj"
	-@erase "$(INTDIR)\Label.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\label.dll"
	-@erase "$(OUTDIR)\label.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LABEL_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\label.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib lsapi.lib /nologo /dll /pdb:none /debug /machine:I386 /def:".\exports.def" /out:"$(OUTDIR)\label.dll" /implib:"$(OUTDIR)\label.lib" 
DEF_FILE= \
	".\exports.def"
LINK32_OBJS= \
	"$(INTDIR)\bangCommands.obj" \
	"$(INTDIR)\DefaultTexture.obj" \
	"$(INTDIR)\Font.obj" \
	"$(INTDIR)\GradientTexture.obj" \
	"$(INTDIR)\ImageTexture.obj" \
	"$(INTDIR)\Label.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\utility.obj"

"$(OUTDIR)\label.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("label.dep")
!INCLUDE "label.dep"
!ELSE 
!MESSAGE Warning: cannot find "label.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "label - Win32 Release" || "$(CFG)" == "label - Win32 Debug"
SOURCE=.\bangCommands.cpp

"$(INTDIR)\bangCommands.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DefaultTexture.cpp

"$(INTDIR)\DefaultTexture.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Font.cpp

"$(INTDIR)\Font.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\GradientTexture.cpp

"$(INTDIR)\GradientTexture.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ImageTexture.cpp

"$(INTDIR)\ImageTexture.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Label.cpp

"$(INTDIR)\Label.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\utility.cpp

"$(INTDIR)\utility.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

