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

/**
 * AdgLineStyleId:
 * @ADG_LINE_STYLE_DRAW: normal drawing
 * @ADG_LINE_STYLE_CENTER: axis and center-lines
 * @ADG_LINE_STYLE_HIDDEN: covered entities
 * @ADG_LINE_STYLE_XATCH: xatches
 * @ADG_LINE_STYLE_DIM: extension and base lines of dimension entities
 * @ADG_LINE_STYLE_LAST: start of user-defined styles
 *
 * Builtin id to get some predefined #AdgLineStyle instances.
 */
typedef enum {
    ADG_LINE_STYLE_DRAW,
    ADG_LINE_STYLE_CENTER,
    ADG_LINE_STYLE_HIDDEN,
    ADG_LINE_STYLE_XATCH,
    ADG_LINE_STYLE_DIM,
    ADG_LINE_STYLE_LAST
} AdgLineStyleId;

/**
 * AdgFontStyleId:
 * @ADG_FONT_STYLE_TEXT: generic text style
 * @ADG_FONT_STYLE_QUOTE: text style for the quote number
 * @ADG_FONT_STYLE_TOLERANCE: text style for quote tolerances
 * @ADG_FONT_STYLE_NOTE: text style for additional notes appended to the quote
 * @ADG_FONT_STYLE_LAST: start of user defined font styles
 *
 * Builtin id to get some predefined #AdgFontStyle instances.
 */
typedef enum {
    ADG_FONT_STYLE_TEXT,
    ADG_FONT_STYLE_QUOTE,
    ADG_FONT_STYLE_TOLERANCE,
    ADG_FONT_STYLE_NOTE,
    ADG_FONT_STYLE_LAST
} AdgFontStyleId;

/**
 * AdgArrowStyleId:
 * @ADG_ARROW_STYLE_ARROW: the classic arrow to use in technical drawings
 * @ADG_ARROW_STYLE_TRIANGLE: same as above, but not filled
 * @ADG_ARROW_STYLE_DOT: a filled circle
 * @ADG_ARROW_STYLE_CIRCLE: an empty circle
 * @ADG_ARROW_STYLE_BLOCK: a filled square
 * @ADG_ARROW_STYLE_SQUARE: an empty square
 * @ADG_ARROW_STYLE_TICK: an architetural tick
 * @ADG_ARROW_STYLE_LAST: start of user-defined arrow styles
 *
 * Builtin id to get some predefined #AdgArrowStyle instances.
 */
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

/**
 * AdgDimStyleId:
 * @ADG_DIM_STYLE_ISO: the ISO standard
 * @ADG_DIM_STYLE_LAST: start of user-defined dimension styles
 *
 * Builtin id to get some predefined #AdgDimStyle instances.
 */
typedef enum {
    ADG_DIM_STYLE_ISO,
    ADG_DIM_STYLE_LAST
} AdgDimStyleId;

G_END_DECLS


#endif                          /* __ADG_ENUMS_H__ */
