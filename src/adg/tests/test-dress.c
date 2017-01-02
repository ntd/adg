/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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


#include <adg-test.h>
#include <adg.h>

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
_adg_behavior_builtins(void)
{
    /* Check built-in names */
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR),                 ==, "ADG_DRESS_COLOR");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_BACKGROUND),      ==, "ADG_DRESS_COLOR_BACKGROUND");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_STROKE),          ==, "ADG_DRESS_COLOR_STROKE");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_DIMENSION),       ==, "ADG_DRESS_COLOR_DIMENSION");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_ANNOTATION),      ==, "ADG_DRESS_COLOR_ANNOTATION");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_FILL),            ==, "ADG_DRESS_COLOR_FILL");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_AXIS),            ==, "ADG_DRESS_COLOR_AXIS");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_COLOR_HIDDEN),          ==, "ADG_DRESS_COLOR_HIDDEN");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE),                  ==, "ADG_DRESS_LINE");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_STROKE),           ==, "ADG_DRESS_LINE_STROKE");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_DIMENSION),        ==, "ADG_DRESS_LINE_DIMENSION");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_FILL),             ==, "ADG_DRESS_LINE_FILL");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_GRID),             ==, "ADG_DRESS_LINE_GRID");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_FRAME),            ==, "ADG_DRESS_LINE_FRAME");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_AXIS),             ==, "ADG_DRESS_LINE_AXIS");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_LINE_HIDDEN),           ==, "ADG_DRESS_LINE_HIDDEN");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT),                  ==, "ADG_DRESS_FONT");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_TEXT),             ==, "ADG_DRESS_FONT_TEXT");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_ANNOTATION),       ==, "ADG_DRESS_FONT_ANNOTATION");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_QUOTE_TEXT),       ==, "ADG_DRESS_FONT_QUOTE_TEXT");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FONT_QUOTE_ANNOTATION), ==, "ADG_DRESS_FONT_QUOTE_ANNOTATION");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_DIMENSION),             ==, "ADG_DRESS_DIMENSION");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FILL),                  ==, "ADG_DRESS_FILL");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_FILL_HATCH),            ==, "ADG_DRESS_FILL_HATCH");
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_TABLE),                 ==, "ADG_DRESS_TABLE");

    /* Check built-in values */
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR"),                 ==, ADG_DRESS_COLOR);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR_BACKGROUND"),      ==, ADG_DRESS_COLOR_BACKGROUND);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR_STROKE"),          ==, ADG_DRESS_COLOR_STROKE);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR_DIMENSION"),       ==, ADG_DRESS_COLOR_DIMENSION);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR_ANNOTATION"),      ==, ADG_DRESS_COLOR_ANNOTATION);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR_FILL"),            ==, ADG_DRESS_COLOR_FILL);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR_AXIS"),            ==, ADG_DRESS_COLOR_AXIS);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_COLOR_HIDDEN"),          ==, ADG_DRESS_COLOR_HIDDEN);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE"),                  ==, ADG_DRESS_LINE);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE_STROKE"),           ==, ADG_DRESS_LINE_STROKE);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE_DIMENSION"),        ==, ADG_DRESS_LINE_DIMENSION);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE_FILL"),             ==, ADG_DRESS_LINE_FILL);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE_GRID"),             ==, ADG_DRESS_LINE_GRID);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE_FRAME"),            ==, ADG_DRESS_LINE_FRAME);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE_AXIS"),             ==, ADG_DRESS_LINE_AXIS);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_LINE_HIDDEN"),           ==, ADG_DRESS_LINE_HIDDEN);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_FONT"),                  ==, ADG_DRESS_FONT);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_FONT_TEXT"),             ==, ADG_DRESS_FONT_TEXT);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_FONT_ANNOTATION"),       ==, ADG_DRESS_FONT_ANNOTATION);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_FONT_QUOTE_TEXT"),       ==, ADG_DRESS_FONT_QUOTE_TEXT);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_FONT_QUOTE_ANNOTATION"), ==, ADG_DRESS_FONT_QUOTE_ANNOTATION);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_DIMENSION"),             ==, ADG_DRESS_DIMENSION);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_FILL"),                  ==, ADG_DRESS_FILL);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_FILL_HATCH"),            ==, ADG_DRESS_FILL_HATCH);
    g_assert_cmpint(adg_dress_from_name("ADG_DRESS_TABLE"),                 ==, ADG_DRESS_TABLE);
}

