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


#include "test-internal.h"

/* ADG_TYPE_BEST_FONT_STYLE must be defined in order to check
 * style resolution from font dresses, but its definition is
 * internal to src/adg (adg-text-internal.h). Duplicating its
 * definition here for testing convenience.
 */
#include "config.h"

#ifdef PANGO_ENABLED
#define ADG_TYPE_BEST_FONT_STYLE        ADG_TYPE_PANGO_STYLE
#else
#define ADG_TYPE_BEST_FONT_STYLE        ADG_TYPE_FONT_STYLE
#endif


static void
_adg_test_generic(void)
{
    g_assert(adg_dress_get_name(ADG_DRESS_UNDEFINED) == NULL);

    g_assert(adg_dress_get_ancestor_type(ADG_DRESS_UNDEFINED) == 0);

    g_assert(adg_dress_are_related(ADG_DRESS_UNDEFINED, ADG_DRESS_UNDEFINED) == FALSE);

    g_assert_cmpint(adg_dress_from_name("unexistent-dress"), ==, ADG_DRESS_UNDEFINED);
    g_assert_cmpint(adg_dress_from_name(NULL), ==, ADG_DRESS_UNDEFINED);
}

static void
_adg_test_set(void)
{
    AdgDress dress;

    /* Must fail because src is not related to color dress */
    dress = ADG_DRESS_COLOR;
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_STROKE) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE) == FALSE);

    /* Must fail because src is not related to line dress */
    dress = ADG_DRESS_LINE;
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_DIMENSION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_ANNOTATION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FILL) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE) == FALSE);

    /* Must fail because src is not related to font dress */
    dress = ADG_DRESS_FONT;
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_AXIS) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_FILL) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE) == FALSE);

    /* Must fail because src is not related to dimension dress */
    dress = ADG_DRESS_DIMENSION;
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_HIDDEN) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_DIMENSION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FILL) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE) == FALSE);

    /* Must fail because src is not related to fill dress */
    dress = ADG_DRESS_FILL;
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_FILL) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_FILL) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_ANNOTATION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE) == FALSE);

    /* Must fail because src is not related to table dress */
    dress = ADG_DRESS_TABLE;
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_BACKGROUND) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_FRAME) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH) == FALSE);
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION) == FALSE);

    /* Check for color dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_BACKGROUND));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_STROKE));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_DIMENSION));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_ANNOTATION));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_FILL));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_AXIS));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_HIDDEN));

    /* Check for line dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE));
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_STROKE));
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_DIMENSION));
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_FILL));
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_GRID));
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_FRAME));
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_AXIS));
    g_assert(adg_dress_set(&dress, ADG_DRESS_LINE_HIDDEN));

    /* Check for font dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT));
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_TEXT));
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_ANNOTATION));
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT));
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_ANNOTATION));

    /* Check for dimension dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION));

    /* Check for fill dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_FILL));
    g_assert(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH));

    /* Check for table dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE));

    /* Assignment that does not change the dress must return FALSE */
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT));
    g_assert(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT) == FALSE);
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_STROKE));
    g_assert(adg_dress_set(&dress, ADG_DRESS_COLOR_STROKE) == FALSE);
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION));
    g_assert(adg_dress_set(&dress, ADG_DRESS_DIMENSION) == FALSE);
    dress = ADG_DRESS_UNDEFINED;
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE));
    g_assert(adg_dress_set(&dress, ADG_DRESS_TABLE) == FALSE);
}

static void
_adg_test_ancestor(void)
{
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR),                 ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR),                 ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR_BACKGROUND),      ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR_STROKE),          ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR_DIMENSION),       ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR_ANNOTATION),      ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR_FILL),            ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR_AXIS),            ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_COLOR_HIDDEN),          ==, ADG_TYPE_COLOR_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE),                  ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE_STROKE),           ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE_DIMENSION),        ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE_FILL),             ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE_GRID),             ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE_FRAME),            ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE_AXIS),             ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_LINE_HIDDEN),           ==, ADG_TYPE_LINE_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_FONT),                  ==, ADG_TYPE_BEST_FONT_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_FONT_TEXT),             ==, ADG_TYPE_BEST_FONT_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_FONT_ANNOTATION),       ==, ADG_TYPE_BEST_FONT_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_FONT_QUOTE_TEXT),       ==, ADG_TYPE_BEST_FONT_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_FONT_QUOTE_ANNOTATION), ==, ADG_TYPE_BEST_FONT_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_DIMENSION),             ==, ADG_TYPE_DIM_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_FILL),                  ==, ADG_TYPE_FILL_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_FILL_HATCH),            ==, ADG_TYPE_FILL_STYLE);
    g_assert_cmpint(adg_dress_get_ancestor_type(ADG_DRESS_TABLE),                 ==, ADG_TYPE_TABLE_STYLE);
}

