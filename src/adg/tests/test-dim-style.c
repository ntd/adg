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


static void
_adg_property_baseline_spacing(void)
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
    adg_assert_isapprox(baseline_spacing, valid_baseline_spacing_1);

    adg_dim_style_set_baseline_spacing(dim_style, invalid_baseline_spacing);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    adg_assert_isapprox(baseline_spacing, valid_baseline_spacing_1);

    adg_dim_style_set_baseline_spacing(dim_style, valid_baseline_spacing_2);
    baseline_spacing = adg_dim_style_get_baseline_spacing(dim_style);
    adg_assert_isapprox(baseline_spacing, valid_baseline_spacing_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "baseline-spacing", valid_baseline_spacing_1, NULL);
    g_object_get(dim_style, "baseline-spacing", &baseline_spacing, NULL);
    adg_assert_isapprox(baseline_spacing, valid_baseline_spacing_1);

    g_object_set(dim_style, "baseline-spacing", invalid_baseline_spacing, NULL);
    g_object_get(dim_style, "baseline-spacing", &baseline_spacing, NULL);
    adg_assert_isapprox(baseline_spacing, valid_baseline_spacing_1);

    g_object_set(dim_style, "baseline-spacing", valid_baseline_spacing_2, NULL);
    g_object_get(dim_style, "baseline-spacing", &baseline_spacing, NULL);
    adg_assert_isapprox(baseline_spacing, valid_baseline_spacing_2);

    g_object_unref(dim_style);
}

static void
_adg_property_beyond(void)
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
    adg_assert_isapprox(beyond, valid_beyond_1);

    adg_dim_style_set_beyond(dim_style, invalid_beyond);
    beyond = adg_dim_style_get_beyond(dim_style);
    adg_assert_isapprox(beyond, valid_beyond_1);

    adg_dim_style_set_beyond(dim_style, valid_beyond_2);
    beyond = adg_dim_style_get_beyond(dim_style);
    adg_assert_isapprox(beyond, valid_beyond_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "beyond", valid_beyond_1, NULL);
    g_object_get(dim_style, "beyond", &beyond, NULL);
    adg_assert_isapprox(beyond, valid_beyond_1);

    g_object_set(dim_style, "beyond", invalid_beyond, NULL);
    g_object_get(dim_style, "beyond", &beyond, NULL);
    adg_assert_isapprox(beyond, valid_beyond_1);

    g_object_set(dim_style, "beyond", valid_beyond_2, NULL);
    g_object_get(dim_style, "beyond", &beyond, NULL);
    adg_assert_isapprox(beyond, valid_beyond_2);

    g_object_unref(dim_style);
}

static void
_adg_property_color_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress color_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_COLOR_ANNOTATION;
    valid_dress_2 = ADG_DRESS_COLOR;
    incompatible_dress = ADG_DRESS_FONT;

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
_adg_property_from_offset(void)
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
    adg_assert_isapprox(from_offset, valid_from_offset_1);

    adg_dim_style_set_from_offset(dim_style, invalid_from_offset);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    adg_assert_isapprox(from_offset, valid_from_offset_1);

    adg_dim_style_set_from_offset(dim_style, valid_from_offset_2);
    from_offset = adg_dim_style_get_from_offset(dim_style);
    adg_assert_isapprox(from_offset, valid_from_offset_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "from-offset", valid_from_offset_1, NULL);
    g_object_get(dim_style, "from-offset", &from_offset, NULL);
    adg_assert_isapprox(from_offset, valid_from_offset_1);

    g_object_set(dim_style, "from-offset", invalid_from_offset, NULL);
    g_object_get(dim_style, "from-offset", &from_offset, NULL);
    adg_assert_isapprox(from_offset, valid_from_offset_1);

    g_object_set(dim_style, "from-offset", valid_from_offset_2, NULL);
    g_object_get(dim_style, "from-offset", &from_offset, NULL);
    adg_assert_isapprox(from_offset, valid_from_offset_2);

    g_object_unref(dim_style);
}

