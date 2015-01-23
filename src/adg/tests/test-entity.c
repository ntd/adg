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
#include <adg.h>


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
    g_assert_true(parent == valid_container);

    adg_entity_set_parent(entity, invalid_container);
    parent = adg_entity_get_parent(entity);
    g_assert_true(parent == valid_container);

    adg_entity_set_parent(entity, NULL);
    parent = adg_entity_get_parent(entity);
    g_assert_null(parent);

    /* Using GObject property methods */
    g_object_set(entity, "parent", valid_container, NULL);
    g_object_get(entity, "parent", &parent, NULL);
    g_assert_true(parent == valid_container);
    adg_entity_destroy(parent);

    g_object_set(entity, "parent", invalid_container, NULL);
    g_object_get(entity, "parent", &parent, NULL);
    g_assert_true(parent == valid_container);
    adg_entity_destroy(parent);

    g_object_set(entity, "parent", NULL, NULL);
    g_object_get(entity, "parent", &parent, NULL);
    g_assert_null(parent);

    adg_entity_destroy(entity);
    adg_entity_destroy(valid_container);
}

static void
_adg_test_global_map(void)
{
    AdgEntity *entity;
    const cairo_matrix_t *identity_map;
    cairo_matrix_t null_map, dummy_map;
    const cairo_matrix_t *global_map;
    cairo_matrix_t *global_map_dup;

    entity = ADG_ENTITY(adg_logo_new());
    identity_map = adg_matrix_identity();

    /* A null map is a kind of degenerated matrix: it must be
     * treated as valid value by the API */
    cairo_matrix_init(&null_map, 0, 0, 0, 0, 0, 0);
    /* A general purpose map value without translations */
    cairo_matrix_init(&dummy_map, 1, 2, 3, 4, 0, 0);

    /* Using the public APIs */
    adg_entity_set_global_map(entity, &null_map);
    global_map = adg_entity_get_global_map(entity);
    g_assert_true(adg_matrix_equal(global_map, &null_map));

    adg_entity_transform_global_map(entity, &dummy_map, ADG_TRANSFORM_AFTER);
    global_map = adg_entity_get_global_map(entity);
    g_assert_true(adg_matrix_equal(global_map, &null_map));

    adg_entity_set_global_map(entity, identity_map);
    global_map = adg_entity_get_global_map(entity);
    g_assert_true(adg_matrix_equal(global_map, identity_map));

    adg_entity_set_global_map(entity, NULL);
    global_map = adg_entity_get_global_map(entity);
    g_assert_true(adg_matrix_equal(global_map, identity_map));

    adg_entity_transform_global_map(entity, &dummy_map, ADG_TRANSFORM_BEFORE);
    global_map = adg_entity_get_global_map(entity);
    g_assert_true(adg_matrix_equal(global_map, &dummy_map));

    /* Using GObject property methods */
    g_object_set(entity, "global-map", &null_map, NULL);
    g_object_get(entity, "global-map", &global_map_dup, NULL);
    g_assert_true(adg_matrix_equal(global_map_dup, &null_map));
    g_free(global_map_dup);

    g_object_set(entity, "global-map", NULL, NULL);
    g_object_get(entity, "global-map", &global_map_dup, NULL);
    g_assert_true(adg_matrix_equal(global_map_dup, &null_map));
    g_free(global_map_dup);

    g_object_set(entity, "global-map", identity_map, NULL);
    g_object_get(entity, "global-map", &global_map_dup, NULL);
    g_assert_true(adg_matrix_equal(global_map_dup, identity_map));
    g_free(global_map_dup);

    adg_entity_destroy(entity);
}

