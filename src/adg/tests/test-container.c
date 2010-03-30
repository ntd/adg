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
_adg_container_child(void)
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
    g_assert(children == NULL);

    adg_container_add(container, invalid_entity);
    children = adg_container_children(container);
    g_assert(children == NULL);

    adg_container_add(container, valid_entity);
    children = adg_container_children(container);
    g_assert(children != NULL);
    g_assert(children->data == valid_entity);
    g_slist_free(children);

    adg_container_remove(container, invalid_entity);
    children = adg_container_children(container);
    g_assert(children != NULL);
    g_assert(children->data == valid_entity);
    g_slist_free(children);

    adg_container_remove(container, valid_entity);
    children = adg_container_children(container);
    g_assert(children == NULL);

    /* Using GObject property methods */
    g_object_set(container, "child", NULL, NULL);
    children = adg_container_children(container);
    g_assert(children == NULL);

    g_object_set(container, "child", invalid_entity, NULL);
    children = adg_container_children(container);
    g_assert(children == NULL);

    g_object_set(container, "child", valid_entity, NULL);
    children = adg_container_children(container);
    g_assert(children != NULL);
    g_assert(children->data == valid_entity);
    g_slist_free(children);

    adg_container_remove(container, valid_entity);
    children = adg_container_children(container);
    g_assert(children == NULL);

    g_object_unref(container);
    g_object_unref(valid_entity);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/container/child", _adg_container_child);

    return g_test_run();
}