static void
_adg_property_limits_shift(void)
{
    AdgDimStyle *dim_style;
    CpmlPair null_shift, identity_shift;
    const CpmlPair *shift;
    CpmlPair *shift_dup;

    dim_style = adg_dim_style_new();
    null_shift.x = 0;
    null_shift.y = 0;
    identity_shift.x = 1;
    identity_shift.y = 1;

    /* Using the public APIs */
    adg_dim_style_set_limits_shift(dim_style, &identity_shift);
    shift = adg_dim_style_get_limits_shift(dim_style);
    g_assert_true(cpml_pair_equal(shift, &identity_shift));

    adg_dim_style_set_limits_shift(dim_style, &null_shift);
    shift = adg_dim_style_get_limits_shift(dim_style);
    g_assert_true(cpml_pair_equal(shift, &null_shift));

    adg_dim_style_set_limits_shift(dim_style, NULL);
    shift = adg_dim_style_get_limits_shift(dim_style);
    g_assert_true(cpml_pair_equal(shift, &null_shift));

    /* Using GObject property methods */
    g_object_set(dim_style, "limits-shift", &identity_shift, NULL);
    g_object_get(dim_style, "limits-shift", &shift_dup, NULL);
    g_assert_true(cpml_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "limits-shift", NULL, NULL);
    g_object_get(dim_style, "limits-shift", &shift_dup, NULL);
    g_assert_true(cpml_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "limits-shift", &null_shift, NULL);
    g_object_get(dim_style, "limits-shift", &shift_dup, NULL);
    g_assert_true(cpml_pair_equal(shift_dup, &null_shift));
    g_free(shift_dup);

    g_object_unref(dim_style);
}

static void
_adg_property_limits_spacing(void)
{
    AdgDimStyle *dim_style;
    gdouble limits_spacing;

    dim_style = adg_dim_style_new();

    /* Using the public APIs */
    adg_dim_style_set_limits_spacing(dim_style, 999.);
    limits_spacing = adg_dim_style_get_limits_spacing(dim_style);
    adg_assert_isapprox(limits_spacing, 999.);

    /* Negative spacing is allowed */
    adg_dim_style_set_limits_spacing(dim_style, -1.);
    limits_spacing = adg_dim_style_get_limits_spacing(dim_style);
    adg_assert_isapprox(limits_spacing, -1.);

    adg_dim_style_set_limits_spacing(dim_style, 0.);
    limits_spacing = adg_dim_style_get_limits_spacing(dim_style);
    adg_assert_isapprox(limits_spacing, 0.);

    /* Using GObject property methods */
    g_object_set(dim_style, "limits-spacing", 999., NULL);
    g_object_get(dim_style, "limits-spacing", &limits_spacing, NULL);
    adg_assert_isapprox(limits_spacing, 999.);

    g_object_set(dim_style, "limits-spacing", -1., NULL);
    g_object_get(dim_style, "limits-spacing", &limits_spacing, NULL);
    adg_assert_isapprox(limits_spacing, -1.);

    g_object_set(dim_style, "limits-spacing", 0., NULL);
    g_object_get(dim_style, "limits-spacing", &limits_spacing, NULL);
    adg_assert_isapprox(limits_spacing, 0.);

    g_object_unref(dim_style);
}