static void
_adg_test_local_map(void)
{
    AdgEntity *entity;
    const cairo_matrix_t *identity_map;
    cairo_matrix_t null_map, dummy_map;
    const cairo_matrix_t *local_map;
    cairo_matrix_t *local_map_dup;

    entity = ADG_ENTITY(adg_logo_new());
    identity_map = adg_matrix_identity();

    /* A null map is a kind of degenerated matrix: it must be
     * treated as valid value by the API */
    cairo_matrix_init(&null_map, 0, 0, 0, 0, 0, 0);
    /* A general purpose map value without translations */
    cairo_matrix_init(&dummy_map, 1, 2, 3, 4, 0, 0);

    /* Using the public APIs */
    adg_entity_set_local_map(entity, &null_map);
    local_map = adg_entity_get_local_map(entity);
    g_assert_true(adg_matrix_equal(local_map, &null_map));

    adg_entity_transform_local_map(entity, &dummy_map, ADG_TRANSFORM_AFTER);
    local_map = adg_entity_get_local_map(entity);
    g_assert_true(adg_matrix_equal(local_map, &null_map));

    adg_entity_set_local_map(entity, identity_map);
    local_map = adg_entity_get_local_map(entity);
    g_assert_true(adg_matrix_equal(local_map, identity_map));

    adg_entity_set_local_map(entity, NULL);
    local_map = adg_entity_get_local_map(entity);
    g_assert_true(adg_matrix_equal(local_map, identity_map));

    adg_entity_transform_local_map(entity, &dummy_map, ADG_TRANSFORM_BEFORE);
    local_map = adg_entity_get_local_map(entity);
    g_assert_true(adg_matrix_equal(local_map, &dummy_map));

    /* Using GObject property methods */
    g_object_set(entity, "local-map", &null_map, NULL);
    g_object_get(entity, "local-map", &local_map_dup, NULL);
    g_assert_true(adg_matrix_equal(local_map_dup, &null_map));
    g_free(local_map_dup);

    g_object_set(entity, "local-map", NULL, NULL);
    g_object_get(entity, "local-map", &local_map_dup, NULL);
    g_assert_true(adg_matrix_equal(local_map_dup, &null_map));
    g_free(local_map_dup);

    g_object_set(entity, "local-map", identity_map, NULL);
    g_object_get(entity, "local-map", &local_map_dup, NULL);
    g_assert_true(adg_matrix_equal(local_map_dup, identity_map));
    g_free(local_map_dup);

    adg_entity_destroy(entity);
}

static void
_adg_test_local_mix(void)
{
    AdgEntity *entity;
    AdgMix valid_mix1, valid_mix2, invalid_mix;
    AdgMix local_mix;

    entity = ADG_ENTITY(adg_logo_new());
    valid_mix1 = ADG_MIX_UNDEFINED;
    valid_mix2 = ADG_MIX_ANCESTORS_NORMALIZED;
    invalid_mix = G_MAXINT;

    /* Using the public APIs */
    adg_entity_set_local_mix(entity, valid_mix1);
    local_mix = adg_entity_get_local_mix(entity);
    g_assert_cmpint(local_mix, ==, valid_mix1);

    adg_entity_set_local_mix(entity, invalid_mix);
    local_mix = adg_entity_get_local_mix(entity);
    g_assert_cmpint(local_mix, !=, invalid_mix);

    adg_entity_set_local_mix(entity, valid_mix2);
    local_mix = adg_entity_get_local_mix(entity);
    g_assert_cmpint(local_mix, ==, valid_mix2);

    /* Using GObject property methods */
    g_object_set(entity, "local-mix", valid_mix1, NULL);
    g_object_get(entity, "local-mix", &local_mix, NULL);
    g_assert_cmpint(local_mix, ==, valid_mix1);

    g_object_set(entity, "local-mix", invalid_mix, NULL);
    g_object_get(entity, "local-mix", &local_mix, NULL);
    g_assert_cmpint(local_mix, ==, valid_mix1);

    g_object_set(entity, "local-mix", valid_mix2, NULL);
    g_object_get(entity, "local-mix", &local_mix, NULL);
    g_assert_cmpint(local_mix, ==, valid_mix2);

    adg_entity_destroy(entity);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/entity/parent", _adg_test_parent);
    adg_test_add_func("/adg/entity/global-map", _adg_test_global_map);
    adg_test_add_func("/adg/entity/local-map", _adg_test_local_map);
    adg_test_add_func("/adg/entity/local-mix", _adg_test_local_mix);

    return g_test_run();
}
