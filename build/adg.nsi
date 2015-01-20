; Installer script for ADG on Windows (both 32 and 64 bits)
; Check NSIS website for details: http://nsis.sourceforge.net/
;
; ADG - Automatic Drawing Generation
; Copyright (C) 2010-2015  Nicola Fontana <ntd at entidi.it>
;
; This file is free software; you can redistribute it and/or
; modify it under the terms of the GNU Lesser General Public
; License as published by the Free Software Foundation; either
; version 2 of the License, or (at your option) any later version.
;
; This library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; Lesser General Public License for more details.
;
; You should have received a copy of the GNU Lesser General Public
; License along with this library; if not, write to the
; Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
; Boston, MA  02110-1301, USA.


; ---------------------------------------------------------------
; 1. Header file

!include "build/Custom.nsh"
!include "MUI2.nsh"
!include "${SRCDIR}/build/EnvVarUpdate.nsh"

Name "${PACKAGE_NAME} ${PACKAGE_VERSION}"
OutFile "${PACKAGE_NAME}-${PACKAGE_VERSION}-win${PACKAGE_ARCH}.exe"
InstallDir "$PROGRAMFILES${PACKAGE_ARCH}\${PACKAGE_NAME}"

SetCompressor /SOLID lzma
ShowInstDetails show
ShowUninstDetails show
SetDateSave on
RequestExecutionLevel highest


; ---------------------------------------------------------------
; 2. Interface configuration

!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC

!define MUI_ICON "${SRCDIR}/build/adg.ico"
!define MUI_UNICON "${SRCDIR}/build/adg.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${SRCDIR}/build/adg-header.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${SRCDIR}/build/adg-header.bmp"

!define MUI_WELCOMEFINISHPAGE_BITMAP "${SRCDIR}/build/adg-welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${SRCDIR}/build/adg-welcome.bmp"


; ---------------------------------------------------------------
; 3. Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${SRCDIR}/COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


; ---------------------------------------------------------------
; 4. Language files

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Italian"


; ---------------------------------------------------------------
; 5. Reserve files


; ---------------------------------------------------------------
; 6. Sections

; I. Introduction

!define DLLDIR		"${USRDIR}/bin"

Section $(TITLE_SecBase) SecBase
  SectionIn RO

  SetOutPath "$INSTDIR"
  File /oname=adg-demo.exe  "${BUILDDIR}/demo/.libs/adg-demo-uninstalled.exe"
  File /oname=cpml-demo.exe "${BUILDDIR}/demo/.libs/cpml-demo-uninstalled.exe"

  SetOutPath "$INSTDIR\share\adg"
  File "${BUILDDIR}/demo/adg-demo.ui"
  File "${BUILDDIR}/demo/cpml-demo.ui"
  File "${SRCDIR}/demo/adg-16.png"
  File "${SRCDIR}/demo/adg-32.png"
  File "${SRCDIR}/demo/adg-48.png"
  File "${SRCDIR}/demo/adg-64.png"
  File "${SRCDIR}/demo/adg-128.png"

  SetOutPath "$INSTDIR\bin"
  File "${DLLDIR}/libwinpthread-*.dll"
  File "${DLLDIR}/libgcc_s_*-1.dll"
  File "${DLLDIR}/zlib1.dll"
  File "${DLLDIR}/libbz2-*.dll"
  File "${DLLDIR}/libexpat-*.dll"
  File "${DLLDIR}/libintl-*.dll"
  File "${DLLDIR}/iconv.dll"
  File "${DLLDIR}/libffi-*.dll"
  File "${DLLDIR}/libpng??-*.dll"
  File "${DLLDIR}/libglib-2.0-*.dll"
  File "${DLLDIR}/libgthread-2.0-*.dll"
  File "${DLLDIR}/libgmodule-2.0-*.dll"
  File "${DLLDIR}/libgobject-2.0-*.dll"
  File "${DLLDIR}/libgio-2.0-*.dll"
  File "${DLLDIR}/libfreetype-*.dll"
  File "${DLLDIR}/libpixman-1-*.dll"
  File "${DLLDIR}/libcairo-*.dll"
  File "${DLLDIR}/libcairo-gobject-*.dll"
  File "${DLLDIR}/libpango-1.0-*.dll"
  File "${DLLDIR}/libpangocairo-1.0-*.dll"
  File "${DLLDIR}/libpangowin32-1.0-*.dll"
  File "${DLLDIR}/libatk-1.0-*.dll"
  File "${DLLDIR}/libgdk_pixbuf-2.0-*.dll"
  File "${DLLDIR}/libgdk-3-*.dll"
  File "${DLLDIR}/libgtk-3-*.dll"
  File "${BUILDDIR}/src/cpml/.libs/libcpml-1-*.dll"
  File "${BUILDDIR}/src/adg/.libs/libadg-1-*.dll"

  SetOutPath "$INSTDIR\share\glib-2.0\schemas"
  File "${SRCDIR}/build/gschemas.compiled"

  CreateDirectory "$SMPROGRAMS\ADG Canvas"
  CreateShortcut  "$SMPROGRAMS\ADG Canvas\ADG demonstration program.lnk" '"$INSTDIR\adg-demo.exe"'
  CreateShortcut  "$SMPROGRAMS\ADG Canvas\CPML showcase.lnk" '"$INSTDIR\cpml-demo.exe"'

  WriteUninstaller "$INSTDIR\uninstall.exe"
  ${EnvVarUpdate} $0 "PATH" "A" "HKCU" "$INSTDIR\bin"