static void
_adg_test_related(void)
{
    /* Try some successful combination */
    g_assert(adg_dress_are_related(ADG_DRESS_COLOR, ADG_DRESS_COLOR));
    g_assert(adg_dress_are_related(ADG_DRESS_FONT, ADG_DRESS_FONT));
    g_assert(adg_dress_are_related(ADG_DRESS_COLOR_HIDDEN, ADG_DRESS_COLOR_BACKGROUND));
    g_assert(adg_dress_are_related(ADG_DRESS_COLOR_STROKE, ADG_DRESS_COLOR));
    g_assert(adg_dress_are_related(ADG_DRESS_COLOR_DIMENSION, ADG_DRESS_COLOR_AXIS));
    g_assert(adg_dress_are_related(ADG_DRESS_COLOR, ADG_DRESS_COLOR_FILL));
    g_assert(adg_dress_are_related(ADG_DRESS_LINE_FRAME, ADG_DRESS_LINE_STROKE));
    g_assert(adg_dress_are_related(ADG_DRESS_LINE_FILL, ADG_DRESS_LINE_HIDDEN));
    g_assert(adg_dress_are_related(ADG_DRESS_LINE_AXIS, ADG_DRESS_LINE));
    g_assert(adg_dress_are_related(ADG_DRESS_FONT_QUOTE_TEXT, ADG_DRESS_FONT));
    g_assert(adg_dress_are_related(ADG_DRESS_FONT_ANNOTATION, ADG_DRESS_FONT_QUOTE_ANNOTATION));
    g_assert(adg_dress_are_related(ADG_DRESS_FILL, ADG_DRESS_FILL_HATCH));

    /* Try some combination that must not match */
    g_assert(adg_dress_are_related(ADG_DRESS_FILL_HATCH, ADG_DRESS_COLOR_HIDDEN) == FALSE);
    g_assert(adg_dress_are_related(ADG_DRESS_FONT_QUOTE_ANNOTATION, ADG_DRESS_DIMENSION) == FALSE);
    g_assert(adg_dress_are_related(ADG_DRESS_TABLE, ADG_DRESS_FONT) == FALSE);
    g_assert(adg_dress_are_related(ADG_DRESS_FONT_ANNOTATION, ADG_DRESS_COLOR_ANNOTATION) == FALSE);
    g_assert(adg_dress_are_related(ADG_DRESS_COLOR, ADG_DRESS_LINE) == FALSE);
    g_assert(adg_dress_are_related(ADG_DRESS_LINE_FILL, ADG_DRESS_COLOR_FILL) == FALSE);
    g_assert(adg_dress_are_related(ADG_DRESS_FONT_QUOTE_ANNOTATION, ADG_DRESS_LINE_GRID) == FALSE);
    g_assert(adg_dress_are_related(ADG_DRESS_COLOR_AXIS, ADG_DRESS_LINE_HIDDEN) == FALSE);
}

