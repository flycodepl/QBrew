; QBrew Win32 Installer Script
; Requires NSIS 2.0 and above
; http://nsis.sf.net

!define PRODUCT "QBrew"
!define VERSION "0.4.1"
!define PUBLISHER "Usermode.org"
!define WEB_SITE "http://www.usermode.org"

!include "MUI.nsh"

;----------------------------------------
; Configuration

	; General
	Name "QBrew"
	OutFile "qbrew-install.exe"

	; Folder-selection page
	InstallDir "$PROGRAMFILES\${PRODUCT}"

	; Remember install folder
	InstallDirRegKey HKCU "Software\${PRODUCT}" ""

	; don't bother with verification or compression
	CRCCheck off
	SetCompress off

;----------------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Interface Configuration

	!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
	!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
	!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
	!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"

;----------------------------------------
; Modern UI Configuration

	!insertmacro MUI_PAGE_WELCOME
	;!insertmacro MUI_PAGE_LICENSE
	;!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	;!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\doc\handbook-index.html"
	!insertmacro MUI_PAGE_FINISH
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES

;------------------------------------------
; Languages

	!insertmacro MUI_LANGUAGE "English"

;------------------------------------------
;Installer Sections

; Required, installs program files
Section "QBrew Program" SecCore
SectionIn RO
	SetShellVarContext all ; modifies 'all users' area of start menu
	SetOverwrite on
	SetOutPath $INSTDIR

	; Files to get
	File /r "C:\qbrew\*"
	File "${NSISDIR}\Contrib\UIs\modern.exe"

	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\QBrew" DisplayName "QBrew (remove only)"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\QBrew" UninstallString "$INSTDIR\Uninstall.exe"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\usermode\QBrew" "InstallDir" $INSTDIR
	WriteRegStr HKEY_LOCAL_MACHINE "Software\usermode\QBrew" "Version" "${VERSION}"
	WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

	; Required, start menu shortcuts
Section -StartMenu
	SetShellVarContext all
	CreateDirectory	"$SMPROGRAMS\${PRODUCT}"
	IfFileExists "$SMPROGRAMS\${PRODUCT}" createIcons
	SetShellVarContext current
	CreateDirectory	"$SMPROGRAMS\${PRODUCT}"

	createIcons:
	CreateShortcut "$SMPROGRAMS\${PRODUCT}\QBrew.lnk" "$INSTDIR\qbrew.exe" ;"" $INSTDIR\qbrew.ico
	CreateShortCut "$SMPROGRAMS\${PRODUCT}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT}\QBrew Handbook.lnk" "$INSTDIR\doc\handbook-index.html"
	CreateShortcut "$DESKTOP\QBrew.lnk" "$INSTDIR\qbrew.exe" ;"" $INSTDIR\qbrew.ico
SectionEnd

;------------------------------------------
;Descriptions

	;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	;!insertmacro MUI_DESCRIPTION_TEXT ${SecCore} "QBrew program files. (Required for QBrew to run)"
	;!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Select to add a link to QBrew on your desktop."
	;!insertmacro MUI_FUNCTION_DESCRIPTION_END

;------------------------------------------
;Uninstaller Section
Section	"Uninstall"
	SetShellVarContext  all

	Delete "$INSTDIR\modern.exe"
	Delete "$INSTDIR\Uninstall.exe"
	RMDir /r "$SMPROGRAMS\${PRODUCT}"
	Delete "$DESKTOP\QBrew.lnk"
	RMDir /r "$INSTDIR\doc"
	Delete "$INSTDIR\LICENSE"
	Delete "$INSTDIR\README"
	Delete "$INSTDIR\qbrew.exe"
	Delete "$INSTDIR\qbrewdata"
	Delete "$INSTDIR\splash.png"
	Delete "$INSTDIR\qbrew.ico"
	Delete "$INSTDIR\mingwm10.dll"
	Delete "$INSTDIR\QtCore4.dll"
	Delete "$INSTDIR\QtGui4.dll"
	RMDir "$INSTDIR"

	DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\QBrew"
	DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\usermode\QBrew"
SectionEnd

;eof