static void
_adg_property_line_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress line_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_LINE_FILL;
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
_adg_property_marker1(void)
{
    AdgDimStyle *dim_style;
    AdgMarker *valid_marker, *invalid_marker, *marker;

    dim_style = adg_dim_style_new();
    valid_marker = ADG_MARKER(adg_arrow_new());
    invalid_marker = adg_test_invalid_pointer();

    /* Using the public APIs */
    adg_dim_style_set_marker1(dim_style, valid_marker);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    adg_dim_style_set_marker1(dim_style, invalid_marker);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    adg_dim_style_set_marker1(dim_style, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert_null(marker);

    /* Using GObject property methods */
    g_object_set(dim_style, "marker1", valid_marker, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    g_object_set(dim_style, "marker1", invalid_marker, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    g_object_set(dim_style, "marker1", NULL, NULL);
    marker = adg_dim_style_marker1_new(dim_style);
    g_assert_null(marker);

    g_object_unref(dim_style);
    adg_entity_destroy(ADG_ENTITY(valid_marker));
}

static void
_adg_property_marker2(void)
{
    AdgDimStyle *dim_style;
    AdgMarker *valid_marker, *invalid_marker, *marker;

    dim_style = adg_dim_style_new();
    valid_marker = ADG_MARKER(adg_arrow_new());
    invalid_marker = adg_test_invalid_pointer();

    /* Using the public APIs */
    adg_dim_style_set_marker2(dim_style, valid_marker);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    adg_dim_style_set_marker2(dim_style, invalid_marker);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    adg_dim_style_set_marker2(dim_style, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert_null(marker);

    /* Using GObject property methods */
    g_object_set(dim_style, "marker2", valid_marker, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    g_object_set(dim_style, "marker2", invalid_marker, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert_nonnull(marker);
    adg_entity_destroy(ADG_ENTITY(marker));

    g_object_set(dim_style, "marker2", NULL, NULL);
    marker = adg_dim_style_marker2_new(dim_style);
    g_assert_null(marker);

    g_object_unref(dim_style);
    adg_entity_destroy(ADG_ENTITY(valid_marker));
}

static void
_adg_property_max_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress max_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_FONT_QUOTE_ANNOTATION;
    valid_dress_2 = ADG_DRESS_FONT_ANNOTATION;
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
_adg_property_min_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress min_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_FONT_QUOTE_TEXT;
    valid_dress_2 = ADG_DRESS_FONT_QUOTE_ANNOTATION;
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
_adg_property_number_format(void)
{
    AdgDimStyle *dim_style;
    const gchar *valid_text_1, *valid_text_2;
    const gchar *number_format;
    gchar *number_format_dup;

    dim_style = adg_dim_style_new();
    valid_text_1 = "%lf";
    valid_text_2 = "%abc%";

    /* Check default value */
    number_format = adg_dim_style_get_number_format(dim_style);
    g_assert_cmpstr(number_format, ==, "%-.7g");

    /* Using the public APIs */
    adg_dim_style_set_number_format(dim_style, valid_text_1);
    number_format = adg_dim_style_get_number_format(dim_style);
    g_assert_cmpstr(number_format, ==, valid_text_1);

    adg_dim_style_set_number_format(dim_style, valid_text_2);
    number_format = adg_dim_style_get_number_format(dim_style);
    g_assert_cmpstr(number_format, ==, valid_text_2);

    adg_dim_style_set_number_format(dim_style, NULL);
    number_format = adg_dim_style_get_number_format(dim_style);
    g_assert_null(number_format);

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
    g_assert_null(number_format_dup);

    g_object_unref(dim_style);
}

static void
_adg_property_number_arguments(void)
{
    AdgDimStyle *dim_style;
    const gchar *valid_text_1, *valid_text_2;
    const gchar *number_arguments;
    gchar *number_arguments_dup;

    dim_style = adg_dim_style_new();
    valid_text_1 = "aieDdMmSs";
    valid_text_2 = "";

    /* Check default value */
    number_arguments = adg_dim_style_get_number_arguments(dim_style);
    g_assert_cmpstr(number_arguments, ==, "d");

    /* Using the public APIs */
    adg_dim_style_set_number_arguments(dim_style, valid_text_1);
    number_arguments = adg_dim_style_get_number_arguments(dim_style);
    g_assert_cmpstr(number_arguments, ==, valid_text_1);

    adg_dim_style_set_number_arguments(dim_style, "invalid");
    number_arguments = adg_dim_style_get_number_arguments(dim_style);
    g_assert_cmpstr(number_arguments, ==, valid_text_1);

    adg_dim_style_set_number_arguments(dim_style, " ");
    number_arguments = adg_dim_style_get_number_arguments(dim_style);
    g_assert_cmpstr(number_arguments, ==, valid_text_1);

    adg_dim_style_set_number_arguments(dim_style, valid_text_2);
    number_arguments = adg_dim_style_get_number_arguments(dim_style);
    g_assert_cmpstr(number_arguments, ==, valid_text_2);

    adg_dim_style_set_number_arguments(dim_style, NULL);
    number_arguments = adg_dim_style_get_number_arguments(dim_style);
    g_assert_null(number_arguments);

    /* Using GObject property methods */
    g_object_set(dim_style, "number-arguments", valid_text_1, NULL);
    g_object_get(dim_style, "number-arguments", &number_arguments_dup, NULL);
    g_assert_cmpstr(number_arguments_dup, ==, valid_text_1);
    g_free(number_arguments_dup);

    g_object_set(dim_style, "number-arguments", "invalid", NULL);
    g_object_get(dim_style, "number-arguments", &number_arguments_dup, NULL);
    g_assert_cmpstr(number_arguments_dup, ==, valid_text_1);
    g_free(number_arguments_dup);

    g_object_set(dim_style, "number-arguments", " ", NULL);
    g_object_get(dim_style, "number-arguments", &number_arguments_dup, NULL);
    g_assert_cmpstr(number_arguments_dup, ==, valid_text_1);
    g_free(number_arguments_dup);

    g_object_set(dim_style, "number-arguments", valid_text_2, NULL);
    g_object_get(dim_style, "number-arguments", &number_arguments_dup, NULL);
    g_assert_cmpstr(number_arguments_dup, ==, valid_text_2);
    g_free(number_arguments_dup);

    g_object_set(dim_style, "number-arguments", NULL, NULL);
    g_object_get(dim_style, "number-arguments", &number_arguments_dup, NULL);
    g_assert_null(number_arguments_dup);

    g_object_unref(dim_style);
}

static void
_adg_property_number_tag(void)
{
    AdgDimStyle *dim_style;
    const gchar *valid_text_1, *valid_text_2;
    const gchar *number_tag;
    gchar *number_tag_dup;

    dim_style = adg_dim_style_new();
    valid_text_1 = "";
    valid_text_2 = "<è>";

    /* Check default value */
    number_tag = adg_dim_style_get_number_tag(dim_style);
    g_assert_cmpstr(number_tag, ==, "<>");

    /* Using the public APIs */
    adg_dim_style_set_number_tag(dim_style, valid_text_1);
    number_tag = adg_dim_style_get_number_tag(dim_style);
    g_assert_cmpstr(number_tag, ==, valid_text_1);

    adg_dim_style_set_number_tag(dim_style, valid_text_2);
    number_tag = adg_dim_style_get_number_tag(dim_style);
    g_assert_cmpstr(number_tag, ==, valid_text_2);

    adg_dim_style_set_number_tag(dim_style, NULL);
    number_tag = adg_dim_style_get_number_tag(dim_style);
    g_assert_null(number_tag);

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
    g_assert_null(number_tag_dup);

    g_object_unref(dim_style);
}

static void
_adg_property_decimals(void)
{
    AdgDimStyle *dim_style;
    gint decimals;

    dim_style = adg_dim_style_new();

    /* Check default value */
    decimals = adg_dim_style_get_decimals(dim_style);
    g_assert_cmpint(decimals, ==, 2);

    /* Using the public APIs */
    adg_dim_style_set_decimals(dim_style, 4);
    decimals = adg_dim_style_get_decimals(dim_style);
    g_assert_cmpint(decimals, ==, 4);

    adg_dim_style_set_decimals(dim_style, -2);
    decimals = adg_dim_style_get_decimals(dim_style);
    g_assert_cmpint(decimals, ==, 4);

    decimals = adg_dim_style_get_decimals(NULL);
    g_assert_cmpint(decimals, ==, -2);

    adg_dim_style_set_decimals(dim_style, -1);
    decimals = adg_dim_style_get_decimals(dim_style);
    g_assert_cmpint(decimals, ==, -1);

    /* Using GObject property methods */
    g_object_set(dim_style, "decimals", 2, NULL);
    g_object_get(dim_style, "decimals", &decimals, NULL);
    g_assert_cmpint(decimals, ==, 2);

    g_object_set(dim_style, "decimals", -2, NULL);
    g_object_get(dim_style, "decimals", &decimals, NULL);
    g_assert_cmpint(decimals, ==, 2);

    g_object_set(dim_style, "decimals", -1, NULL);
    g_object_get(dim_style, "decimals", &decimals, NULL);
    g_assert_cmpint(decimals, ==, -1);

    g_object_unref(dim_style);
}

static void
_adg_property_rounding(void)
{
    AdgDimStyle *dim_style;
    gint rounding;

    dim_style = adg_dim_style_new();

    /* Check default value */
    rounding = adg_dim_style_get_rounding(dim_style);
    g_assert_cmpint(rounding, ==, 6);

    /* Using the public APIs */
    adg_dim_style_set_rounding(dim_style, 4);
    rounding = adg_dim_style_get_rounding(dim_style);
    g_assert_cmpint(rounding, ==, 4);

    adg_dim_style_set_rounding(dim_style, -2);
    rounding = adg_dim_style_get_rounding(dim_style);
    g_assert_cmpint(rounding, ==, 4);

    rounding = adg_dim_style_get_rounding(NULL);
    g_assert_cmpint(rounding, ==, -2);

    adg_dim_style_set_rounding(dim_style, -1);
    rounding = adg_dim_style_get_rounding(dim_style);
    g_assert_cmpint(rounding, ==, -1);

    /* Using GObject property methods */
    g_object_set(dim_style, "rounding", 2, NULL);
    g_object_get(dim_style, "rounding", &rounding, NULL);
    g_assert_cmpint(rounding, ==, 2);

    g_object_set(dim_style, "rounding", -2, NULL);
    g_object_get(dim_style, "rounding", &rounding, NULL);
    g_assert_cmpint(rounding, ==, 2);

    g_object_set(dim_style, "rounding", -1, NULL);
    g_object_get(dim_style, "rounding", &rounding, NULL);
    g_assert_cmpint(rounding, ==, -1);

    g_object_unref(dim_style);
}

static void
_adg_property_quote_shift(void)
{
    AdgDimStyle *dim_style;
    CpmlPair null_shift, identity_shift;
    const CpmlPair *shift;
    CpmlPair *shift_dup;

    dim_style = adg_dim_style_new();
    null_shift.x = 0;
    null_shift.y = 0;
    identity_shift.x = 1;
    identity_shift.y = 1;

    /* Using the public APIs */
    adg_dim_style_set_quote_shift(dim_style, &identity_shift);
    shift = adg_dim_style_get_quote_shift(dim_style);
    g_assert_true(cpml_pair_equal(shift, &identity_shift));

    adg_dim_style_set_quote_shift(dim_style, &null_shift);
    shift = adg_dim_style_get_quote_shift(dim_style);
    g_assert_true(cpml_pair_equal(shift, &null_shift));

    adg_dim_style_set_quote_shift(dim_style, NULL);
    shift = adg_dim_style_get_quote_shift(dim_style);
    g_assert_true(cpml_pair_equal(shift, &null_shift));

    /* Using GObject property methods */
    g_object_set(dim_style, "quote-shift", &identity_shift, NULL);
    g_object_get(dim_style, "quote-shift", &shift_dup, NULL);
    g_assert_true(cpml_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "quote-shift", NULL, NULL);
    g_object_get(dim_style, "quote-shift", &shift_dup, NULL);
    g_assert_true(cpml_pair_equal(shift_dup, &identity_shift));
    g_free(shift_dup);

    g_object_set(dim_style, "quote-shift", &null_shift, NULL);
    g_object_get(dim_style, "quote-shift", &shift_dup, NULL);
    g_assert_true(cpml_pair_equal(shift_dup, &null_shift));
    g_free(shift_dup);

    g_object_unref(dim_style);
}

static void
_adg_property_to_offset(void)
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
    adg_assert_isapprox(to_offset, valid_to_offset_1);

    adg_dim_style_set_to_offset(dim_style, invalid_to_offset);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    adg_assert_isapprox(to_offset, valid_to_offset_1);

    adg_dim_style_set_to_offset(dim_style, valid_to_offset_2);
    to_offset = adg_dim_style_get_to_offset(dim_style);
    adg_assert_isapprox(to_offset, valid_to_offset_2);

    /* Using GObject property methods */
    g_object_set(dim_style, "to-offset", valid_to_offset_1, NULL);
    g_object_get(dim_style, "to-offset", &to_offset, NULL);
    adg_assert_isapprox(to_offset, valid_to_offset_1);

    g_object_set(dim_style, "to-offset", invalid_to_offset, NULL);
    g_object_get(dim_style, "to-offset", &to_offset, NULL);
    adg_assert_isapprox(to_offset, valid_to_offset_1);

    g_object_set(dim_style, "to-offset", valid_to_offset_2, NULL);
    g_object_get(dim_style, "to-offset", &to_offset, NULL);
    adg_assert_isapprox(to_offset, valid_to_offset_2);

    g_object_unref(dim_style);
}

static void
_adg_property_value_dress(void)
{
    AdgDimStyle *dim_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress value_dress;

    dim_style = adg_dim_style_new();
    valid_dress_1 = ADG_DRESS_FONT;
    valid_dress_2 = ADG_DRESS_FONT_QUOTE_ANNOTATION;
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

static void
_adg_method_convert(void)
{
    AdgDimStyle *dim_style;
    gdouble value;

    dim_style = adg_dim_style_new();
    adg_dim_style_set_decimals(dim_style, 0);

    /* Sanity check */
    g_assert_false(adg_dim_style_convert(NULL, &value, 'a'));
    g_assert_false(adg_dim_style_convert(dim_style, NULL, 'a'));
    g_assert_false(adg_dim_style_convert(NULL, NULL, 'a'));

    /* Checking the various conversions. For reference:
     * - 'a': the raw @value, i.e. no conversion is performed;
     * - 'i': the raw number of minutes, i.e. the fractional part of @value x 60
     * - 'e': the raw number of seconds, i.e. the fractional part of the raw
     *        number of minutes x 60
     * - 'D': the truncated value of the raw value ('a');
     * - 'd': the rounded value of the raw value ('a');
     * - 'M': the truncated value of the raw number of minutes ('i');
     * - 'm': the rounded value of the raw number of minutes ('i');
     * - 'S': the truncated value of the raw number of seconds ('e');
     * - 's': the rounded value of the raw number of seconds ('e');
     */
    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'a'));
    adg_assert_isapprox(value, 5.678);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'i'));
    adg_assert_isapprox(value, 40.68);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'e'));
    adg_assert_isapprox(value, 40.8);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'D'));
    adg_assert_isapprox(value, 5);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'd'));
    adg_assert_isapprox(value, 6);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'M'));
    adg_assert_isapprox(value, 40);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'm'));
    adg_assert_isapprox(value, 41);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'S'));
    adg_assert_isapprox(value, 40);

    value = 5.678;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 's'));
    adg_assert_isapprox(value, 41);

    /* Checking that the round property rounds */
    value = 59.999;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'D'));
    adg_assert_isapprox(value, 59);

    adg_dim_style_set_rounding(dim_style, 3);

    value = 59.999;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'D'));
    adg_assert_isapprox(value, 59);

    adg_dim_style_set_rounding(dim_style, 2);

    value = 59.999;
    g_assert_true(adg_dim_style_convert(dim_style, &value, 'D'));
    adg_assert_isapprox(value, 60);
    g_object_unref(dim_style);
}

