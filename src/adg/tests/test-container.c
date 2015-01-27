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
_adg_property_child(void)
{
    AdgContainer *container;
    AdgEntity *valid_entity, *invalid_entity;
    GSList *children;

    container = adg_container_new();
    valid_entity = ADG_ENTITY(adg_logo_new());
    invalid_entity = adg_test_invalid_pointer();

    /* Keep alive the entity between all the tests */
    g_object_ref(valid_entity);

    /* Using the public APIs */
    adg_container_add(container, NULL);
    children = adg_container_children(container);
    g_assert_null(children);

    adg_container_add(container, invalid_entity);
    children = adg_container_children(container);
    g_assert_null(children);

    adg_container_add(container, valid_entity);
    children = adg_container_children(container);
    g_assert_nonnull(children);
    g_assert_true(children->data == valid_entity);
    g_slist_free(children);

    adg_container_remove(container, invalid_entity);
    children = adg_container_children(container);
    g_assert_nonnull(children);
    g_assert_true(children->data == valid_entity);
    g_slist_free(children);

    adg_container_remove(container, valid_entity);
    children = adg_container_children(container);
    g_assert_null(children);

    /* Using GObject property methods */
    g_object_set(container, "child", NULL, NULL);
    children = adg_container_children(container);
    g_assert_null(children);

    g_object_set(container, "child", invalid_entity, NULL);
    children = adg_container_children(container);
    g_assert_null(children);

    g_object_set(container, "child", valid_entity, NULL);
    children = adg_container_children(container);
    g_assert_nonnull(children);
    g_assert_true(children->data == valid_entity);
    g_slist_free(children);

    adg_container_remove(container, valid_entity);
    children = adg_container_children(container);
    g_assert_null(children);

    adg_entity_destroy(ADG_ENTITY(container));
    adg_entity_destroy(valid_entity);
}

static void
_adg_misc(void)
{
    AdgContainer *container;
    AdgEntity *entity1, *entity2;
    GSList *children;

    container = adg_container_new();
    entity1 = ADG_ENTITY(adg_toy_text_new("Testing..."));
    entity2 = ADG_ENTITY(adg_title_block_new());

    children = adg_container_children(container);
    g_assert_null(children);

    adg_container_add(container, entity1);
    children = adg_container_children(container);
    g_assert_nonnull(children);
    g_assert_cmpint(g_slist_length(children), ==, 1);
    g_slist_free(children);

    adg_container_add(container, entity2);
    children = adg_container_children(container);
    g_assert_nonnull(children);
    g_assert_cmpint(g_slist_length(children), ==, 2);
    g_slist_free(children);

    adg_entity_destroy(entity1);
    children = adg_container_children(container);
    g_assert_nonnull(children);
    g_assert_cmpint(g_slist_length(children), ==, 1);
    g_slist_free(children);

    adg_entity_destroy(entity2);
    children = adg_container_children(container);
    g_assert_null(children);

    adg_entity_destroy(ADG_ENTITY(container));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/container/type/object", ADG_TYPE_CONTAINER);

    adg_test_add_func("/adg/container/property/child", _adg_property_child);

    adg_test_add_func("/adg/container/misc", _adg_misc);

    return g_test_run();
}
