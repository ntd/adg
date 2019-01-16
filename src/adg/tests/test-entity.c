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

#define ADG_TYPE_DUMMY      (adg_dummy_get_type())


typedef GObject AdgDummy;
typedef GObjectClass AdgDummyClass;

static void
adg_dummy_class_init(AdgDummyClass *klass)
{
}

static void
adg_dummy_init(AdgDummy *dummy)
{
}

G_DEFINE_TYPE(AdgDummy, adg_dummy, ADG_TYPE_ENTITY);


static void
_adg_behavior_misc(void)
{
    AdgDummy *dummy = g_object_new(ADG_TYPE_DUMMY, NULL);
    cairo_t *cr = adg_test_cairo_context();

    /* Ensure NULL virtual methods do not crash the process */
    adg_entity_invalidate(ADG_ENTITY(dummy));
    adg_entity_arrange(ADG_ENTITY(dummy));
    adg_entity_render(ADG_ENTITY(dummy), cr);

    cairo_destroy(cr);
    g_object_unref(dummy);
}

static void
_adg_behavior_style(void)
{
    AdgEntity *entity;
    AdgStyle *style, *color_style, *line_style;

    entity = ADG_ENTITY(adg_logo_new());
    color_style = ADG_STYLE(adg_color_style_new());
    line_style = ADG_STYLE(adg_line_style_new());

    /* Sanity check */
    adg_entity_set_style(NULL, ADG_DRESS_COLOR, color_style);
    g_assert_null(adg_entity_get_style(NULL, ADG_DRESS_COLOR));

    adg_entity_set_style(entity, ADG_DRESS_COLOR, color_style);

    style = adg_entity_get_style(entity, ADG_DRESS_COLOR);
    g_assert_nonnull(style);
    g_assert_true(style == color_style);

    adg_entity_set_style(entity, ADG_DRESS_COLOR, color_style);

    style = adg_entity_get_style(entity, ADG_DRESS_COLOR);
    g_assert_nonnull(style);
    g_assert_true(style == color_style);

    adg_entity_set_style(entity, ADG_DRESS_COLOR, NULL);
    g_assert_null(adg_entity_get_style(NULL, ADG_DRESS_COLOR));

    /* Try to set an incompatible style */
    adg_entity_set_style(entity, ADG_DRESS_COLOR, line_style);

    g_assert_null(adg_entity_get_style(NULL, ADG_DRESS_COLOR));

    adg_entity_destroy(entity);
    g_object_unref(color_style);
    g_object_unref(line_style);
}

static void
_adg_behavior_local(void)
{
    AdgCanvas *canvas;
    AdgEntity *entity;

    /* Sanity check */
    adg_entity_set_local_mix(NULL, ADG_MIX_UNDEFINED);
    g_assert_cmpint(adg_entity_get_local_mix(NULL), ==, ADG_MIX_UNDEFINED);
    adg_entity_local_changed(NULL);

    canvas = adg_test_canvas();
    entity = ADG_ENTITY(adg_logo_new());
    adg_container_add(ADG_CONTAINER(canvas), entity);

    g_assert_cmpint(adg_entity_get_local_mix(entity), !=, ADG_MIX_UNDEFINED);

    /* Check any AdgMix value does not crash the process */
    adg_entity_set_local_mix(entity, ADG_MIX_UNDEFINED);
    adg_entity_local_changed(entity);
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_UNDEFINED);

    adg_entity_set_local_mix(entity, ADG_MIX_DISABLED);
    adg_entity_local_changed(entity);
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_DISABLED);

    adg_entity_set_local_mix(entity, ADG_MIX_NONE);
    adg_entity_local_changed(entity);
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_NONE);

    adg_entity_set_local_mix(entity, ADG_MIX_ANCESTORS);
    adg_entity_local_changed(entity);
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_ANCESTORS);

    adg_entity_set_local_mix(entity, ADG_MIX_ANCESTORS_NORMALIZED);
    adg_entity_local_changed(entity);
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_ANCESTORS_NORMALIZED);

    adg_entity_set_local_mix(entity, ADG_MIX_PARENT);
    adg_entity_local_changed(entity);
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_PARENT);

    adg_entity_set_local_mix(entity, ADG_MIX_PARENT_NORMALIZED);
    adg_entity_local_changed(entity);
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_PARENT_NORMALIZED);

    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_property_floating(void)
{
    AdgEntity *entity;
    gboolean invalid_boolean;
    gboolean floating;

    entity = ADG_ENTITY(adg_logo_new());
    invalid_boolean = (gboolean) 1234;

    /* Ensure the default state is false */
    g_assert_false(adg_entity_has_floating(entity));

    /* Using the public APIs */
    adg_entity_switch_floating(entity, invalid_boolean);
    g_assert_false(adg_entity_has_floating(entity));

    adg_entity_switch_floating(entity, TRUE);
    g_assert_true(adg_entity_has_floating(entity));

    /* Using GObject property methods */
    g_object_set(entity, "floating", FALSE, NULL);
    g_object_get(entity, "floating", &floating, NULL);
    g_assert_false(floating);

    g_object_set(entity, "floating", invalid_boolean, NULL);
    g_object_get(entity, "floating", &floating, NULL);
    g_assert_false(floating);

    g_object_set(entity, "floating", TRUE, NULL);
    g_object_get(entity, "floating", &floating, NULL);
    g_assert_true(floating);

    adg_entity_destroy(entity);
}

