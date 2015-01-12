/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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
_adg_cell_padding(void)
{
    AdgTableStyle *table_style;
    CpmlPair null_padding, identity_padding;
    const CpmlPair *padding;
    CpmlPair *padding_dup;

    table_style = adg_table_style_new();
    null_padding.x = 0;
    null_padding.y = 0;
    identity_padding.x = 1;
    identity_padding.y = 1;

    /* Using the public APIs */
    adg_table_style_set_cell_padding(table_style, &identity_padding);
    padding = adg_table_style_get_cell_padding(table_style);
    g_assert(cpml_pair_equal(padding, &identity_padding));

    adg_table_style_set_cell_padding(table_style, &null_padding);
    padding = adg_table_style_get_cell_padding(table_style);
    g_assert(cpml_pair_equal(padding, &null_padding));

    adg_table_style_set_cell_padding(table_style, NULL);
    padding = adg_table_style_get_cell_padding(table_style);
    g_assert(cpml_pair_equal(padding, &null_padding));

    /* Using GObject property methods */
    g_object_set(table_style, "cell-padding", &identity_padding, NULL);
    g_object_get(table_style, "cell-padding", &padding_dup, NULL);
    g_assert(cpml_pair_equal(padding_dup, &identity_padding));
    g_free(padding_dup);

    g_object_set(table_style, "cell-padding", NULL, NULL);
    g_object_get(table_style, "cell-padding", &padding_dup, NULL);
    g_assert(cpml_pair_equal(padding_dup, &identity_padding));
    g_free(padding_dup);

    g_object_set(table_style, "cell-padding", &null_padding, NULL);
    g_object_get(table_style, "cell-padding", &padding_dup, NULL);
    g_assert(cpml_pair_equal(padding_dup, &null_padding));
    g_free(padding_dup);

    g_object_unref(table_style);
}

static void
_adg_cell_spacing(void)
{
    AdgTableStyle *table_style;
    CpmlPair null_spacing, identity_spacing;
    const CpmlPair *spacing;
    CpmlPair *spacing_dup;

    table_style = adg_table_style_new();
    null_spacing.x = 0;
    null_spacing.y = 0;
    identity_spacing.x = 1;
    identity_spacing.y = 1;

    /* Using the public APIs */
    adg_table_style_set_cell_spacing(table_style, &identity_spacing);
    spacing = adg_table_style_get_cell_spacing(table_style);
    g_assert(cpml_pair_equal(spacing, &identity_spacing));

    adg_table_style_set_cell_spacing(table_style, &null_spacing);
    spacing = adg_table_style_get_cell_spacing(table_style);
    g_assert(cpml_pair_equal(spacing, &null_spacing));

    adg_table_style_set_cell_spacing(table_style, NULL);
    spacing = adg_table_style_get_cell_spacing(table_style);
    g_assert(cpml_pair_equal(spacing, &null_spacing));

    /* Using GObject property methods */
    g_object_set(table_style, "cell-spacing", &identity_spacing, NULL);
    g_object_get(table_style, "cell-spacing", &spacing_dup, NULL);
    g_assert(cpml_pair_equal(spacing_dup, &identity_spacing));
    g_free(spacing_dup);

    g_object_set(table_style, "cell-spacing", NULL, NULL);
    g_object_get(table_style, "cell-spacing", &spacing_dup, NULL);
    g_assert(cpml_pair_equal(spacing_dup, &identity_spacing));
    g_free(spacing_dup);

    g_object_set(table_style, "cell-spacing", &null_spacing, NULL);
    g_object_get(table_style, "cell-spacing", &spacing_dup, NULL);
    g_assert(cpml_pair_equal(spacing_dup, &null_spacing));
    g_free(spacing_dup);

    g_object_unref(table_style);
}