SectionEnd

Function .onInstSuccess
  ; The uninstall shortcut is installed inside onInstSuccess so any subsequent
  ; section (e.g. SecHTML) can install other shortcuts *before* this one.
  CreateShortcut  "$SMPROGRAMS\ADG Canvas\Uninstall ADG Canvas.lnk" "$INSTDIR\uninstall.exe"
FunctionEnd

; II. Language support

!include "${SRCDIR}/build/Locale.nsh"

Section $(TITLE_SecLanguages) SecLanguages
  ; TODO: this should iterate over po/LINGUAS
  !insertmacro FileLocale "it"
SectionEnd

; III. Documentation

SectionGroup /e $(TITLE_SecDocumentation) SecDocumentation

Section $(TITLE_SecHTML) SecHTML
  SetOutPath "$INSTDIR\share\gtk-doc\html\cpml"
  File /r "${BUILDDIR}/_host/docs/cpml/html/*.*"

  SetOutPath "$INSTDIR\share\gtk-doc\html\adg"
  File /r "${BUILDDIR}/_host/docs/adg/html/*.*"

  CreateShortcut "$SMPROGRAMS\ADG Canvas\CPML HTML manual.lnk" '"$INSTDIR\share\gtk-doc\html\cpml\index.html"'
  CreateShortcut "$SMPROGRAMS\ADG Canvas\ADG HTML manual.lnk" '"$INSTDIR\share\gtk-doc\html\adg\index.html"'
SectionEnd

Section /o $(TITLE_SecPDF) SecPDF
  SetOutPath "$INSTDIR\share\gtk-doc"
  File /r "${BUILDDIR}/_host/docs/cpml/cpml.pdf"
  File /r "${BUILDDIR}/_host/docs/adg/adg.pdf"

  CreateShortcut "$SMPROGRAMS\ADG Canvas\CPML PDF manual.lnk" '"$INSTDIR\share\gtk-doc\cpml.pdf"'
  CreateShortcut "$SMPROGRAMS\ADG Canvas\ADG PDF manual.lnk" '"$INSTDIR\share\gtk-doc\adg.pdf"'
SectionEnd

SectionGroupEnd

; IV. Uninstaller

