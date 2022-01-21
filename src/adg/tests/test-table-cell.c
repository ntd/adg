/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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
_adg_behavior_misc(void)
{
    AdgTable *table;
    AdgTableRow *row;
    AdgTableCell *cell;

    table = adg_table_new();
    row = adg_table_row_new(table);

    /* Sanity check */
    g_assert_null(adg_table_cell_dup(NULL));
    g_assert_null(adg_table_cell_new(NULL));
    g_assert_null(adg_table_cell_new_before(NULL));
    g_assert_null(adg_table_cell_new_with_width(NULL, 123));
    g_assert_null(adg_table_cell_new_full(NULL, 123, NULL, NULL, FALSE));
    g_assert_null(adg_table_cell_get_row(NULL));
    g_assert_null(adg_table_cell_get_table(NULL));
    adg_table_cell_dispose(NULL);
    adg_table_cell_free(NULL);

    /* Check default constructor */
    cell = adg_table_cell_new(row);
    g_assert_nonnull(cell);
    g_assert_true(adg_table_cell_get_row(cell) == row);
    g_assert_true(adg_table_cell_get_table(cell) == table);

    /* Check dependent construction */
    g_assert_nonnull(adg_table_cell_dup(cell));
    g_assert_nonnull(adg_table_cell_new_before(cell));

    /* Check default destructor: other AdgTableCell instances will be
     * implicitely destroyed when the parent AdgTable is destroyed */
    adg_table_cell_free(cell);

    /* Check alternative valid construction conditions */
    g_assert_nonnull(adg_table_cell_new_with_width(row, 0));
    g_assert_nonnull(adg_table_cell_new_with_width(row, 123));
    g_assert_nonnull(adg_table_cell_new_full(row, 12, "name", "title", FALSE));
    g_assert_nonnull(adg_table_cell_new_full(row, 34, NULL, "title", TRUE));
    g_assert_nonnull(adg_table_cell_new_full(row, 56, "name", NULL, FALSE));
    g_assert_nonnull(adg_table_cell_new_full(row, 78, NULL, NULL, TRUE));
    g_assert_nonnull(adg_table_cell_new_full(row, 0, NULL, NULL, TRUE));

    /* Check invalid conditions */
    g_assert_null(adg_table_cell_new_with_width(row, -1));
    g_assert_null(adg_table_cell_new_full(row, -1, NULL, NULL, TRUE));

    adg_entity_destroy(ADG_ENTITY(table));
}

static void
_adg_property_title(void)
{
    AdgTable *table;
    AdgTableRow *row;
    AdgTableCell *cell;
    AdgEntity *entity;

    table = adg_table_new();
    row = adg_table_row_new(table);
    cell = adg_table_cell_new(row);
    entity = ADG_ENTITY(adg_logo_new());

    /* Sanity check */
    g_assert_null(adg_table_cell_title(NULL));
    adg_table_cell_set_title(NULL, NULL);
    adg_table_cell_set_title(NULL, entity);

    /* A newly created cell should not have any content */
    g_assert_null(adg_table_cell_title(cell));

    /* Check explicit setting */
    adg_table_cell_set_title(cell, entity);
    g_assert_nonnull(adg_table_cell_title(cell));
    g_assert_true(adg_table_cell_title(cell) == entity);

    /* Check explicit unsetting */
    adg_table_cell_set_title(cell, NULL);
    g_assert_null(adg_table_cell_title(cell));

    /* Check implicit setting during construction */
    cell = adg_table_cell_new_full(row, 12, NULL, "title", FALSE);
    g_assert_nonnull(adg_table_cell_title(cell));

    /* Check the content is not set implicitely */
    cell = adg_table_cell_new_full(row, 12, NULL, NULL, FALSE);
    g_assert_null(adg_table_cell_title(cell));

    adg_entity_destroy(ADG_ENTITY(table));
    adg_entity_destroy(entity);
}

