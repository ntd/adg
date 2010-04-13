/* ADG - Automatic Drawing Generation
 * Copyright (C) 2010  Nicola Fontana <ntd at entidi.it>
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


static void
_adg_baseline_spacing(void)
{
    AdgDimStyle *dim_style;
    gdouble valid_baseline_spacing_1, valid_baseline_spacing_2, invalid_baseline_spacing;
    gdouble baseline_spacing;

    dim_style = adg_dim_style_new();
    valid_baseline_spacing_1 = 0;
    valid_baseline_spacing_2 = 999;
    invalid_baseline_spacing = -1;

    /* Using the public APIs */
    adg_dim_style_set_baseline_spacing(dim_style, valid_baseline_spacing_1);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    g_assert_cmpfloat(baseline_spacing, ==, valid_baseline_spacing_1);

    adg_dim_style_set_baseline_spacing(dim_style, invalid_baseline_spacing);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    g_assert_cmpfloat(baseline_spacing, ==, valid_baseline_spacing_1);

    adg_dim_style_set_baseline_spacing(dim_style, valid_baseline_spacing_2);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    g_assert_cmpfloat(baseline_spacing, ==, valid_baseline_spacing_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "baseline-spacing", valid_baseline_spacing_1, NULL);
    g_object_get(dim_style, "baseline-spacing", &baseline_spacing, NULL);
    g_assert_cmpfloat(baseline_spacing, ==, valid_baseline_spacing_1);

    g_object_set(dim_style, "baseline-spacing", invalid_baseline_spacing, NULL);
    g_object_get(dim_style, "baseline-spacing", &baseline_spacing, NULL);
    g_assert_cmpfloat(baseline_spacing, ==, valid_baseline_spacing_1);

    g_object_set(dim_style, "baseline-spacing", valid_baseline_spacing_2, NULL);
    g_object_get(dim_style, "baseline-spacing", &baseline_spacing, NULL);
    g_assert_cmpfloat(baseline_spacing, ==, valid_baseline_spacing_2);

    g_object_unref(dim_style);
}

static void
_adg_beyond(void)
{
    AdgDimStyle *dim_style;
    gdouble valid_beyond_1, valid_beyond_2, invalid_beyond;
    gdouble beyond;

    dim_style = adg_dim_style_new();
    valid_beyond_1 = 0;
    valid_beyond_2 = 999;
    invalid_beyond = -1;

    /* Using the public APIs */
    adg_dim_style_set_beyond(dim_style, valid_beyond_1);
    beyond = adg_dim_style_get_beyond(dim_style);
    g_assert_cmpfloat(beyond, ==, valid_beyond_1);

    adg_dim_style_set_beyond(dim_style, invalid_beyond);
    beyond = adg_dim_style_get_beyond(dim_style);
    g_assert_cmpfloat(beyond, ==, valid_beyond_1);

    adg_dim_style_set_beyond(dim_style, valid_beyond_2);
    beyond = adg_dim_style_get_beyond(dim_style);
    g_assert_cmpfloat(beyond, ==, valid_beyond_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "beyond", valid_beyond_1, NULL);
    g_object_get(dim_style, "beyond", &beyond, NULL);
    g_assert_cmpfloat(beyond, ==, valid_beyond_1);

    g_object_set(dim_style, "beyond", invalid_beyond, NULL);
    g_object_get(dim_style, "beyond", &beyond, NULL);
    g_assert_cmpfloat(beyond, ==, valid_beyond_1);

    g_object_set(dim_style, "beyond", valid_beyond_2, NULL);
    g_object_get(dim_style, "beyond", &beyond, NULL);
    g_assert_cmpfloat(beyond, ==, valid_beyond_2);

    g_object_unref(dim_style);
}

