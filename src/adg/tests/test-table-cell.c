/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
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

    result = g_test_run();
    g_object_unref(table);
    return result;
}