Section "Uninstall"
  Delete "$SMPROGRAMS\ADG Canvas\ADG Demonstration program.lnk"
  Delete "$SMPROGRAMS\ADG Canvas\CPML showcase.lnk"
  Delete "$SMPROGRAMS\ADG Canvas\CPML HTML manual.lnk"
  Delete "$SMPROGRAMS\ADG Canvas\CPML PDF manual.lnk"
  Delete "$SMPROGRAMS\ADG Canvas\ADG HTML manual.lnk"
  Delete "$SMPROGRAMS\ADG Canvas\ADG PDF manual.lnk"
  Delete "$SMPROGRAMS\ADG Canvas\Uninstall ADG Canvas.lnk"
  RMDir  "$SMPROGRAMS\ADG Canvas"

  Delete "$INSTDIR\bin\libgtk-3-*.dll"
  Delete "$INSTDIR\bin\libgdk-3-*.dll"
  Delete "$INSTDIR\bin\libgdk_pixbuf-2.0-*.dll"
  Delete "$INSTDIR\bin\libatk-1.0-*.dll"
  Delete "$INSTDIR\bin\libpangowin32-1.0-*.dll"
  Delete "$INSTDIR\bin\libpangocairo-1.0-*.dll"
  Delete "$INSTDIR\bin\libpango-1.0-*.dll"
  Delete "$INSTDIR\bin\libcairo-gobject-*.dll"
  Delete "$INSTDIR\bin\libcairo-*.dll"
  Delete "$INSTDIR\bin\libpixman-1-*.dll"
  Delete "$INSTDIR\bin\libfreetype-*.dll"
  Delete "$INSTDIR\bin\libgio-2.0-*.dll"
  Delete "$INSTDIR\bin\libgobject-2.0-*.dll"
  Delete "$INSTDIR\bin\libgmodule-2.0-*.dll"
  Delete "$INSTDIR\bin\libgthread-2.0-*.dll"
  Delete "$INSTDIR\bin\libglib-2.0-*.dll"
  Delete "$INSTDIR\bin\libpng??-*.dll"
  Delete "$INSTDIR\bin\libffi-*.dll"
  Delete "$INSTDIR\bin\iconv.dll"
  Delete "$INSTDIR\bin\libintl-*.dll"
  Delete "$INSTDIR\bin\libexpat-*.dll"
  Delete "$INSTDIR\bin\libbz2-*.dll"
  Delete "$INSTDIR\bin\zlib1.dll"
  Delete "$INSTDIR\bin\libgcc_s_*-1.dll"
  Delete "$INSTDIR\bin\libwinpthread-*.dll"
  Delete "$INSTDIR\bin\libcpml-1-*.dll"
  Delete "$INSTDIR\bin\libadg-1-*.dll"
  RMDir  "$INSTDIR\bin"

  Delete "$INSTDIR\share\adg\adg-demo.ui"
  Delete "$INSTDIR\share\adg\cpml-demo.ui"
  Delete "$INSTDIR\share\adg\adg-128.png"
  Delete "$INSTDIR\share\adg\adg-64.png"
  Delete "$INSTDIR\share\adg\adg-48.png"
  Delete "$INSTDIR\share\adg\adg-32.png"
  Delete "$INSTDIR\share\adg\adg-16.png"
  RMDir  "$INSTDIR\share\adg"

  Delete "$INSTDIR\share\glib-2.0\schemas\gschemas.compiled"
  RMDir  "$INSTDIR\share\glib-2.0\schemas"
  RMDir  "$INSTDIR\share\glib-2.0"

  ; TODO: this should iterate over po/LINGUAS
  !insertmacro DeleteLocale "it"

  RMDir  "$INSTDIR\share\locale"
  RMDir /r "$INSTDIR\share\gtk-doc\html\cpml"
  RMDir /r "$INSTDIR\share\gtk-doc\html\adg"
  RMDir  "$INSTDIR\share\gtk-doc\html"

  Delete "$INSTDIR\share\gtk-doc\adg.pdf"
  Delete "$INSTDIR\share\gtk-doc\cpml.pdf"

  RMDir  "$INSTDIR\share\gtk-doc"
  RMDir  "$INSTDIR\share"

  Delete "$INSTDIR\adg-demo.exe"
  Delete "$INSTDIR\cpml-demo.exe"
  Delete "$INSTDIR\uninstall.exe"
  RMDir  "$INSTDIR"

  ${un.EnvVarUpdate} $0 "PATH" "R" "HKCU" "$INSTDIR\bin"
SectionEnd


; ---------------------------------------------------------------
; 7. Internationalization

!include "${SRCDIR}/build/Translations.nsh"

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecBase} $(DESC_SecBase)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecLanguages} $(DESC_SecLanguages)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDocumentation} $(DESC_SecDocumentation)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecHTML} $(DESC_SecHTML)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPDF} $(DESC_SecPDF)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