static void
_adg_color_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress color_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_COLOR_DIMENSION;
    valid_dress_2 = ADG_DRESS_COLOR_HATCH;
    incompatible_dress = ADG_DRESS_FONT_LIMIT;

    /* Using the public APIs */
    adg_dim_style_set_color_dress(dim_style, valid_dress_1);
    color_dress = adg_dim_style_get_color_dress(dim_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_dim_style_set_color_dress(dim_style, incompatible_dress);
    color_dress = adg_dim_style_get_color_dress(dim_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_dim_style_set_color_dress(dim_style, valid_dress_2);
    color_dress = adg_dim_style_get_color_dress(dim_style);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "color-dress", valid_dress_1, NULL);
    g_object_get(dim_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(dim_style, "color-dress", incompatible_dress, NULL);
    g_object_get(dim_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(dim_style, "color-dress", valid_dress_2, NULL);
    g_object_get(dim_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    g_object_unref(dim_style);
}

static void
_adg_from_offset(void)
{
    AdgDimStyle *dim_style;
    gdouble valid_from_offset_1, valid_from_offset_2, invalid_from_offset;
    gdouble from_offset;

    dim_style = adg_dim_style_new();
    valid_from_offset_1 = 0;
    valid_from_offset_2 = 999;
    invalid_from_offset = -1;

    /* Using the public APIs */
    adg_dim_style_set_from_offset(dim_style, valid_from_offset_1);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    g_assert_cmpfloat(from_offset, ==, valid_from_offset_1);

    adg_dim_style_set_from_offset(dim_style, invalid_from_offset);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    g_assert_cmpfloat(from_offset, ==, valid_from_offset_1);

    adg_dim_style_set_from_offset(dim_style, valid_from_offset_2);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    g_assert_cmpfloat(from_offset, ==, valid_from_offset_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "from-offset", valid_from_offset_1, NULL);
    g_object_get(dim_style, "from-offset", &from_offset, NULL);
    g_assert_cmpfloat(from_offset, ==, valid_from_offset_1);

    g_object_set(dim_style, "from-offset", invalid_from_offset, NULL);
    g_object_get(dim_style, "from-offset", &from_offset, NULL);
    g_assert_cmpfloat(from_offset, ==, valid_from_offset_1);

    g_object_set(dim_style, "from-offset", valid_from_offset_2, NULL);
    g_object_get(dim_style, "from-offset", &from_offset, NULL);
    g_assert_cmpfloat(from_offset, ==, valid_from_offset_2);

    g_object_unref(dim_style);
}

static void
_adg_limits_shift(void)
{
    AdgDimStyle *dim_style;
    AdgPair null_shift, identity_shift;
    const AdgPair *shift;
    AdgPair *shift_dup;

    dim_style = adg_dim_style_new();
    null_shift.x = 0;
    null_shift.y = 0;
    identity_shift.x = 1;
    identity_shift.y = 1;

    /* Using the public APIs */
    adg_dim_style_set_limits_shift(dim_style, &identity_shift);
    shift = adg_dim_style_get_limits_shift(dim_style);
    g_assert(adg_pair_equal(shift, &identity_shift));

    adg_dim_style_set_limits_shift(dim_style, &null_shift);
    shift = adg_dim_style_get_limits_shift(dim_style);
    g_assert(adg_pair_equal(shift, &null_shift));

    adg_dim_style_set_limits_shift(dim_style, NULL);
    shift = adg_dim_style_get_limits_shift(dim_style);
    g_assert(adg_pair_equal(shift, &null_shift));

    /* Using GObject property methods */
    g_object_set(dim_style, "limits-shift", &identity_shift, NULL);
    g_object_get(dim_style, "limits-shift", &shift_dup, NULL);
    g_assert(adg_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "limits-shift", NULL, NULL);
    g_object_get(dim_style, "limits-shift", &shift_dup, NULL);
    g_assert(adg_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "limits-shift", &null_shift, NULL);
    g_object_get(dim_style, "limits-shift", &shift_dup, NULL);
    g_assert(adg_pair_equal(shift_dup, &null_shift));
    g_free(shift_dup);

    g_object_unref(dim_style);
}

static void
_adg_limits_spacing(void)
{
    AdgDimStyle *dim_style;
    gdouble valid_limits_spacing_1, valid_limits_spacing_2, invalid_limits_spacing;
    gdouble limits_spacing;

    dim_style = adg_dim_style_new();
    valid_limits_spacing_1 = 0;
    valid_limits_spacing_2 = 999;
    invalid_limits_spacing = -1;

    /* Using the public APIs */
    adg_dim_style_set_limits_spacing(dim_style, valid_limits_spacing_1);
    limits_spacing = adg_dim_style_get_limits_spacing(dim_style);
    g_assert_cmpfloat(limits_spacing, ==, valid_limits_spacing_1);

    adg_dim_style_set_limits_spacing(dim_style, invalid_limits_spacing);
    limits_spacing = adg_dim_style_get_limits_spacing(dim_style);
    g_assert_cmpfloat(limits_spacing, ==, valid_limits_spacing_1);

    adg_dim_style_set_limits_spacing(dim_style, valid_limits_spacing_2);
    limits_spacing = adg_dim_style_get_limits_spacing(dim_style);
    g_assert_cmpfloat(limits_spacing, ==, valid_limits_spacing_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "limits-spacing", valid_limits_spacing_1, NULL);
    g_object_get(dim_style, "limits-spacing", &limits_spacing, NULL);
    g_assert_cmpfloat(limits_spacing, ==, valid_limits_spacing_1);

    g_object_set(dim_style, "limits-spacing", invalid_limits_spacing, NULL);
    g_object_get(dim_style, "limits-spacing", &limits_spacing, NULL);
    g_assert_cmpfloat(limits_spacing, ==, valid_limits_spacing_1);

    g_object_set(dim_style, "limits-spacing", valid_limits_spacing_2, NULL);
    g_object_get(dim_style, "limits-spacing", &limits_spacing, NULL);
    g_assert_cmpfloat(limits_spacing, ==, valid_limits_spacing_2);

    g_object_unref(dim_style);
}

static void
_adg_line_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress line_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_LINE_HATCH;
    valid_dress_2 = ADG_DRESS_LINE;
    incompatible_dress = ADG_DRESS_COLOR;

    /* Using the public APIs */
    adg_dim_style_set_line_dress(dim_style, valid_dress_1);
    line_dress = adg_dim_style_get_line_dress(dim_style);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    adg_dim_style_set_line_dress(dim_style, incompatible_dress);
    line_dress = adg_dim_style_get_line_dress(dim_style);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    adg_dim_style_set_line_dress(dim_style, valid_dress_2);
    line_dress = adg_dim_style_get_line_dress(dim_style);
    g_assert_cmpint(line_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "line-dress", valid_dress_1, NULL);
    g_object_get(dim_style, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    g_object_set(dim_style, "line-dress", incompatible_dress, NULL);
    g_object_get(dim_style, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_1);

    g_object_set(dim_style, "line-dress", valid_dress_2, NULL);
    g_object_get(dim_style, "line-dress", &line_dress, NULL);
    g_assert_cmpint(line_dress, ==, valid_dress_2);

    g_object_unref(dim_style);
}

static void
_adg_marker1(void)
{
    AdgDimStyle *dim_style;
    AdgMarker *valid_marker, *invalid_marker, *marker;

    dim_style = adg_dim_style_new();
    valid_marker = ADG_MARKER(adg_arrow_new());
    invalid_marker = adg_test_invalid_pointer();

    /* Using the public APIs */
    adg_dim_style_set_marker1(dim_style, valid_marker);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    adg_dim_style_set_marker1(dim_style, invalid_marker);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    adg_dim_style_set_marker1(dim_style, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert(marker == NULL);

    /* Using GObject property methods */
    g_object_set(dim_style, "marker1", valid_marker, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    g_object_set(dim_style, "marker1", invalid_marker, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    g_object_set(dim_style, "marker1", NULL, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert(marker == NULL);

    g_object_unref(dim_style);
    g_object_unref(valid_marker);
}

static void
_adg_marker2(void)
{
    AdgDimStyle *dim_style;
    AdgMarker *valid_marker, *invalid_marker, *marker;

    dim_style = adg_dim_style_new();
    valid_marker = ADG_MARKER(adg_arrow_new());
    invalid_marker = adg_test_invalid_pointer();

    /* Using the public APIs */
    adg_dim_style_set_marker2(dim_style, valid_marker);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    adg_dim_style_set_marker2(dim_style, invalid_marker);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    adg_dim_style_set_marker2(dim_style, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert(marker == NULL);

    /* Using GObject property methods */
    g_object_set(dim_style, "marker2", valid_marker, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    g_object_set(dim_style, "marker2", invalid_marker, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert(marker != NULL);
    g_object_unref(marker);

    g_object_set(dim_style, "marker2", NULL, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert(marker == NULL);

    g_object_unref(dim_style);
    g_object_unref(valid_marker);
}

static void
_adg_max_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress max_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_FONT;
    valid_dress_2 = ADG_DRESS_FONT_LIMIT;
    incompatible_dress = ADG_DRESS_FILL_HATCH;

    /* Using the public APIs */
    adg_dim_style_set_max_dress(dim_style, valid_dress_1);
    max_dress = adg_dim_style_get_max_dress(dim_style);
    g_assert_cmpint(max_dress, ==, valid_dress_1);

    adg_dim_style_set_max_dress(dim_style, incompatible_dress);
    max_dress = adg_dim_style_get_max_dress(dim_style);
    g_assert_cmpint(max_dress, ==, valid_dress_1);

    adg_dim_style_set_max_dress(dim_style, valid_dress_2);
    max_dress = adg_dim_style_get_max_dress(dim_style);
    g_assert_cmpint(max_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "max-dress", valid_dress_1, NULL);
    g_object_get(dim_style, "max-dress", &max_dress, NULL);
    g_assert_cmpint(max_dress, ==, valid_dress_1);

    g_object_set(dim_style, "max-dress", incompatible_dress, NULL);
    g_object_get(dim_style, "max-dress", &max_dress, NULL);
    g_assert_cmpint(max_dress, ==, valid_dress_1);

    g_object_set(dim_style, "max-dress", valid_dress_2, NULL);
    g_object_get(dim_style, "max-dress", &max_dress, NULL);
    g_assert_cmpint(max_dress, ==, valid_dress_2);

    g_object_unref(dim_style);
}

static void
_adg_min_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress min_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_FONT;
    valid_dress_2 = ADG_DRESS_FONT_LIMIT;
    incompatible_dress = ADG_DRESS_FILL_HATCH;

    /* Using the public APIs */
    adg_dim_style_set_min_dress(dim_style, valid_dress_1);
    min_dress = adg_dim_style_get_min_dress(dim_style);
    g_assert_cmpint(min_dress, ==, valid_dress_1);

    adg_dim_style_set_min_dress(dim_style, incompatible_dress);
    min_dress = adg_dim_style_get_min_dress(dim_style);
    g_assert_cmpint(min_dress, ==, valid_dress_1);

    adg_dim_style_set_min_dress(dim_style, valid_dress_2);
    min_dress = adg_dim_style_get_min_dress(dim_style);
    g_assert_cmpint(min_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "min-dress", valid_dress_1, NULL);
    g_object_get(dim_style, "min-dress", &min_dress, NULL);
    g_assert_cmpint(min_dress, ==, valid_dress_1);

    g_object_set(dim_style, "min-dress", incompatible_dress, NULL);
    g_object_get(dim_style, "min-dress", &min_dress, NULL);
    g_assert_cmpint(min_dress, ==, valid_dress_1);

    g_object_set(dim_style, "min-dress", valid_dress_2, NULL);
    g_object_get(dim_style, "min-dress", &min_dress, NULL);
    g_assert_cmpint(min_dress, ==, valid_dress_2);

    g_object_unref(dim_style);
}

static void
_adg_number_format(void)
{
    AdgDimStyle *dim_style;
    const gchar *valid_text_1, *valid_text_2;
    const gchar *number_format;
    gchar *number_format_dup;

    dim_style = adg_dim_style_new();
    valid_text_1 = "%lf";
    valid_text_2 = "%abc%";

    /* Using the public APIs */
    adg_dim_style_set_number_format(dim_style, valid_text_1);
    number_format = adg_dim_style_get_number_format(dim_style);
    g_assert_cmpstr(number_format, ==, valid_text_1);

    adg_dim_style_set_number_format(dim_style, valid_text_2);
    number_format = adg_dim_style_get_number_format(dim_style);
    g_assert_cmpstr(number_format, ==, valid_text_2);

    adg_dim_style_set_number_format(dim_style, NULL);
    number_format = adg_dim_style_get_number_format(dim_style);
    g_assert(number_format == NULL);

    /* Using GObject property methods */
    g_object_set(dim_style, "number-format", valid_text_1, NULL);
    g_object_get(dim_style, "number-format", &number_format_dup, NULL);
    g_assert_cmpstr(number_format_dup, ==, valid_text_1);
    g_free(number_format_dup);

    g_object_set(dim_style, "number-format", valid_text_2, NULL);
    g_object_get(dim_style, "number-format", &number_format_dup, NULL);
    g_assert_cmpstr(number_format_dup, ==, valid_text_2);
    g_free(number_format_dup);

    g_object_set(dim_style, "number-format", NULL, NULL);
    g_object_get(dim_style, "number-format", &number_format_dup, NULL);
    g_assert(number_format_dup == NULL);

    g_object_unref(dim_style);
}

static void
_adg_number_tag(void)
{
    AdgDimStyle *dim_style;
    const gchar *valid_text_1, *valid_text_2;
    const gchar *number_tag;
    gchar *number_tag_dup;

    dim_style = adg_dim_style_new();
    valid_text_1 = "";
    valid_text_2 = "<è>";

    /* Using the public APIs */
    adg_dim_style_set_number_tag(dim_style, valid_text_1);
    number_tag = adg_dim_style_get_number_tag(dim_style);
    g_assert_cmpstr(number_tag, ==, valid_text_1);

    adg_dim_style_set_number_tag(dim_style, valid_text_2);
    number_tag = adg_dim_style_get_number_tag(dim_style);
    g_assert_cmpstr(number_tag, ==, valid_text_2);

    adg_dim_style_set_number_tag(dim_style, NULL);
    number_tag = adg_dim_style_get_number_tag(dim_style);
    g_assert(number_tag == NULL);

    /* Using GObject property methods */
    g_object_set(dim_style, "number-tag", valid_text_1, NULL);
    g_object_get(dim_style, "number-tag", &number_tag_dup, NULL);
    g_assert_cmpstr(number_tag_dup, ==, valid_text_1);
    g_free(number_tag_dup);

    g_object_set(dim_style, "number-tag", valid_text_2, NULL);
    g_object_get(dim_style, "number-tag", &number_tag_dup, NULL);
    g_assert_cmpstr(number_tag_dup, ==, valid_text_2);
    g_free(number_tag_dup);

    g_object_set(dim_style, "number-tag", NULL, NULL);
    g_object_get(dim_style, "number-tag", &number_tag_dup, NULL);
    g_assert(number_tag_dup == NULL);

    g_object_unref(dim_style);
}

static void
_adg_quote_shift(void)
{
    AdgDimStyle *dim_style;
    AdgPair null_shift, identity_shift;
    const AdgPair *shift;
    AdgPair *shift_dup;

    dim_style = adg_dim_style_new();
    null_shift.x = 0;
    null_shift.y = 0;
    identity_shift.x = 1;
    identity_shift.y = 1;

    /* Using the public APIs */
    adg_dim_style_set_quote_shift(dim_style, &identity_shift);
    shift = adg_dim_style_get_quote_shift(dim_style);
    g_assert(adg_pair_equal(shift, &identity_shift));

    adg_dim_style_set_quote_shift(dim_style, &null_shift);
    shift = adg_dim_style_get_quote_shift(dim_style);
    g_assert(adg_pair_equal(shift, &null_shift));

    adg_dim_style_set_quote_shift(dim_style, NULL);
    shift = adg_dim_style_get_quote_shift(dim_style);
    g_assert(adg_pair_equal(shift, &null_shift));

    /* Using GObject property methods */
    g_object_set(dim_style, "quote-shift", &identity_shift, NULL);
    g_object_get(dim_style, "quote-shift", &shift_dup, NULL);
    g_assert(adg_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "quote-shift", NULL, NULL);
    g_object_get(dim_style, "quote-shift", &shift_dup, NULL);
    g_assert(adg_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "quote-shift", &null_shift, NULL);
    g_object_get(dim_style, "quote-shift", &shift_dup, NULL);
    g_assert(adg_pair_equal(shift_dup, &null_shift));
    g_free(shift_dup);

    g_object_unref(dim_style);
}

static void
_adg_to_offset(void)
{
    AdgDimStyle *dim_style;
    gdouble valid_to_offset_1, valid_to_offset_2, invalid_to_offset;
    gdouble to_offset;

    dim_style = adg_dim_style_new();
    valid_to_offset_1 = 0;
    valid_to_offset_2 = 999;
    invalid_to_offset = -1;

    /* Using the public APIs */
    adg_dim_style_set_to_offset(dim_style, valid_to_offset_1);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    g_assert_cmpfloat(to_offset, ==, valid_to_offset_1);

    adg_dim_style_set_to_offset(dim_style, invalid_to_offset);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    g_assert_cmpfloat(to_offset, ==, valid_to_offset_1);

    adg_dim_style_set_to_offset(dim_style, valid_to_offset_2);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    g_assert_cmpfloat(to_offset, ==, valid_to_offset_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "to-offset", valid_to_offset_1, NULL);
    g_object_get(dim_style, "to-offset", &to_offset, NULL);
    g_assert_cmpfloat(to_offset, ==, valid_to_offset_1);

    g_object_set(dim_style, "to-offset", invalid_to_offset, NULL);
    g_object_get(dim_style, "to-offset", &to_offset, NULL);
    g_assert_cmpfloat(to_offset, ==, valid_to_offset_1);

    g_object_set(dim_style, "to-offset", valid_to_offset_2, NULL);
    g_object_get(dim_style, "to-offset", &to_offset, NULL);
    g_assert_cmpfloat(to_offset, ==, valid_to_offset_2);

    g_object_unref(dim_style);
}

static void
_adg_value_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress value_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_FONT;
    valid_dress_2 = ADG_DRESS_FONT_LIMIT;
    incompatible_dress = ADG_DRESS_FILL_HATCH;

    /* Using the public APIs */
    adg_dim_style_set_value_dress(dim_style, valid_dress_1);
    value_dress = adg_dim_style_get_value_dress(dim_style);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    adg_dim_style_set_value_dress(dim_style, incompatible_dress);
    value_dress = adg_dim_style_get_value_dress(dim_style);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    adg_dim_style_set_value_dress(dim_style, valid_dress_2);
    value_dress = adg_dim_style_get_value_dress(dim_style);
    g_assert_cmpint(value_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "value-dress", valid_dress_1, NULL);
    g_object_get(dim_style, "value-dress", &value_dress, NULL);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    g_object_set(dim_style, "value-dress", incompatible_dress, NULL);
    g_object_get(dim_style, "value-dress", &value_dress, NULL);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    g_object_set(dim_style, "value-dress", valid_dress_2, NULL);
    g_object_get(dim_style, "value-dress", &value_dress, NULL);
    g_assert_cmpint(value_dress, ==, valid_dress_2);

    g_object_unref(dim_style);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/dim-style/baseline-spacing", _adg_baseline_spacing);
    adg_test_add_func("/adg/dim-style/beyond", _adg_beyond);
    adg_test_add_func("/adg/dim-style/color-dress", _adg_color_dress);
    adg_test_add_func("/adg/dim-style/from-offset", _adg_from_offset);
    adg_test_add_func("/adg/dim-style/limits-shift", _adg_limits_shift);
    adg_test_add_func("/adg/dim-style/limits-spacing", _adg_limits_spacing);
    adg_test_add_func("/adg/dim-style/line-dress", _adg_line_dress);
    adg_test_add_func("/adg/dim-style/marker1", _adg_marker1);
    adg_test_add_func("/adg/dim-style/marker2", _adg_marker2);
    adg_test_add_func("/adg/dim-style/max-dress", _adg_max_dress);
    adg_test_add_func("/adg/dim-style/min-dress", _adg_min_dress);
    adg_test_add_func("/adg/dim-style/number-format", _adg_number_format);
    adg_test_add_func("/adg/dim-style/number-tag", _adg_number_tag);
    adg_test_add_func("/adg/dim-style/quote-shift", _adg_quote_shift);
    adg_test_add_func("/adg/dim-style/to-offset", _adg_to_offset);
    adg_test_add_func("/adg/dim-style/value-dress", _adg_value_dress);

    return g_test_run();
}
