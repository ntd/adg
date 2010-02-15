/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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


#include <adg/adg.h>
#include <stdlib.h>


static void
test_named_pair(void)
{
    AdgModel *model;
    AdgPair pair;
    const AdgPair *got_pair;

    model = ADG_MODEL(adg_path_new());
    pair.x = -1234;
    pair.y = 4321;

    adg_model_set_named_pair(model, "Existent", &pair);
    got_pair = adg_model_get_named_pair(model, "Existent");
    g_assert_cmpint(pair.x, ==, got_pair->x);
    g_assert_cmpint(pair.y, ==, got_pair->y);

    got_pair = adg_model_get_named_pair(model, "Not existent");
    g_assert(got_pair == NULL);

    adg_model_set_named_pair(model, "Latin1: àèìòù", &pair);
    got_pair = adg_model_get_named_pair(model, "Latin1: àèìòù");
    g_assert_cmpint(pair.x, ==, got_pair->x);
    g_assert_cmpint(pair.y, ==, got_pair->y);

    g_object_unref(model);
}

static void
test_dependency(void)
{
    AdgModel *model;
    AdgEntity *entity;
    const GSList *dependencies;

    model = ADG_MODEL(adg_path_new());
    entity = ADG_ENTITY(adg_logo_new());

    /* Temporarily disable these test to let make check pass:
     * it is known to fail because of a test framework issue
     * present in glib up to 2.22.4. Here it is the link:
     * http://mail.gnome.org/archives/gtk-app-devel-list/2010-February/msg00045.html
     */
#if 0
    g_object_set(model, "dependency", NULL, NULL);
    dependencies = adg_model_get_dependencies(model);
    g_assert(dependencies == NULL);

    adg_model_add_dependency(model, NULL);
    dependencies = adg_model_get_dependencies(model);
    g_assert(dependencies == NULL);
#endif

    adg_model_add_dependency(model, entity);
    dependencies = adg_model_get_dependencies(model);
    g_assert(dependencies != NULL);
    g_assert(dependencies->data == entity);

    adg_model_remove_dependency(model, entity);
    dependencies = adg_model_get_dependencies(model);
    g_assert(dependencies == NULL);

    g_object_unref(model);
    g_object_unref(entity);
}


int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    g_type_init();

    g_test_add_func("/adg/AdgModel/named-pair", test_named_pair);
    g_test_add_func("/adg/AdgModel/dependency", test_dependency);

    return g_test_run();
}