static void
_adg_property_parent(void)
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
_adg_property_global_map(void)
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
_adg_property_local_map(void)
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
_adg_property_local_mix(void)
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

static void
_adg_property_extents(void)
{
    AdgEntity *entity;
    CpmlExtents new_extents;
    const CpmlExtents *extents;

    /* Sanity check */
    adg_entity_set_extents(NULL, NULL);
    g_assert_null(adg_entity_get_extents(NULL));

    entity = ADG_ENTITY(adg_logo_new());
    new_extents.is_defined = TRUE;
    new_extents.org.x = 1;
    new_extents.org.y = 2;
    new_extents.size.x = 3;
    new_extents.size.y = 4;

    extents = adg_entity_get_extents(entity);
    g_assert_false(extents->is_defined);

    adg_entity_set_extents(entity, &new_extents);

    extents = adg_entity_get_extents(entity);
    g_assert_true(extents->is_defined);
    adg_assert_isapprox(extents->org.x, 1);
    adg_assert_isapprox(extents->org.y, 2);
    adg_assert_isapprox(extents->size.x, 3);
    adg_assert_isapprox(extents->size.y, 4);

    adg_entity_set_extents(entity, NULL);
    g_assert_false(extents->is_defined);

    adg_entity_destroy(entity);
}

static void
_adg_method_get_canvas(void)
{
    AdgCanvas *canvas, *response;
    AdgEntity *entity;

    /* Sanity check */
    g_assert_null(adg_entity_get_canvas(NULL));

    canvas = adg_test_canvas();
    entity = ADG_ENTITY(adg_logo_new());

    g_assert_null(adg_entity_get_canvas(entity));
    adg_container_add(ADG_CONTAINER(canvas), entity);

    response = adg_entity_get_canvas(entity);
    g_assert_nonnull(response);
    g_assert_true(response == canvas);

    adg_entity_destroy(ADG_ENTITY(canvas));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/entity/type/object", ADG_TYPE_ENTITY);
    adg_test_add_entity_checks("/adg/entity/type/entity", ADG_TYPE_ENTITY);

    g_test_add_func("/adg/entity/behavior/misc", _adg_behavior_misc);
    g_test_add_func("/adg/entity/behavior/style", _adg_behavior_style);
    g_test_add_func("/adg/entity/behavior/local", _adg_behavior_local);

    g_test_add_func("/adg/entity/property/floating", _adg_property_floating);
    g_test_add_func("/adg/entity/property/parent", _adg_property_parent);
    g_test_add_func("/adg/entity/property/global-map", _adg_property_global_map);
    g_test_add_func("/adg/entity/property/local-map", _adg_property_local_map);
    g_test_add_func("/adg/entity/property/local-mix", _adg_property_local_mix);
    g_test_add_func("/adg/entity/property/extents", _adg_property_extents);

    g_test_add_func("/adg/entity/method/get-canvas", _adg_method_get_canvas);

    return g_test_run();
}
