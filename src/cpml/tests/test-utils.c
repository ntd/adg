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
#include <cpml.h>
#include <math.h>


static void
_cpml_behavior_angle(void)
{
    adg_assert_isapprox(0, cpml_angle(0));
    adg_assert_isapprox(0, cpml_angle(2 * M_PI));
    adg_assert_isapprox(M_PI, cpml_angle(M_PI));
    adg_assert_isapprox(M_PI, cpml_angle(3 * M_PI));
    adg_assert_isapprox(M_PI, cpml_angle(-M_PI));
    adg_assert_isapprox(M_PI, cpml_angle(-3 * M_PI));
    adg_assert_isapprox(-M_PI + 1, cpml_angle(-3 * M_PI + 1));
}

static void
_cpml_behavior_angle_distance(void)
{
    adg_assert_isapprox(0, cpml_angle_distance(0, 0));
    adg_assert_isapprox(0, cpml_angle_distance(M_PI, M_PI));
    adg_assert_isapprox(0, cpml_angle_distance(M_PI, 3 * M_PI));
    adg_assert_isapprox(0, cpml_angle_distance(M_PI, -M_PI));
    adg_assert_isapprox(0, cpml_angle_distance(2 * M_PI, 0));
    adg_assert_isapprox(0, cpml_angle_distance(-2 * M_PI, 0));
    adg_assert_isapprox(M_PI, cpml_angle_distance(0, M_PI));
    adg_assert_isapprox(M_PI, cpml_angle_distance(M_PI, 0));
    adg_assert_isapprox(M_PI, cpml_angle_distance(2 * M_PI, M_PI));
    adg_assert_isapprox(M_PI, cpml_angle_distance(0, -M_PI));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/cpml/utils/behavior/angle", _cpml_behavior_angle);
    g_test_add_func("/cpml/utils/behavior/angle-distance", _cpml_behavior_angle_distance);

    return g_test_run();
}
