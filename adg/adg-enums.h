/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_ENUMS_H__
#define __ADG_ENUMS_H__

#include <glib-object.h>


G_BEGIN_DECLS

typedef enum {
    ADG_LINE_STYLE_DRAW,
    ADG_LINE_STYLE_CENTER,
    ADG_LINE_STYLE_HIDDEN,
    ADG_LINE_STYLE_HATCH,
    ADG_LINE_STYLE_DIM,
    ADG_LINE_STYLE_LAST
} AdgLineStyleId;

typedef enum {
    ADG_FONT_STYLE_TEXT,
    ADG_FONT_STYLE_VALUE,
    ADG_FONT_STYLE_TOLERANCE,
    ADG_FONT_STYLE_NOTE,
    ADG_FONT_STYLE_LAST
} AdgFontStyleId;

typedef enum {
    ADG_ARROW_STYLE_ARROW,
    ADG_ARROW_STYLE_TRIANGLE,
    ADG_ARROW_STYLE_DOT,
    ADG_ARROW_STYLE_CIRCLE,
    ADG_ARROW_STYLE_BLOCK,
    ADG_ARROW_STYLE_SQUARE,
    ADG_ARROW_STYLE_TICK,
    ADG_ARROW_STYLE_LAST
} AdgArrowStyleId;

typedef enum {
    ADG_DIM_STYLE_ISO,
    ADG_DIM_STYLE_LAST
} AdgDimStyleId;

G_END_DECLS


#endif                          /* __ADG_ENUMS_H__ */