static void
_adg_method_clone(void)
{
    AdgDimStyle *dim_style, *clone;
    AdgMarker *org, *dup;

    dim_style = adg_dim_style_new();
    clone = ADG_DIM_STYLE(adg_style_clone(ADG_STYLE(dim_style)));


    /* Check that first markers are complatible */
    org = adg_dim_style_marker1_new(dim_style);
    dup = adg_dim_style_marker1_new(clone);
    g_assert_cmpuint(adg_marker_get_n_segment(org), ==, adg_marker_get_n_segment(org));
    adg_assert_isapprox(adg_marker_get_pos(org), adg_marker_get_pos(dup));
    adg_assert_isapprox(adg_marker_get_size(org), adg_marker_get_size(dup));
    g_object_unref(org);
    g_object_unref(dup);


    /* Check that second markers are complatible */
    org = adg_dim_style_marker2_new(dim_style);
    dup = adg_dim_style_marker2_new(clone);
    g_assert_cmpuint(adg_marker_get_n_segment(org), ==, adg_marker_get_n_segment(org));
    adg_assert_isapprox(adg_marker_get_pos(org), adg_marker_get_pos(dup));
    adg_assert_isapprox(adg_marker_get_size(org), adg_marker_get_size(dup));
    g_object_unref(org);
    g_object_unref(dup);


    g_object_unref(dim_style);
    g_object_unref(clone);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/dim-style/type/object", ADG_TYPE_DIM_STYLE);

    g_test_add_func("/adg/dim-style/property/baseline-spacing", _adg_property_baseline_spacing);
    g_test_add_func("/adg/dim-style/property/beyond", _adg_property_beyond);
    g_test_add_func("/adg/dim-style/property/color-dress", _adg_property_color_dress);
    g_test_add_func("/adg/dim-style/property/from-offset", _adg_property_from_offset);
    g_test_add_func("/adg/dim-style/property/limits-shift", _adg_property_limits_shift);
    g_test_add_func("/adg/dim-style/property/limits-spacing", _adg_property_limits_spacing);
    g_test_add_func("/adg/dim-style/property/line-dress", _adg_property_line_dress);
    g_test_add_func("/adg/dim-style/property/marker1", _adg_property_marker1);
    g_test_add_func("/adg/dim-style/property/marker2", _adg_property_marker2);
    g_test_add_func("/adg/dim-style/property/max-dress", _adg_property_max_dress);
    g_test_add_func("/adg/dim-style/property/min-dress", _adg_property_min_dress);
    g_test_add_func("/adg/dim-style/property/number-format", _adg_property_number_format);
    g_test_add_func("/adg/dim-style/property/number-arguments", _adg_property_number_arguments);
    g_test_add_func("/adg/dim-style/property/number-tag", _adg_property_number_tag);
    g_test_add_func("/adg/dim-style/property/decimals", _adg_property_decimals);
    g_test_add_func("/adg/dim-style/property/rounding", _adg_property_rounding);
    g_test_add_func("/adg/dim-style/property/quote-shift", _adg_property_quote_shift);
    g_test_add_func("/adg/dim-style/property/to-offset", _adg_property_to_offset);
    g_test_add_func("/adg/dim-style/property/value-dress", _adg_property_value_dress);

    g_test_add_func("/adg/dim-style/method/convert", _adg_method_convert);
    g_test_add_func("/adg/dim-style/method/clone", _adg_method_clone);

    return g_test_run();
}
