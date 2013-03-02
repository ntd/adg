/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_DRESS_BUILTINS_H__
#define __ADG_DRESS_BUILTINS_H__


G_BEGIN_DECLS

#define ADG_DRESS_UNDEFINED             0
#define ADG_DRESS_COLOR                 (_adg_dress_color())
#define ADG_DRESS_COLOR_BACKGROUND      (_adg_dress_color_background())
#define ADG_DRESS_COLOR_STROKE          (_adg_dress_color_stroke())
#define ADG_DRESS_COLOR_DIMENSION       (_adg_dress_color_dimension())
#define ADG_DRESS_COLOR_ANNOTATION      (_adg_dress_color_annotation())
#define ADG_DRESS_COLOR_FILL            (_adg_dress_color_fill())
#define ADG_DRESS_COLOR_AXIS            (_adg_dress_color_axis())
#define ADG_DRESS_COLOR_HIDDEN          (_adg_dress_color_hidden())
#define ADG_DRESS_LINE                  (_adg_dress_line())
#define ADG_DRESS_LINE_STROKE           (_adg_dress_line_stroke())
#define ADG_DRESS_LINE_DIMENSION        (_adg_dress_line_dimension())
#define ADG_DRESS_LINE_FILL             (_adg_dress_line_fill())
#define ADG_DRESS_LINE_GRID             (_adg_dress_line_grid())
#define ADG_DRESS_LINE_FRAME            (_adg_dress_line_frame())
#define ADG_DRESS_LINE_AXIS             (_adg_dress_line_axis())
#define ADG_DRESS_LINE_HIDDEN           (_adg_dress_line_hidden())
#define ADG_DRESS_FONT                  (_adg_dress_font())
#define ADG_DRESS_FONT_TEXT             (_adg_dress_font_text())
#define ADG_DRESS_FONT_ANNOTATION       (_adg_dress_font_annotation())
#define ADG_DRESS_FONT_QUOTE_TEXT       (_adg_dress_font_quote_text())
#define ADG_DRESS_FONT_QUOTE_ANNOTATION (_adg_dress_font_quote_annotation())
#define ADG_DRESS_DIMENSION             (_adg_dress_dimension())
#define ADG_DRESS_FILL                  (_adg_dress_fill())
#define ADG_DRESS_FILL_HATCH            (_adg_dress_fill_hatch())
#define ADG_DRESS_TABLE                 (_adg_dress_table())


AdgDress        _adg_dress_color                (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_background     (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_stroke         (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_dimension      (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_fill           (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_axis           (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_hidden          (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_annotation     (void) G_GNUC_CONST;
AdgDress        _adg_dress_line                 (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_stroke          (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_dimension       (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_fill            (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_grid            (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_frame           (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_axis            (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_hidden          (void) G_GNUC_CONST;
AdgDress        _adg_dress_font                 (void) G_GNUC_CONST;
AdgDress        _adg_dress_font_text            (void) G_GNUC_CONST;
AdgDress        _adg_dress_font_annotation      (void) G_GNUC_CONST;
AdgDress        _adg_dress_font_quote_text      (void) G_GNUC_CONST;
AdgDress        _adg_dress_font_quote_annotation(void) G_GNUC_CONST;
AdgDress        _adg_dress_dimension            (void) G_GNUC_CONST;
AdgDress        _adg_dress_fill                 (void) G_GNUC_CONST;
AdgDress        _adg_dress_fill_hatch           (void) G_GNUC_CONST;
AdgDress        _adg_dress_table                (void) G_GNUC_CONST;

G_END_DECLS


#endif /* __ADG_DRESS_BUILTINS_H__ */
