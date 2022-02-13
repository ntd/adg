; Helper script to handle ADG l10n files.
;
; ADG - Automatic Drawing Generation
; Copyright (C) 2015  Nicola Fontana <ntd at entidi.it>
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

!macro FileLocale LOCALE
  SetOutPath "$INSTDIR\share\locale\${LOCALE}\LC_MESSAGES"
  File /oname=adg.mo "${SRCDIR}/po/${LOCALE}.gmo"
  File "${PREFIX}/share/locale/${LOCALE}/LC_MESSAGES/glib20.mo"
  File "${PREFIX}/share/locale/${LOCALE}/LC_MESSAGES/gdk-pixbuf.mo"
  File "${PREFIX}/share/locale/${LOCALE}/LC_MESSAGES/gtk30.mo"
  File "${PREFIX}/share/locale/${LOCALE}/LC_MESSAGES/gtk30-properties.mo"
!macroend

!macro DeleteLocale LOCALE
  Delete "$INSTDIR\share\locale\${LOCALE}\LC_MESSAGES\adg.mo"
  Delete "$INSTDIR\share\locale\${LOCALE}\LC_MESSAGES\gtk30-properties.mo"
  Delete "$INSTDIR\share\locale\${LOCALE}\LC_MESSAGES\gtk30.mo"
  Delete "$INSTDIR\share\locale\${LOCALE}\LC_MESSAGES\gdk-pixbuf.mo"
  Delete "$INSTDIR\share\locale\${LOCALE}\LC_MESSAGES\glib20.mo"
  RMDir  "$INSTDIR\share\locale\${LOCALE}\LC_MESSAGES"
  RMDir  "$INSTDIR\share\locale\${LOCALE}"
!macroend
