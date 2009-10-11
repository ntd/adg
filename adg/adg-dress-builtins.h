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


#ifndef __ADG_DRESS_BUILTINS_H__
#define __ADG_DRESS_BUILTINS_H__

#include <adg/adg-dress.h>


G_BEGIN_DECLS

#define ADG_DRESS_UNDEFINED             0
#define ADG_DRESS_COLOR                 (_adg_dress_color())
#define ADG_DRESS_COLOR_STROKE          (_adg_dress_color_stroke())
#define ADG_DRESS_COLOR_DIMENSION       (_adg_dress_color_dimension())
#define ADG_DRESS_COLOR_HATCH           (_adg_dress_color_hatch())
#define ADG_DRESS_LINE                  (_adg_dress_line())
#define ADG_DRESS_LINE_STROKE           (_adg_dress_line_stroke())
#define ADG_DRESS_LINE_DIMENSION        (_adg_dress_line_dimension())
#define ADG_DRESS_LINE_HATCH            (_adg_dress_line_hatch())
#define ADG_DRESS_TEXT                  (_adg_dress_text())
#define ADG_DRESS_TEXT_VALUE            (_adg_dress_text_value())
#define ADG_DRESS_TEXT_LIMIT            (_adg_dress_text_limit())
#define ADG_DRESS_DIMENSION             (_adg_dress_dimension())
#define ADG_DRESS_FILL                  (_adg_dress_fill())
#define ADG_DRESS_FILL_HATCH            (_adg_dress_fill_hatch())


AdgDress        _adg_dress_color                (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_stroke         (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_dimension      (void) G_GNUC_CONST;
AdgDress        _adg_dress_color_hatch          (void) G_GNUC_CONST;
AdgDress        _adg_dress_line                 (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_stroke          (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_dimension       (void) G_GNUC_CONST;
AdgDress        _adg_dress_line_hatch           (void) G_GNUC_CONST;
AdgDress        _adg_dress_text                 (void) G_GNUC_CONST;
AdgDress        _adg_dress_text_value           (void) G_GNUC_CONST;
AdgDress        _adg_dress_text_limit           (void) G_GNUC_CONST;
AdgDress        _adg_dress_dimension            (void) G_GNUC_CONST;
AdgDress        _adg_dress_fill                 (void) G_GNUC_CONST;
AdgDress        _adg_dress_fill_hatch           (void) G_GNUC_CONST;

G_END_DECLS


#endif /* __ADG_DRESS_BUILTINS_H__ */