static void
_adg_behavior_misc(void)
{
    g_assert_cmpstr(adg_dress_get_name(ADG_DRESS_UNDEFINED), ==, "ADG_DRESS_UNDEFINED");

    g_assert_true(adg_dress_get_ancestor_type(ADG_DRESS_UNDEFINED) == 0);

    g_assert_false(adg_dress_are_related(ADG_DRESS_UNDEFINED, ADG_DRESS_UNDEFINED));

    g_assert_cmpint(adg_dress_from_name("unexistent-dress"), ==, ADG_DRESS_UNDEFINED);
    g_assert_cmpint(adg_dress_from_name(NULL), ==, ADG_DRESS_UNDEFINED);

    /* Ensure all the dresses have univoque id */
    g_assert_cmpint(ADG_DRESS_COLOR,                 !=, ADG_DRESS_COLOR_BACKGROUND);
    g_assert_cmpint(ADG_DRESS_COLOR_BACKGROUND,      !=, ADG_DRESS_COLOR_STROKE);
    g_assert_cmpint(ADG_DRESS_COLOR_STROKE,          !=, ADG_DRESS_COLOR_DIMENSION);
    g_assert_cmpint(ADG_DRESS_COLOR_DIMENSION,       !=, ADG_DRESS_COLOR_ANNOTATION);
    g_assert_cmpint(ADG_DRESS_COLOR_ANNOTATION,      !=, ADG_DRESS_COLOR_FILL);
    g_assert_cmpint(ADG_DRESS_COLOR_FILL,            !=, ADG_DRESS_COLOR_AXIS);
    g_assert_cmpint(ADG_DRESS_COLOR_AXIS,            !=, ADG_DRESS_COLOR_HIDDEN);
    g_assert_cmpint(ADG_DRESS_COLOR_HIDDEN,          !=, ADG_DRESS_LINE);
    g_assert_cmpint(ADG_DRESS_LINE,                  !=, ADG_DRESS_LINE_STROKE);
    g_assert_cmpint(ADG_DRESS_LINE_STROKE,           !=, ADG_DRESS_LINE_DIMENSION);
    g_assert_cmpint(ADG_DRESS_LINE_DIMENSION,        !=, ADG_DRESS_LINE_FILL);
    g_assert_cmpint(ADG_DRESS_LINE_FILL,             !=, ADG_DRESS_LINE_GRID);
    g_assert_cmpint(ADG_DRESS_LINE_GRID,             !=, ADG_DRESS_LINE_FRAME);
    g_assert_cmpint(ADG_DRESS_LINE_FRAME,            !=, ADG_DRESS_LINE_AXIS);
    g_assert_cmpint(ADG_DRESS_LINE_AXIS,             !=, ADG_DRESS_LINE_HIDDEN);
    g_assert_cmpint(ADG_DRESS_LINE_HIDDEN,           !=, ADG_DRESS_FONT);
    g_assert_cmpint(ADG_DRESS_FONT,                  !=, ADG_DRESS_FONT_TEXT);
    g_assert_cmpint(ADG_DRESS_FONT_TEXT,             !=, ADG_DRESS_FONT_ANNOTATION);
    g_assert_cmpint(ADG_DRESS_FONT_ANNOTATION,       !=, ADG_DRESS_FONT_QUOTE_TEXT);
    g_assert_cmpint(ADG_DRESS_FONT_QUOTE_TEXT,       !=, ADG_DRESS_FONT_QUOTE_ANNOTATION);
    g_assert_cmpint(ADG_DRESS_FONT_QUOTE_ANNOTATION, !=, ADG_DRESS_DIMENSION);
    g_assert_cmpint(ADG_DRESS_DIMENSION,             !=, ADG_DRESS_FILL);
    g_assert_cmpint(ADG_DRESS_FILL,                  !=, ADG_DRESS_FILL_HATCH);
    g_assert_cmpint(ADG_DRESS_FILL_HATCH,            !=, ADG_DRESS_TABLE);
}

