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


#ifndef __ADG_FONT_STYLE_PRIVATE_H__
#define __ADG_FONT_STYLE_PRIVATE_H__


G_BEGIN_DECLS

typedef struct _AdgFontStylePrivate AdgFontStylePrivate;

struct _AdgFontStylePrivate {
    AdgDress                     color_dress;
    gchar                       *family;
    cairo_font_slant_t           slant;
    cairo_font_weight_t          weight;
    gdouble                      size;
    cairo_antialias_t            antialias;
    cairo_subpixel_order_t       subpixel_order;
    cairo_hint_style_t           hint_style;
    cairo_hint_metrics_t         hint_metrics;

    cairo_font_face_t           *face;
    cairo_scaled_font_t         *font;
};

G_END_DECLS


#endif /* __ADG_FONT_STYLE_PRIVATE_H__ */