static void
_adg_property_value(void)
{
    AdgTable *table;
    AdgTableRow *row;
    AdgTableCell *cell;
    AdgEntity *entity;

    table = adg_table_new();
    row = adg_table_row_new(table);
    cell = adg_table_cell_new(row);
    entity = ADG_ENTITY(adg_logo_new());

    /* Sanity check */
    g_assert_null(adg_table_cell_value(NULL));
    adg_table_cell_set_value(NULL, NULL);
    adg_table_cell_set_value(NULL, entity);

    /* A newly created cell should not have any content */
    g_assert_null(adg_table_cell_value(cell));

    /* Check explicit setting */
    adg_table_cell_set_value(cell, entity);
    g_assert_nonnull(adg_table_cell_value(cell));
    g_assert_true(adg_table_cell_value(cell) == entity);

    /* Check explicit unsetting */
    adg_table_cell_set_value(cell, NULL);
    g_assert_null(adg_table_cell_value(cell));

    /* Check text setting */
    adg_table_cell_set_text_value(cell, "value");
    g_assert_nonnull(adg_table_cell_value(cell));

    /* Check text unsetting */
    adg_table_cell_set_text_value(cell, NULL);
    g_assert_null(adg_table_cell_value(cell));

    adg_entity_destroy(ADG_ENTITY(table));
    adg_entity_destroy(entity);
}

static void
_adg_property_width(void)
{
    AdgTable *table;
    AdgTableRow *row;
    AdgTableCell *cell;

    table = adg_table_new();
    row = adg_table_row_new(table);
    cell = adg_table_cell_new(row);

    /* Sanity check */
    g_assert_cmpint(adg_table_cell_get_width(NULL), ==, 0);
    adg_table_cell_set_width(NULL, 123);

    /* Check explicit setting */
    g_assert_cmpint(adg_table_cell_get_width(cell), ==, 0);
    adg_table_cell_set_width(cell, 321);
    g_assert_cmpint(adg_table_cell_get_width(cell), ==, 321);
    adg_table_cell_set_width(cell, 0);
    g_assert_cmpint(adg_table_cell_get_width(cell), ==, 0);

    /* Check implicit setting during construction */
    cell = adg_table_cell_new_with_width(row, 456);
    g_assert_cmpint(adg_table_cell_get_width(cell), ==, 456);
    cell = adg_table_cell_new_with_width(row, 0);
    g_assert_cmpint(adg_table_cell_get_width(cell), ==, 0);

    adg_entity_destroy(ADG_ENTITY(table));
}

static void
_adg_property_frame(void)
{
    AdgTable *table;
    AdgTableRow *row;
    AdgTableCell *cell;

    table = adg_table_new();
    row = adg_table_row_new(table);
    cell = adg_table_cell_new(row);

    /* Sanity check */
    g_assert_false(adg_table_cell_has_frame(NULL));
    adg_table_cell_switch_frame(NULL, FALSE);

    /* Check setting and unsetting */
    g_assert_false(adg_table_cell_has_frame(cell));
    adg_table_cell_switch_frame(cell, TRUE);
    g_assert_true(adg_table_cell_has_frame(cell));
    adg_table_cell_switch_frame(cell, FALSE);
    g_assert_false(adg_table_cell_has_frame(cell));

    adg_entity_destroy(ADG_ENTITY(table));
}


int
main(int argc, char *argv[])
{
    AdgTable *table;
    AdgTableRow *row;
    int result;

    adg_test_init(&argc, &argv);

    /* Create a dummy table row, otherwise no cell would be instantiable */
    table = adg_table_new();
    row = adg_table_row_new(table);

    adg_test_add_boxed_checks("/adg/table-cell/type/boxed", ADG_TYPE_TABLE_CELL,
                              adg_table_cell_new(row));

    g_test_add_func("/adg/table-cell/behavior/misc", _adg_behavior_misc);
    g_test_add_func("/adg/table-cell/property/title", _adg_property_title);
    g_test_add_func("/adg/table-cell/property/value", _adg_property_value);
    g_test_add_func("/adg/table-cell/property/width", _adg_property_width);
    g_test_add_func("/adg/table-cell/property/frame", _adg_property_frame);

    result = g_test_run();
    adg_entity_destroy(ADG_ENTITY(table));

    return result;
}