static void
_adg_method_set(void)
{
    AdgDress dress;

    /* Must fail because src is not related to color dress */
    dress = ADG_DRESS_COLOR;
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_LINE_STROKE));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FONT));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_DIMENSION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_TABLE));

    /* Must fail because src is not related to line dress */
    dress = ADG_DRESS_LINE;
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_COLOR_DIMENSION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FONT_ANNOTATION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_DIMENSION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FILL));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_TABLE));

    /* Must fail because src is not related to font dress */
    dress = ADG_DRESS_FONT;
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_COLOR_AXIS));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_LINE_FILL));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_DIMENSION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_TABLE));

    /* Must fail because src is not related to dimension dress */
    dress = ADG_DRESS_DIMENSION;
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_COLOR_HIDDEN));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_LINE_DIMENSION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FILL));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_TABLE));

    /* Must fail because src is not related to fill dress */
    dress = ADG_DRESS_FILL;
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_COLOR_FILL));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_LINE_FILL));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_ANNOTATION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_DIMENSION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_TABLE));

    /* Must fail because src is not related to table dress */
    dress = ADG_DRESS_TABLE;
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_COLOR_BACKGROUND));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_LINE_FRAME));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FONT));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_DIMENSION));

    /* Check for color dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_BACKGROUND));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_STROKE));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_DIMENSION));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_ANNOTATION));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_FILL));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_AXIS));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_HIDDEN));

    /* Check for line dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE_STROKE));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE_DIMENSION));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE_FILL));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE_GRID));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE_FRAME));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE_AXIS));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_LINE_HIDDEN));

    /* Check for font dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FONT));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FONT_TEXT));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FONT_ANNOTATION));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_ANNOTATION));

    /* Check for dimension dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_DIMENSION));

    /* Check for fill dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FILL));
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FILL_HATCH));

    /* Check for table dresses change */
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_TABLE));

    /* Assignment that does not change the dress must return FALSE */
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_FONT_QUOTE_TEXT));
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_COLOR_STROKE));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_COLOR_STROKE));
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_DIMENSION));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_DIMENSION));
    dress = ADG_DRESS_UNDEFINED;
    g_assert_true(adg_dress_set(&dress, ADG_DRESS_TABLE));
    g_assert_false(adg_dress_set(&dress, ADG_DRESS_TABLE));
}

static void
_adg_method_are_related(void)
{
    /* Try some successful combination */
    g_assert_true(adg_dress_are_related(ADG_DRESS_COLOR, ADG_DRESS_COLOR));
    g_assert_true(adg_dress_are_related(ADG_DRESS_FONT, ADG_DRESS_FONT));
    g_assert_true(adg_dress_are_related(ADG_DRESS_COLOR_HIDDEN, ADG_DRESS_COLOR_BACKGROUND));
    g_assert_true(adg_dress_are_related(ADG_DRESS_COLOR_STROKE, ADG_DRESS_COLOR));
    g_assert_true(adg_dress_are_related(ADG_DRESS_COLOR_DIMENSION, ADG_DRESS_COLOR_AXIS));
    g_assert_true(adg_dress_are_related(ADG_DRESS_COLOR, ADG_DRESS_COLOR_FILL));
    g_assert_true(adg_dress_are_related(ADG_DRESS_LINE_FRAME, ADG_DRESS_LINE_STROKE));
    g_assert_true(adg_dress_are_related(ADG_DRESS_LINE_FILL, ADG_DRESS_LINE_HIDDEN));
    g_assert_true(adg_dress_are_related(ADG_DRESS_LINE_AXIS, ADG_DRESS_LINE));
    g_assert_true(adg_dress_are_related(ADG_DRESS_FONT_QUOTE_TEXT, ADG_DRESS_FONT));
    g_assert_true(adg_dress_are_related(ADG_DRESS_FONT_ANNOTATION, ADG_DRESS_FONT_QUOTE_ANNOTATION));
    g_assert_true(adg_dress_are_related(ADG_DRESS_FILL, ADG_DRESS_FILL_HATCH));

    /* Try some combination that must not match */
    g_assert_false(adg_dress_are_related(ADG_DRESS_FILL_HATCH, ADG_DRESS_COLOR_HIDDEN));
    g_assert_false(adg_dress_are_related(ADG_DRESS_FONT_QUOTE_ANNOTATION, ADG_DRESS_DIMENSION));
    g_assert_false(adg_dress_are_related(ADG_DRESS_TABLE, ADG_DRESS_FONT));
    g_assert_false(adg_dress_are_related(ADG_DRESS_FONT_ANNOTATION, ADG_DRESS_COLOR_ANNOTATION));
    g_assert_false(adg_dress_are_related(ADG_DRESS_COLOR, ADG_DRESS_LINE));
    g_assert_false(adg_dress_are_related(ADG_DRESS_LINE_FILL, ADG_DRESS_COLOR_FILL));
    g_assert_false(adg_dress_are_related(ADG_DRESS_FONT_QUOTE_ANNOTATION, ADG_DRESS_LINE_GRID));
    g_assert_false(adg_dress_are_related(ADG_DRESS_COLOR_AXIS, ADG_DRESS_LINE_HIDDEN));
}

static void
_adg_method_get_ancestor_type(void)
{
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


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/adg/dress/behavior/builtins", _adg_behavior_builtins);
    g_test_add_func("/adg/dress/behavior/misc", _adg_behavior_misc);

    g_test_add_func("/adg/dress/method/set", _adg_method_set);
    g_test_add_func("/adg/dress/method/are-related",  _adg_method_are_related);
    g_test_add_func("/adg/dress/method/ancestor-type", _adg_method_get_ancestor_type);

    return g_test_run();
}