static void
_adg_test_builtins(void)
{
    /* Check built-in names */
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR),                 ==, "color");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_BACKGROUND),      ==, "color-background");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_STROKE),          ==, "color-stroke");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_DIMENSION),       ==, "color-dimension");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_ANNOTATION),      ==, "color-annotation");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_FILL),            ==, "color-fill");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_AXIS),            ==, "color-axis");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_HIDDEN),          ==, "color-hidden");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE),                  ==, "line");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_STROKE),           ==, "line-stroke");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_DIMENSION),        ==, "line-dimension");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_FILL),             ==, "line-fill");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_GRID),             ==, "line-grid");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_FRAME),            ==, "line-frame");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_AXIS),             ==, "line-axis");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_HIDDEN),           ==, "line-hidden");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT),                  ==, "font");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_TEXT),             ==, "font-text");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_ANNOTATION),       ==, "font-annotation");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_QUOTE_TEXT),       ==, "font-quote-text");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_QUOTE_ANNOTATION), ==, "font-quote-annotation");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_DIMENSION),             ==, "dimension");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FILL),                  ==, "fill");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FILL_HATCH),            ==, "fill-hatch");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_TABLE),                 ==, "table");

    /* Check built-in values */
    g_assert_cmpint(adg_dress_from_name("color"),                 ==, ADG_DRESS_COLOR);
    g_assert_cmpint(adg_dress_from_name("color-background"),      ==, ADG_DRESS_COLOR_BACKGROUND);
    g_assert_cmpint(adg_dress_from_name("color-stroke"),          ==, ADG_DRESS_COLOR_STROKE);
    g_assert_cmpint(adg_dress_from_name("color-dimension"),       ==, ADG_DRESS_COLOR_DIMENSION);
    g_assert_cmpint(adg_dress_from_name("color-annotation"),      ==, ADG_DRESS_COLOR_ANNOTATION);
    g_assert_cmpint(adg_dress_from_name("color-fill"),            ==, ADG_DRESS_COLOR_FILL);
    g_assert_cmpint(adg_dress_from_name("color-axis"),            ==, ADG_DRESS_COLOR_AXIS);
    g_assert_cmpint(adg_dress_from_name("color-hidden"),          ==, ADG_DRESS_COLOR_HIDDEN);
    g_assert_cmpint(adg_dress_from_name("line"),                  ==, ADG_DRESS_LINE);
    g_assert_cmpint(adg_dress_from_name("line-stroke"),           ==, ADG_DRESS_LINE_STROKE);
    g_assert_cmpint(adg_dress_from_name("line-dimension"),        ==, ADG_DRESS_LINE_DIMENSION);
    g_assert_cmpint(adg_dress_from_name("line-fill"),             ==, ADG_DRESS_LINE_FILL);
    g_assert_cmpint(adg_dress_from_name("line-grid"),             ==, ADG_DRESS_LINE_GRID);
    g_assert_cmpint(adg_dress_from_name("line-frame"),            ==, ADG_DRESS_LINE_FRAME);
    g_assert_cmpint(adg_dress_from_name("line-axis"),             ==, ADG_DRESS_LINE_AXIS);
    g_assert_cmpint(adg_dress_from_name("line-hidden"),           ==, ADG_DRESS_LINE_HIDDEN);
    g_assert_cmpint(adg_dress_from_name("font"),                  ==, ADG_DRESS_FONT);
    g_assert_cmpint(adg_dress_from_name("font-text"),             ==, ADG_DRESS_FONT_TEXT);
    g_assert_cmpint(adg_dress_from_name("font-annotation"),       ==, ADG_DRESS_FONT_ANNOTATION);
    g_assert_cmpint(adg_dress_from_name("font-quote-text"),       ==, ADG_DRESS_FONT_QUOTE_TEXT);
    g_assert_cmpint(adg_dress_from_name("font-quote-annotation"), ==, ADG_DRESS_FONT_QUOTE_ANNOTATION);
    g_assert_cmpint(adg_dress_from_name("dimension"),             ==, ADG_DRESS_DIMENSION);
    g_assert_cmpint(adg_dress_from_name("fill"),                  ==, ADG_DRESS_FILL);
    g_assert_cmpint(adg_dress_from_name("fill-hatch"),            ==, ADG_DRESS_FILL_HATCH);
    g_assert_cmpint(adg_dress_from_name("table"),                 ==, ADG_DRESS_TABLE);
}

int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/dress/generic",  _adg_test_generic);
    adg_test_add_func("/adg/dress/set",      _adg_test_set);
    adg_test_add_func("/adg/dress/ancestor", _adg_test_ancestor);
    adg_test_add_func("/adg/dress/related",  _adg_test_related);
    adg_test_add_func("/adg/dress/builtins", _adg_test_builtins);

    return g_test_run();
}
