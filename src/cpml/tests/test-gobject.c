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


#include <adg-test.h>
#include <cpml.h>


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_boxed_checks("/cpml/pair/type/boxed", CPML_TYPE_PAIR, g_new0(CpmlPair, 1));
    adg_test_add_boxed_checks("/cpml/primitive/type/boxed", CPML_TYPE_PRIMITIVE, g_new0(CpmlPrimitive, 1));
    adg_test_add_boxed_checks("/cpml/segment/type/boxed", CPML_TYPE_SEGMENT, g_new0(CpmlSegment, 1));
    adg_test_add_enum_checks("/cpml/curve-offset-algorithm/type/enum", CPML_TYPE_CURVE_OFFSET_ALGORITHM);

    return g_test_run();
}
