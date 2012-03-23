/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

/*
 * This header provides:
 *
 * - an internal proxy type (AdgBestFontStyle) that resolves to
 *   AdgFontStyle or AdgPangoStyle, depending on wheter or not
 *   the pango support has been compiled;
 * - an internal proxy type (AdgBestText) that resolves to
 *   AdgToyText or AdgText, depending on wheter or not  the pango
 *   support has been compiled;
 */

#ifndef __ADG_TEXT_INTERNAL_H__
#define __ADG_TEXT_INTERNAL_H__


#include "adg-textual.h"
#include "adg-entity.h"
#include "adg-toy-text.h"
#include "adg-style.h"
#include "adg-font-style.h"


#ifdef PANGO_ENABLED

#include <pango/pango.h>
#include "adg-text.h"
#include "adg-pango-style.h"

#define ADG_TYPE_BEST_TEXT              ADG_TYPE_TEXT
#define ADG_TYPE_BEST_FONT_STYLE        ADG_TYPE_PANGO_STYLE

#else /* ! PANGO_ENABLED */

#define ADG_TYPE_BEST_TEXT              ADG_TYPE_TOY_TEXT
#define ADG_TYPE_BEST_FONT_STYLE        ADG_TYPE_FONT_STYLE

#endif /* PANGO_ENABLED */


#endif /* __ADG_TEXT_INTERNAL_H__ */
