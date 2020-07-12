; Script for NSIS packaging.

!include MUI2.nsh
!include LogicLib.nsh

!define PRODUCTNAME            "Heimer"
!define APPNAME                "Heimer"
!define COMPANYNAME            "Juzzlin"
!define DESCRIPTION            "An application for mind map creation and management."
!define VERSIONMAJOR 1
!define VERSIONMINOR 18
!define VERSIONBUILD 0
!define HELPURL                "http://juzzlin.github.io/Heimer/"
!define UPDATEURL              "https://github.com/juzzlin/Heimer/releases"
!define ABOUTURL               "http://juzzlin.github.io/Heimer/"

;!define MUI_FILE               ""
!define MUI_BRANDINGTEXT       "Heimer"
;!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange.bmp"
!define MUI_ICON               "data\icons\heimer.ico"
!define MUI_UNICON             "data\icons\heimer.ico"
!define MUI_SPECIALBITMAP      "data\icons\heimer.ico"

!define MUI_WELCOMEPAGE  
!define MUI_LICENSEPAGE
!define MUI_DIRECTORYPAGE
!define MUI_ABORTWARNING
!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE
!define MUI_FINISHPAGE  

CRCCheck On
  
RequestExecutionLevel admin
 
InstallDir "$PROGRAMFILES\${PRODUCTNAME}"
 
Name "${PRODUCTNAME}"
Icon "data\icons\heimer.ico"
OutFile "heimer-${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}-install.exe"
 
LicenseData "COPYING" 

Page license
Page directory
Page instfiles

!insertmacro MUI_LANGUAGE "English"

!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
    messageBox mb_iconstop "Administrator rights required!"
    setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
    quit
${EndIf}
!macroend

Function .onInit
	SetShellVarContext all
	!insertmacro VerifyUserIsAdmin
FunctionEnd
 
Section "install"
	SetOutPath $INSTDIR

	File heimer.exe
	File data/icons/heimer.ico
	File CHANGELOG
	File COPYING
	File AUTHORS
	File README.md
	
	WriteUninstaller "$INSTDIR\uninstall.exe"
 
	CreateDirectory "$SMPROGRAMS\${PRODUCTNAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCTNAME}\${APPNAME}.lnk" \
	    "$INSTDIR\heimer.exe" "" "$INSTDIR\heimer.ico"
	CreateShortCut "$SMPROGRAMS\${PRODUCTNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" ""
 
	CreateShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\heimer.exe" "" "$INSTDIR\heimer.ico"
 
	# Registry information for add/remove programs
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "DisplayName" "${PRODUCTNAME} - ${DESCRIPTION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "DisplayIcon" "$\"$INSTDIR\logo.ico$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "Publisher" "$\"${COMPANYNAME}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "HelpLink" "$\"${HELPURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "DisplayVersion" "$\"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}$\""
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "VersionMajor" ${VERSIONMAJOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} \
	    ${PRODUCTNAME}" "VersionMinor" ${VERSIONMINOR}

	# There is no option for modifying or repairing the install
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${PRODUCTNAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${PRODUCTNAME}" "NoRepair" 1

SectionEnd
 
# Uninstaller
 
Function un.onInit

	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanently remove ${PRODUCTNAME}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
	
FunctionEnd
 
Section "uninstall"
 
	RmDir /r "$SMPROGRAMS\${PRODUCTNAME}"
	 
	Delete "$DESKTOP\${APPNAME}.lnk"
	 
	Delete   $INSTDIR\heimer.exe
	Delete   $INSTDIR\heimer.ico
	Delete   $INSTDIR\CHANGELOG
	Delete   $INSTDIR\COPYING
	Delete   $INSTDIR\AUTHORS
	Delete   $INSTDIR\README.md
 
	Delete   $INSTDIR\uninstall.exe
 
	RmDir $INSTDIR
 
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${PRODUCTNAME}"
	
SectionEnd













