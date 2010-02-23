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
_adg_test_parent(void)
{
    AdgEntity *entity;
    AdgEntity *valid_container, *invalid_container;
    AdgEntity *parent;

    entity = ADG_ENTITY(adg_logo_new());
    valid_container = ADG_ENTITY(adg_container_new());
    invalid_container = adg_test_invalid_pointer();

    /* Using the public APIs */
    adg_entity_set_parent(entity, valid_container);
    parent = adg_entity_get_parent(entity);
    g_assert(parent == valid_container);

    adg_entity_set_parent(entity, invalid_container);
    parent = adg_entity_get_parent(entity);
    g_assert(parent == valid_container);

    adg_entity_set_parent(entity, NULL);
    parent = adg_entity_get_parent(entity);
    g_assert(parent == NULL);

    /* Using GObject property methods */
    g_object_set(entity, "parent", valid_container, NULL);
    g_object_get(entity, "parent", &parent, NULL);
    g_assert(parent == valid_container);

    g_object_set(entity, "parent", invalid_container, NULL);
    g_object_get(entity, "parent", &parent, NULL);
    g_assert(parent == valid_container);

    g_object_set(entity, "parent", NULL, NULL);
    g_object_get(entity, "parent", &parent, NULL);
    g_assert(parent == NULL);

    g_object_unref(entity);
    g_object_unref(valid_container);
}

static void
_adg_test_global_map(void)
{
    AdgEntity *entity;
    const AdgMatrix *identity_map;
    AdgMatrix null_map;
    const AdgMatrix *global_map;

    entity = ADG_ENTITY(adg_logo_new());
    identity_map = adg_matrix_identity();

    /* A null map is a kind of degenerated matrix: it must be
     * treated as valid value by the API */
    cairo_matrix_init(&null_map, 0, 0, 0, 0, 0, 0);

    /* Using the public APIs */
    adg_entity_set_global_map(entity, &null_map);
    global_map = adg_entity_get_global_map(entity);
    g_assert(adg_matrix_equal(global_map, &null_map));

    adg_entity_set_global_map(entity, NULL);
    global_map = adg_entity_get_global_map(entity);
    g_assert(adg_matrix_equal(global_map, &null_map));

    adg_entity_set_global_map(entity, identity_map);
    global_map = adg_entity_get_global_map(entity);
    g_assert(adg_matrix_equal(global_map, identity_map));

    /* Using GObject property methods */
    g_object_set(entity, "global-map", &null_map, NULL);
    g_object_get(entity, "global-map", &global_map, NULL);
    g_assert(adg_matrix_equal(global_map, &null_map));

    g_object_set(entity, "global-map", NULL, NULL);
    g_object_get(entity, "global-map", &global_map, NULL);
    g_assert(adg_matrix_equal(global_map, &null_map));

    g_object_set(entity, "global-map", identity_map, NULL);
    g_object_get(entity, "global-map", &global_map, NULL);
    g_assert(adg_matrix_equal(global_map, identity_map));

    g_object_unref(entity);
}

static void
_adg_test_local_map(void)
{
    AdgEntity *entity;
    const AdgMatrix *identity_map;
    AdgMatrix null_map;
    const AdgMatrix *local_map;

    entity = ADG_ENTITY(adg_logo_new());
    identity_map = adg_matrix_identity();

    /* A null map is a kind of degenerated matrix: it must be
     * treated as valid value by the API */
    cairo_matrix_init(&null_map, 0, 0, 0, 0, 0, 0);

    /* Using the public APIs */
    adg_entity_set_local_map(entity, &null_map);
    local_map = adg_entity_get_local_map(entity);
    g_assert(adg_matrix_equal(local_map, &null_map));

    adg_entity_set_local_map(entity, NULL);
    local_map = adg_entity_get_local_map(entity);
    g_assert(adg_matrix_equal(local_map, &null_map));

    adg_entity_set_local_map(entity, identity_map);
    local_map = adg_entity_get_local_map(entity);
    g_assert(adg_matrix_equal(local_map, identity_map));

    /* Using GObject property methods */
    g_object_set(entity, "local-map", &null_map, NULL);
    g_object_get(entity, "local-map", &local_map, NULL);
    g_assert(adg_matrix_equal(local_map, &null_map));

    g_object_set(entity, "local-map", NULL, NULL);
    g_object_get(entity, "local-map", &local_map, NULL);
    g_assert(adg_matrix_equal(local_map, &null_map));

    g_object_set(entity, "local-map", identity_map, NULL);
    g_object_get(entity, "local-map", &local_map, NULL);
    g_assert(adg_matrix_equal(local_map, identity_map));

    g_object_unref(entity);
}

static void
_adg_test_local_method(void)
{
    AdgEntity *entity;
    AdgMixMethod valid_method1, valid_method2, invalid_method;
    AdgMixMethod local_method;

    entity = ADG_ENTITY(adg_logo_new());
    valid_method1 = ADG_MIX_UNDEFINED;
    valid_method2 = ADG_MIX_ANCESTORS_NORMALIZED;
    invalid_method = G_MAXINT;

    /* Using the public APIs */
    adg_entity_set_local_method(entity, valid_method1);
    local_method = adg_entity_get_local_method(entity);
    g_assert_cmpint(local_method, ==, valid_method1);

    adg_entity_set_local_method(entity, invalid_method);
    local_method = adg_entity_get_local_method(entity);
    g_assert_cmpint(local_method, !=, invalid_method);

    adg_entity_set_local_method(entity, valid_method2);
    local_method = adg_entity_get_local_method(entity);
    g_assert_cmpint(local_method, ==, valid_method2);

    /* Using GObject property methods */
    g_object_set(entity, "local-method", valid_method1, NULL);
    g_object_get(entity, "local-method", &local_method, NULL);
    g_assert_cmpint(local_method, ==, valid_method1);

    g_object_set(entity, "local-method", invalid_method, NULL);
    g_object_get(entity, "local-method", &local_method, NULL);
    g_assert_cmpint(local_method, ==, valid_method1);

    g_object_set(entity, "local-method", valid_method2, NULL);
    g_object_get(entity, "local-method", &local_method, NULL);
    g_assert_cmpint(local_method, ==, valid_method2);

    g_object_unref(entity);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/entity/parent", _adg_test_parent);
    adg_test_add_func("/adg/entity/global-map", _adg_test_global_map);
    adg_test_add_func("/adg/entity/local-map", _adg_test_local_map);
    adg_test_add_func("/adg/entity/local-method", _adg_test_local_method);

    return g_test_run();
}