static void
_adg_color_dress(void)
{
    AdgTableStyle *table_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress color_dress;

    table_style = adg_table_style_new();
    valid_dress_1 = ADG_DRESS_COLOR;
    valid_dress_2 = ADG_DRESS_COLOR_STROKE;
    incompatible_dress = ADG_DRESS_FILL;

    /* Using the public APIs */
    adg_table_style_set_color_dress(table_style, valid_dress_1);
    color_dress = adg_table_style_get_color_dress(table_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_table_style_set_color_dress(table_style, incompatible_dress);
    color_dress = adg_table_style_get_color_dress(table_style);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    adg_table_style_set_color_dress(table_style, valid_dress_2);
    color_dress = adg_table_style_get_color_dress(table_style);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(table_style, "color-dress", valid_dress_1, NULL);
    g_object_get(table_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(table_style, "color-dress", incompatible_dress, NULL);
    g_object_get(table_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_1);

    g_object_set(table_style, "color-dress", valid_dress_2, NULL);
    g_object_get(table_style, "color-dress", &color_dress, NULL);
    g_assert_cmpint(color_dress, ==, valid_dress_2);

    g_object_unref(table_style);
}

static void
_adg_frame_dress(void)
{
    AdgTableStyle *table_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress frame_dress;

    table_style = adg_table_style_new();
    valid_dress_1 = ADG_DRESS_LINE_DIMENSION;
    valid_dress_2 = ADG_DRESS_LINE_STROKE;
    incompatible_dress = ADG_DRESS_FONT;

    /* Using the public APIs */
    adg_table_style_set_frame_dress(table_style, valid_dress_1);
    frame_dress = adg_table_style_get_frame_dress(table_style);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    adg_table_style_set_frame_dress(table_style, incompatible_dress);
    frame_dress = adg_table_style_get_frame_dress(table_style);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    adg_table_style_set_frame_dress(table_style, valid_dress_2);
    frame_dress = adg_table_style_get_frame_dress(table_style);
    g_assert_cmpint(frame_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(table_style, "frame-dress", valid_dress_1, NULL);
    g_object_get(table_style, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    g_object_set(table_style, "frame-dress", incompatible_dress, NULL);
    g_object_get(table_style, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    g_object_set(table_style, "frame-dress", valid_dress_2, NULL);
    g_object_get(table_style, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_2);

    g_object_unref(table_style);
}

static void
_adg_grid_dress(void)
{
    AdgTableStyle *table_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress grid_dress;

    table_style = adg_table_style_new();
    valid_dress_1 = ADG_DRESS_LINE;
    valid_dress_2 = ADG_DRESS_LINE_DIMENSION;
    incompatible_dress = ADG_DRESS_FONT;

    /* Using the public APIs */
    adg_table_style_set_grid_dress(table_style, valid_dress_1);
    grid_dress = adg_table_style_get_grid_dress(table_style);
    g_assert_cmpint(grid_dress, ==, valid_dress_1);

    adg_table_style_set_grid_dress(table_style, incompatible_dress);
    grid_dress = adg_table_style_get_grid_dress(table_style);
    g_assert_cmpint(grid_dress, ==, valid_dress_1);

    adg_table_style_set_grid_dress(table_style, valid_dress_2);
    grid_dress = adg_table_style_get_grid_dress(table_style);
    g_assert_cmpint(grid_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(table_style, "grid-dress", valid_dress_1, NULL);
    g_object_get(table_style, "grid-dress", &grid_dress, NULL);
    g_assert_cmpint(grid_dress, ==, valid_dress_1);

    g_object_set(table_style, "grid-dress", incompatible_dress, NULL);
    g_object_get(table_style, "grid-dress", &grid_dress, NULL);
    g_assert_cmpint(grid_dress, ==, valid_dress_1);

    g_object_set(table_style, "grid-dress", valid_dress_2, NULL);
    g_object_get(table_style, "grid-dress", &grid_dress, NULL);
    g_assert_cmpint(grid_dress, ==, valid_dress_2);

    g_object_unref(table_style);
}

static void
_adg_row_height(void)
{
    AdgTableStyle *table_style;
    gdouble valid_row_height_1, valid_row_height_2, invalid_row_height;
    gdouble row_height;

    table_style = adg_table_style_new();
    valid_row_height_1 = 0;
    valid_row_height_2 = 999;
    invalid_row_height = -1;

    /* Using the public APIs */
    adg_table_style_set_row_height(table_style, valid_row_height_1);
    row_height = adg_table_style_get_row_height(table_style);
    g_assert_cmpfloat(row_height, ==, valid_row_height_1);

    adg_table_style_set_row_height(table_style, invalid_row_height);
    row_height = adg_table_style_get_row_height(table_style);
    g_assert_cmpfloat(row_height, ==, valid_row_height_1);

    adg_table_style_set_row_height(table_style, valid_row_height_2);
    row_height = adg_table_style_get_row_height(table_style);
    g_assert_cmpfloat(row_height, ==, valid_row_height_2);

    /* Using GObject property methods */
    g_object_set(table_style, "row-height", valid_row_height_1, NULL);
    g_object_get(table_style, "row-height", &row_height, NULL);
    g_assert_cmpfloat(row_height, ==, valid_row_height_1);

    g_object_set(table_style, "row-height", invalid_row_height, NULL);
    g_object_get(table_style, "row-height", &row_height, NULL);
    g_assert_cmpfloat(row_height, ==, valid_row_height_1);

    g_object_set(table_style, "row-height", valid_row_height_2, NULL);
    g_object_get(table_style, "row-height", &row_height, NULL);
    g_assert_cmpfloat(row_height, ==, valid_row_height_2);

    g_object_unref(table_style);
}

static void
_adg_title_dress(void)
{
    AdgTableStyle *table_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress title_dress;

    table_style = adg_table_style_new();
    valid_dress_1 = ADG_DRESS_FONT_QUOTE_TEXT;
    valid_dress_2 = ADG_DRESS_FONT_TEXT;
    incompatible_dress = ADG_DRESS_LINE_FILL;

    /* Using the public APIs */
    adg_table_style_set_title_dress(table_style, valid_dress_1);
    title_dress = adg_table_style_get_title_dress(table_style);
    g_assert_cmpint(title_dress, ==, valid_dress_1);

    adg_table_style_set_title_dress(table_style, incompatible_dress);
    title_dress = adg_table_style_get_title_dress(table_style);
    g_assert_cmpint(title_dress, ==, valid_dress_1);

    adg_table_style_set_title_dress(table_style, valid_dress_2);
    title_dress = adg_table_style_get_title_dress(table_style);
    g_assert_cmpint(title_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(table_style, "title-dress", valid_dress_1, NULL);
    g_object_get(table_style, "title-dress", &title_dress, NULL);
    g_assert_cmpint(title_dress, ==, valid_dress_1);

    g_object_set(table_style, "title-dress", incompatible_dress, NULL);
    g_object_get(table_style, "title-dress", &title_dress, NULL);
    g_assert_cmpint(title_dress, ==, valid_dress_1);

    g_object_set(table_style, "title-dress", valid_dress_2, NULL);
    g_object_get(table_style, "title-dress", &title_dress, NULL);
    g_assert_cmpint(title_dress, ==, valid_dress_2);

    g_object_unref(table_style);
}

static void
_adg_value_dress(void)
{
    AdgTableStyle *table_style;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress value_dress;

    table_style = adg_table_style_new();
    valid_dress_1 = ADG_DRESS_FONT_TEXT;
    valid_dress_2 = ADG_DRESS_FONT_QUOTE_ANNOTATION;
    incompatible_dress = ADG_DRESS_LINE;

    /* Using the public APIs */
    adg_table_style_set_value_dress(table_style, valid_dress_1);
    value_dress = adg_table_style_get_value_dress(table_style);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    adg_table_style_set_value_dress(table_style, incompatible_dress);
    value_dress = adg_table_style_get_value_dress(table_style);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    adg_table_style_set_value_dress(table_style, valid_dress_2);
    value_dress = adg_table_style_get_value_dress(table_style);
    g_assert_cmpint(value_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(table_style, "value-dress", valid_dress_1, NULL);
    g_object_get(table_style, "value-dress", &value_dress, NULL);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    g_object_set(table_style, "value-dress", incompatible_dress, NULL);
    g_object_get(table_style, "value-dress", &value_dress, NULL);
    g_assert_cmpint(value_dress, ==, valid_dress_1);

    g_object_set(table_style, "value-dress", valid_dress_2, NULL);
    g_object_get(table_style, "value-dress", &value_dress, NULL);
    g_assert_cmpint(value_dress, ==, valid_dress_2);

    g_object_unref(table_style);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/table-style/cell-padding", _adg_cell_padding);
    adg_test_add_func("/adg/table-style/cell-spacing", _adg_cell_spacing);
    adg_test_add_func("/adg/table-style/color-dress", _adg_color_dress);
    adg_test_add_func("/adg/table-style/frame-dress", _adg_frame_dress);
    adg_test_add_func("/adg/table-style/grid-dress", _adg_grid_dress);
    adg_test_add_func("/adg/table-style/row-height", _adg_row_height);
    adg_test_add_func("/adg/table-style/title-dress", _adg_title_dress);
    adg_test_add_func("/adg/table-style/value-dress", _adg_value_dress);

    return g_test_run();
}
