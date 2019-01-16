/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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


#if !defined(__ADG_H__)
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_ENUMS_H__
#define __ADG_ENUMS_H__


G_BEGIN_DECLS

/* This header file should follow this (inconsistent) style to cope
 * with glib-mkenums. At the time of writing, the regex used is
 * /\s*typedef\s+enum\s*({)?.../.
 *
 * Check the sources for up to date information and further details:
 * http://git.gnome.org/browse/glib/tree/gobject/glib-mkenums.in
 */

typedef enum {
    ADG_THREE_STATE_OFF,
    ADG_THREE_STATE_ON,
    ADG_THREE_STATE_UNKNOWN
} AdgThreeState;

typedef enum {
    ADG_TRANSFORM_NONE,
    ADG_TRANSFORM_BEFORE,
    ADG_TRANSFORM_AFTER,
    ADG_TRANSFORM_BEFORE_NORMALIZED,
    ADG_TRANSFORM_AFTER_NORMALIZED
} AdgTransformMode;

typedef enum {
    ADG_MIX_UNDEFINED,
    ADG_MIX_DISABLED,
    ADG_MIX_NONE,
    ADG_MIX_ANCESTORS,
    ADG_MIX_ANCESTORS_NORMALIZED,
    ADG_MIX_PARENT,
    ADG_MIX_PARENT_NORMALIZED
} AdgMix;

typedef enum {
    ADG_PROJECTION_SCHEME_UNDEFINED,
    ADG_PROJECTION_SCHEME_FIRST_ANGLE,
    ADG_PROJECTION_SCHEME_THIRD_ANGLE
} AdgProjectionScheme;

typedef enum {
    ADG_DRESS_UNDEFINED,
    ADG_DRESS_COLOR,
    ADG_DRESS_COLOR_BACKGROUND,
    ADG_DRESS_COLOR_STROKE,
    ADG_DRESS_COLOR_DIMENSION,
    ADG_DRESS_COLOR_ANNOTATION,
    ADG_DRESS_COLOR_FILL,
    ADG_DRESS_COLOR_AXIS,
    ADG_DRESS_COLOR_HIDDEN,
    ADG_DRESS_LINE,
    ADG_DRESS_LINE_STROKE,
    ADG_DRESS_LINE_DIMENSION,
    ADG_DRESS_LINE_FILL,
    ADG_DRESS_LINE_GRID,
    ADG_DRESS_LINE_FRAME,
    ADG_DRESS_LINE_AXIS,
    ADG_DRESS_LINE_HIDDEN,
    ADG_DRESS_FONT,
    ADG_DRESS_FONT_TEXT,
    ADG_DRESS_FONT_ANNOTATION,
    ADG_DRESS_FONT_QUOTE_TEXT,
    ADG_DRESS_FONT_QUOTE_ANNOTATION,
    ADG_DRESS_DIMENSION,
    ADG_DRESS_FILL,
    ADG_DRESS_FILL_HATCH,
    ADG_DRESS_TABLE
} AdgDress;

G_END_DECLS


#endif /* __ADG_ENUMS_H__ */
