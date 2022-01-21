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
_adg_property_local_mix(void)
{
    AdgArrow *arrow;
    AdgEntity *entity;

    /* Check default local mix method */
    arrow = adg_arrow_new();
    entity = (AdgEntity *) arrow;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_PARENT);
    adg_entity_destroy(entity);

    /* Check local mix method overriding */
    arrow = g_object_new(ADG_TYPE_ARROW, "local-mix", ADG_MIX_DISABLED, NULL);
    entity = (AdgEntity *) arrow;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_DISABLED);
    adg_entity_destroy(entity);

    /* Check default mix using GObject methods */
    arrow = g_object_new(ADG_TYPE_ARROW, NULL);
    entity = (AdgEntity *) arrow;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_PARENT);
    adg_entity_destroy(entity);
}

static void
_adg_property_angle(void)
{
    AdgArrow *arrow;
    gdouble valid_value, invalid_value;
    gdouble angle;

    valid_value = -G_PI_2;
    invalid_value = G_PI + 1;
    arrow = adg_arrow_new();

    /* Using the public APIs */
    adg_arrow_set_angle(arrow, valid_value);
    angle = adg_arrow_get_angle(arrow);
    adg_assert_isapprox(angle, valid_value);

    adg_arrow_set_angle(arrow, -G_PI);
    angle = adg_arrow_get_angle(arrow);
    adg_assert_isapprox(angle, G_PI);

    adg_arrow_set_angle(arrow, invalid_value);
    angle = adg_arrow_get_angle(arrow);
    g_assert_cmpfloat(angle, !=, invalid_value);

    /* Using GObject property methods */
    g_object_set(arrow, "angle", valid_value, NULL);
    g_object_get(arrow, "angle", &angle, NULL);
    adg_assert_isapprox(angle, valid_value);

    g_object_set(arrow, "angle", -G_PI, NULL);
    g_object_get(arrow, "angle", &angle, NULL);
    adg_assert_isapprox(angle, G_PI);

    g_object_set(arrow, "angle", invalid_value, NULL);
    g_object_get(arrow, "angle", &angle, NULL);
    g_assert_cmpfloat(angle, !=, invalid_value);

    adg_entity_destroy((AdgEntity *) arrow);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/arrow/type/object", ADG_TYPE_ARROW);
    adg_test_add_entity_checks("/adg/arrow/type/entity", ADG_TYPE_ARROW);

    g_test_add_func("/adg/arrow/property/local-mix", _adg_property_local_mix);
    g_test_add_func("/adg/arrow/property/angle", _adg_property_angle);

    return g_test_run();
}
