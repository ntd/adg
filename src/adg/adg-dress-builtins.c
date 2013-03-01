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


/**
 * SECTION:adg-dress-builtins
 * @Section_Id:dress-builtins
 * @title: Built-in dresses
 * @short_description: A list of predefined dresses implemented
 *                     by the ADG canvas
 *
 * This is a collection of built-it dresses used internally by
 * the ADG library to provide some useful defaults.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-text-internal.h"

#include "adg-model.h"
#include "adg-trail.h"
#include "adg-marker.h"
#include "adg-dress.h"
#include "adg-color-style.h"
#include "adg-dash.h"
#include "adg-line-style.h"
#include "adg-fill-style.h"
#include "adg-dim-style.h"
#include "adg-table-style.h"
#include "adg-arrow.h"
#include "adg-ruled-fill.h"

#include "adg-dress-builtins.h"

#define MM  *2.83464566927


/**
 * ADG_DRESS_UNDEFINED:
 *
 * A value reperesenting an undefined #AdgDress.
 *
 * Since: 1.0
 **/

/**
 * ADG_DRESS_COLOR:
 *
 * The default builtin #AdgDress color.
 * This is a pass-through dress, that is it does not change
 * the cairo context when it is applied.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
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
 * ADG_DRESS_COLOR_BACKGROUND:
 *
 * The default builtin #AdgDress color to be used as the #AdgCanvas
 * background. The fallback style is a full opaque white.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_color_background(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "blue", 1., "green", 1.,
                                          "red", 1., NULL);

        dress = adg_dress_new("color-background", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_STROKE:
 *
 * The default builtin #AdgDress color for #AdgStroke entities.
 * The fallback style is a full opaque black.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
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
 * The fallback style is a somewhat full opaque blue.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_color_dimension(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "red", 0.,
                                          "green", 0.4,
                                          "blue", 0.6,
                                          NULL);

        dress = adg_dress_new("color-dimension", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_ANNOTATION:
 *
 * The builtin #AdgDress color used for rendering helper entities
 * such as #AdgToyText, #AdgTable and #AdgTitleBlock. The fallback
 * style is a full opaque greenish variant.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_color_annotation(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "red", 0.4,
                                          "green", 0.4,
                                          "blue", 0.2,
                                          NULL);

        dress = adg_dress_new("color-annotation", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_FILL:
 *
 * The builtin #AdgDress color used by default by #AdgFillStyle
 * based styles. The fallback style is a full opaque %0.25 gray.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_color_fill(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "red", 0.25,
                                          "green", 0.25,
                                          "blue", 0.25, NULL);

        dress = adg_dress_new("color-fill", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_AXIS:
 *
 * The default builtin #AdgDress color for stroking #ADG_DRESS_LINE_AXIS
 * lines. The fallback color is green.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_color_axis(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "red", 0.,
                                          "green", 0.75,
                                          "blue", 0.25, NULL);

        dress = adg_dress_new("color-axis", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_COLOR_HIDDEN:
 *
 * The default builtin #AdgDress color for stroking #ADG_DRESS_LINE_HIDDEN
 * lines. The fallback color is gray.
 *
 * This dress will be resolved to an #AdgColorStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_color_hidden(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_COLOR_STYLE,
                                          "red", 0.5,
                                          "green", 0.5,
                                          "blue", 0.5, NULL);

        dress = adg_dress_new("color-hidden", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE:
 *
 * The default builtin #AdgDress line.
 * This is a pass-through dress, that is it does not change
 * the cairo context when it is applied.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
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
 * ADG_DRESS_LINE_STROKE:
 *
 * The builtin #AdgDress line type to be used by default
 * for rendering #AdgStroke entities. The fallback style is
 * a line with #ADG_DRESS_COLOR_STROKE color and a thickness
 * of %1.5.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_line_stroke(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "color-dress", ADG_DRESS_COLOR_STROKE,
                                          "width", 1.5, NULL);

        dress = adg_dress_new("line-stroke", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_DIMENSION:
 *
 * The builtin #AdgDress line type used by default
 * for rendering base and extension lines of dimensions.
 * The fallback style is a line with a thickness of %0.5
 * and a pass-through color dress.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_line_dimension(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "width", 0.5, NULL);

        dress = adg_dress_new("line-dimension", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_FILL:
 *
 * The builtin #AdgDress line type used by #AdgFillStyle
 * based styles. The fallback style is a line with
 * #ADG_DRESS_COLOR_FILL color and a thickness of %0.5.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_line_fill(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "color-dress", ADG_DRESS_COLOR_FILL,
                                          "width", 0.5, NULL);

        dress = adg_dress_new("line-fill", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_GRID:
 *
 * The builtin #AdgDress line type used for rendering the grid
 * of #AdgTable entities, that is the frame of the cells.
 * The fallback style is a line with a thickness of %1 and a
 * pass-through color dress, rendered without antialiasing.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
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
 * The builtin #AdgDress line type used for rendering the frame
 * of #AdgTable entities, that is the frame around the whole table.
 * The fallback style is a line with a thickness of %2 and a
 * #ADG_DRESS_COLOR_ANNOTATION color dress, rendered without
 * antialiasing.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_line_frame(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                          "color-dress", ADG_DRESS_COLOR_ANNOTATION,
                                          "antialias", CAIRO_ANTIALIAS_NONE,
                                          "width", 2., NULL);

        dress = adg_dress_new("line-frame", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_AXIS:
 *
 * The builtin #AdgDress line type used for rendering axis and
 * centerlines.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_line_axis(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgDash *dash;
        AdgStyle *fallback;

        dash = adg_dash_new_with_dashes(4, 2 MM, 2 MM, 10 MM, 2 MM);
        fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                "dash", dash,
                                "color-dress", ADG_DRESS_COLOR_AXIS,
                                "width", 0.25 MM, NULL);
        adg_dash_destroy(dash);

        dress = adg_dress_new("line-axis", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_LINE_HIDDEN:
 *
 * The builtin #AdgDress line type used for rendering hidden
 * lines and edges.
 *
 * This dress will be resolved to an #AdgLineStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_line_hidden(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgDash *dash;
        AdgStyle *fallback;

        dash = adg_dash_new_with_dashes(2, 6 MM, 6 MM);
        fallback = g_object_new(ADG_TYPE_LINE_STYLE,
                                "dash", dash,
                                "color-dress", ADG_DRESS_COLOR_HIDDEN,
                                "width", 0.25 MM, NULL);
        adg_dash_destroy(dash);

        dress = adg_dress_new("line-hidden", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_FONT:
 *
 * The default builtin #AdgDress font. The fallback style is
 * a %Sans %14 font with a pass-through color dress.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_font(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                          "family", "Serif",
                                          "size", 14., NULL);

        dress = adg_dress_new("font", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_FONT_TEXT:
 *
 * The builtin #AdgDress font used by default for rendering
 * common text such as #AdgToyText or the value of #AdgTable
 * entities. The fallback style is %Sans %12 %bold with an
 * #ADG_DRESS_COLOR_ANNOTATION color dress.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_font_text(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                          "color-dress", ADG_DRESS_COLOR_ANNOTATION,
                                          "family", "Sans",
                                          "weight", CAIRO_FONT_WEIGHT_BOLD,
                                          "size", 12., NULL);

        dress = adg_dress_new("font-text", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_FONT_ANNOTATION:
 *
 * The builtin #AdgDress font used for rendering auxiliary text,
 * such as the titles on #AdgTable entities. The fallback style
 * is a %Sans %8 with an #ADG_DRESS_COLOR_ANNOTATION color dress.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_font_annotation(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                          "color-dress", ADG_DRESS_COLOR_ANNOTATION,
                                          "family", "Sans",
                                          "size", 8., NULL);

        dress = adg_dress_new("font-annotation", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_FONT_QUOTE_TEXT:
 *
 * The builtin #AdgDress font used for rendering regular text
 * on dimension entities, such as the nominal value and the
 * notes of a quote. The fallback style is %Sans %12 %bold with
 * a pass-through color dress.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_font_quote_text(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                          "family", "Sans",
                                          "weight", CAIRO_FONT_WEIGHT_BOLD,
                                          "size", 12., NULL);

        dress = adg_dress_new("font-quote-text", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_FONT_QUOTE_ANNOTATION:
 *
 * The builtin #AdgDress font used for rendering auxiliary text
 * on dimension entities, such as the min and max limits of a
 * quote. The fallback style is a %Sans %8 with a pass-through
 * color dress.
 *
 * This dress will be resolved to an #AdgFontStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_font_quote_annotation(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_BEST_FONT_STYLE,
                                          "family", "Sans",
                                          "size", 8., NULL);

        dress = adg_dress_new("font-quote-annotation", fallback);
        g_object_unref(fallback);
    }

    return dress;
}

/**
 * ADG_DRESS_DIMENSION:
 *
 * The default builtin #AdgDress for dimensions. The fallback
 * style is the default #AdgDimStyle implementation with
 * #AdgArrow markers (as returned by adg_arrow_new() on both sides.
 *
 * This dress will be resolved to an #AdgDimStyle instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_dimension(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgMarker *arrow = (AdgMarker *) adg_arrow_new();
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
 * The default builtin #AdgDress for filling.
 * This is a pass-through dress, that is it does not change
 * the cairo context when it is applied.
 *
 * This dress will be resolved to an #AdgFillStyle derived instance.
 *
 * Since: 1.0
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
 * The builtin dress used by default by #AdgHatch instances.
 * The fallback style is the default implementation of the
 * #AdgRuledFill instance.
 *
 * This dress will be resolved to an #AdgFillStyle derived instance.
 *
 * Since: 1.0
 **/
AdgDress
_adg_dress_fill_hatch(void)
{
    static AdgDress dress = 0;

    if (G_UNLIKELY(dress == 0)) {
        AdgStyle *fallback = g_object_new(ADG_TYPE_RULED_FILL,
                                          "line-dress", ADG_DRESS_LINE_FILL,
                                          NULL);

        dress = adg_dress_new_full("fill-hatch", fallback, ADG_TYPE_FILL_STYLE);
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
 *
 * Since: 1.0
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
