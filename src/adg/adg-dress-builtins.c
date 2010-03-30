/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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


/**
 * SECTION:adg-dress-builtins
 * @Section_Id:dress-builtins
 * @title: Built-in dresses
 * @short_description: A list of predefined dresses implemented
 *                     by the ADG canvas
 *
 * This is a collection of built-it dresses used internally by
 * the ADG library to provide some useful defaults.
 **/


#include "adg-internal.h"
#include "adg-dress-builtins.h"
#include "adg-color-style.h"
#include "adg-line-style.h"
#include "adg-font-style.h"
#include "adg-dim-style.h"
#include "adg-arrow.h"
#include "adg-ruled-fill.h"
#include "adg-table-style.h"


/**
 * ADG_DRESS_UNDEFINED:
 *
 * A value reperesenting an undefined #AdgDress.
 **/

/**
 * ADG_DRESS_COLOR:
 *
 * The default builtin #AdgDress color. This is a transparent dress
 * without a fallback style.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 **/
AdgDress
_adg_dress_color(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        dress = adg_dress_new_full("color", NULL, ADG_TYPE_COLOR_STYLE);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_STROKE:
 *
 * The default builtin #AdgDress color for #AdgStroke entities.
 * The fallback style is the default implementation of #AdgColor
 * (that is %black).
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 **/
AdgDress
_adg_dress_color_stroke(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE, NULL);

        dress = adg_dress_new("color-stroke", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_DIMENSION:
 *
 * The builtin #AdgDress color used by default in #AdgDimStyle.
 * The fallback style is a %0.75 red.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 **/
AdgDress
_adg_dress_color_dimension(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "red", 0.667, NULL);

        dress = adg_dress_new("color-dimension", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_HATCH:
 *
 * The default builtin #AdgDress color for #AdgHatch entities.
 * The fallback style is a %0.5 blue.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 **/
AdgDress
_adg_dress_color_hatch(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "blue", 0.333, NULL);

        dress = adg_dress_new("color-hatch", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE:
 *
 * The default builtin #AdgDress line. This is a transparent dress
 * without a fallback style.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        dress = adg_dress_new_full("line", NULL, ADG_TYPE_LINE_STYLE);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_MEDIUM:
 *
 * The default generic builtin #AdgDress line type: it is used by
 * default for rendering #AdgStroke entities. The fallback style
 * is a default line with a thickness of %1.5.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_medium(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "width", 1.5, NULL);

        dress = adg_dress_new("line-medium", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_THIN:
 *
 * A generic builtin #AdgDress line type for thin lines.
 * The fallback style is a default line with a thickness of %1.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_thin(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "width", 1., NULL);

        dress = adg_dress_new("line-thin", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_THICK:
 *
 * A generic builtin #AdgDress line type for thick lines.
 * The fallback style is a default line with a thickness of %2.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_thick(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "width", 2., NULL);

        dress = adg_dress_new("line-thick", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_THINNER:
 *
 * A generic builtin #AdgDress line type for really thin lines:
 * it is used by default for rendering base and extension lines
 * of dimension entities. The fallback style is a default line
 * with a thickness of %0.75.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_thinner(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "width", 0.75, NULL);

        dress = adg_dress_new("line-thinner", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_THICKER:
 *
 * A generic builtin #AdgDress line type for really thick lines.
 * The fallback style is a default line with a thickness of %2.5.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_thicker(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "width", 2.5, NULL);

        dress = adg_dress_new("line-thicker", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_HATCH:
 *
 * The builtin #AdgDress line type used by the default #AdgRuledFill
 * style implementation. The fallback style is a default line with
 * a thickness of %1 and an #ADG_DRESS_COLOR_HATCH color dress.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_hatch(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgLineStyle *thin_style;
        AdgStyle *fallback;

        thin_style = (AdgLineStyle *) adg_dress_get_fallback(ADG_DRESS_LINE_THIN);
        fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                "width", adg_line_style_get_width(thin_style),
                                "color-dress", ADG_DRESS_COLOR_HATCH, NULL);

        dress = adg_dress_new("line-hatch", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_GRID:
 *
 * The builtin #AdgDress line type used for rendering grids of
 * #AdgTable entities. The fallback style is a default line with
 * a thickness of %1 and no antialiasing.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_grid(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "antialias", CAIRO_ANTIALIAS_NONE,
                                          "width", 1., NULL);

        dress = adg_dress_new("line-grid", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_FRAME:
 *
 * The builtin #AdgDress line type used for rendering frames of
 * #AdgTable entities. The fallback style is a default line with
 * a thickness of %2 and no antialiasing.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 **/
AdgDress
_adg_dress_line_frame(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "antialias", CAIRO_ANTIALIAS_NONE,
                                          "width", 2., NULL);

        dress = adg_dress_new("line-frame", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_TEXT:
 *
 * The default builtin #AdgDress font. The fallback style is
 * %Sans %14.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 **/
AdgDress
_adg_dress_text(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_FONT_STYLE,
                                          "family", "Serif",
                                          "size", 14., NULL);

        dress = adg_dress_new("text", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_TEXT_VALUE:
 *
 * The builtin #AdgDress font used to render the nominal value of a
 * dimension. The fallback style is %Sans %12 %bold.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 **/
AdgDress
_adg_dress_text_value(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_FONT_STYLE,
                                          "family", "Sans",
                                          "weight", CAIRO_FONT_WEIGHT_BOLD,
                                          "size", 12., NULL);

        dress = adg_dress_new("text-value", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_TEXT_LIMIT:
 *
 * The builtin #AdgDress font used to render the limits of either
 * the min and max values of a dimension. The fallback style
 * is a %Sans %8.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 **/
AdgDress
_adg_dress_text_limit(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_FONT_STYLE,
                                          "family", "Sans",
                                          "size", 8., NULL);

        dress = adg_dress_new("text-limit", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_DIMENSION:
 *
 * The default builtin #AdgDress for dimensioning. The fallback
 * style is the default #AdgDimStyle implementation with #AdgArrow
 * as markers on both sides.
 *
 * This dress will be resolved to an #AdgDimStyle instance.
 **/
AdgDress
_adg_dress_dimension(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgMarker *arrow = g_object_new(ADG_TYPE_ARROW, NULL);
        AdgStyle *fallback = g_object_new(ADG_TYPE_DIM_STYLE, NULL);

        adg_dim_style_set_marker1((AdgDimStyle *) fallback, arrow);
        adg_marker_set_pos(arrow, 1);
        adg_dim_style_set_marker2((AdgDimStyle *) fallback, arrow);

        dress = adg_dress_new("dimension", fallback);
        g_object_unref(fallback);
        g_object_unref(arrow);
    }

    return dress;
}

/**
 * ADG_DRESS_FILL:
 *
 * The default builtin #AdgDress for filling. This is a transparent
 * dress without a fallback style.
 *
 * This dress will be resolved to an #AdgFillStyle derived instance.
 **/
AdgDress
_adg_dress_fill(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        dress = adg_dress_new_full("fill", NULL, ADG_TYPE_FILL_STYLE);
    }

    return dress;
}

/**
 * ADG_DRESS_FILL_HATCH:
 *
 * The default builtin #AdgDress used by #AdgHatch instances.
 * The fallback style is the default implementation of the
 * #AdgRuledFill instance.
 *
 * This dress will be resolved to an #AdgFillStyle derived instance.
 **/
AdgDress
_adg_dress_fill_hatch(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_RULED_FILL,
                                          "line-dress", ADG_DRESS_LINE_HATCH,
                                          NULL);

        dress = adg_dress_new_full("fill-hatch", fallback,
                                   ADG_TYPE_FILL_STYLE);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_TABLE:
 *
 * The default builtin #AdgDress for tables. The fallback style
 * is the default implementation of the #AdgTableStyle instance.
 *
 * This dress will be resolved to an #AdgTableStyle derived instance.
 **/
AdgDress
_adg_dress_table(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_TABLE_STYLE, NULL);
        dress = adg_dress_new_full("table", fallback, ADG_TYPE_TABLE_STYLE);
        g_object_unref(fallback);
    }

    return dress;
}
