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
test_parent(void)
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


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/adg/entity/parent", test_parent);

    return g_test_run();
}
