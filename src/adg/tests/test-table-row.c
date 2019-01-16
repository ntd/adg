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


#include <adg-test.h>
#include <adg.h>


static void
_adg_behavior_misc(void)
{
    AdgTable *table;
    AdgTableRow *row, *row2;
    AdgTableCell *cell;
    const CpmlExtents *extents;
    CpmlExtents layout;
    const CpmlPair *size;

    table = adg_table_new();
    row = adg_table_row_new(table);
    layout.is_defined = 0;

    /* Sanity check */
    g_assert_null(adg_table_row_get_table(NULL));
    g_assert_null(adg_table_row_new_before(NULL));
    g_assert_null(adg_table_row_size_request(NULL));
    g_assert_null(adg_table_row_get_extents(NULL));
    g_assert_null(adg_table_row_arrange(NULL, &layout));
    g_assert_null(adg_table_row_arrange(row, &layout));
    g_assert_null(adg_table_row_arrange(row, NULL));
    adg_table_row_free(NULL);

    g_assert_true(adg_table_row_get_table(row) == table);

    row2 = adg_table_row_new_before(row);
    g_assert_true(adg_table_row_get_table(row2) == table);
    adg_table_row_free(row2);

    extents = adg_table_row_get_extents(row);
    g_assert_nonnull(extents);
    g_assert_false(extents->is_defined);

    size = adg_table_row_size_request(row);
    g_assert_nonnull(size);
    adg_assert_isapprox(size->x, 0);
    adg_assert_isapprox(size->y, 0);

    extents = adg_table_row_get_extents(row);
    g_assert_nonnull(extents);
    g_assert_false(extents->is_defined);

    adg_table_row_set_height(row, 12);

    size = adg_table_row_size_request(row);
    g_assert_nonnull(size);
    adg_assert_isapprox(size->x, 0);
    adg_assert_isapprox(size->y, 12);

    cell = adg_table_cell_new_full(row, 34, "name", "title", FALSE);
    g_assert_nonnull(cell);
    size = adg_table_row_size_request(row);
    g_assert_nonnull(size);
    adg_assert_isapprox(size->x, 34);
    adg_assert_isapprox(size->y, 12);

    layout.is_defined = 1;
    layout.org.x = 12;
    layout.org.y = 34;
    layout.size.x = -56;
    layout.size.y = -78;
    extents = adg_table_row_arrange(row, &layout);
    g_assert_nonnull(extents);
    g_assert_true(extents->is_defined);
    adg_assert_isapprox(extents->org.x, 12);
    adg_assert_isapprox(extents->org.y, 34);
    adg_assert_isapprox(extents->size.x, 34);
    adg_assert_isapprox(extents->size.y, 12);

    layout.size.x = 56;
    extents = adg_table_row_arrange(row, &layout);
    g_assert_nonnull(extents);
    g_assert_true(extents->is_defined);
    adg_assert_isapprox(extents->size.x, 56);
    adg_assert_isapprox(extents->size.y, 12);

    layout.size.x = -1;
    layout.size.y = 78;
    extents = adg_table_row_arrange(row, &layout);
    g_assert_nonnull(extents);
    g_assert_true(extents->is_defined);
    adg_assert_isapprox(extents->size.x, 56);
    adg_assert_isapprox(extents->size.y, 78);

    adg_entity_destroy(ADG_ENTITY(table));
}

static void
_adg_property_height(void)
{
    AdgTable *table;
    AdgTableRow *row;

    /* Sanity check */
    adg_table_row_set_height(NULL, 1);
    adg_table_row_get_height(NULL);

    table = adg_table_new();
    row = adg_table_row_new(table);

    adg_assert_isapprox(adg_table_row_get_height(row), 0);

    adg_table_row_set_height(row, 123);
    adg_assert_isapprox(adg_table_row_get_height(row), 123);

    adg_table_row_set_height(row, 0);
    adg_assert_isapprox(adg_table_row_get_height(row), 0);

    adg_table_row_set_height(row, -123);
    adg_assert_isapprox(adg_table_row_get_height(row), -123);

    adg_entity_destroy(ADG_ENTITY(table));
}


int
main(int argc, char *argv[])
{
    AdgTable *table;
    int result;

    adg_test_init(&argc, &argv);

    /* Create a dummy table, otherwise no row would be instantiable */
    table = adg_table_new();

    adg_test_add_boxed_checks("/adg/table-row/type/boxed", ADG_TYPE_TABLE_ROW,
                              adg_table_row_new(table));

    g_test_add_func("/adg/table-row/behavior/misc", _adg_behavior_misc);

    g_test_add_func("/adg/table-row/property/height", _adg_property_height);

    result = g_test_run();
    adg_entity_destroy(ADG_ENTITY(table));

    return result;
}
