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
_adg_test_named_pair(void)
{
    AdgModel *model;
    CpmlPair valid_pair;
    const CpmlPair *named_pair;

    model = ADG_MODEL(adg_path_new());
    valid_pair.x = -1234;
    valid_pair.y = 4321;

    adg_model_set_named_pair(model, "Existent", &valid_pair);
    named_pair = adg_model_get_named_pair(model, "Existent");
    g_assert_true(cpml_pair_equal(named_pair, &valid_pair));

    named_pair = adg_model_get_named_pair(model, "Not existent");
    g_assert_null(named_pair);

    adg_model_set_named_pair(model, "Latin1: àèìòù", &valid_pair);
    named_pair = adg_model_get_named_pair(model, "Latin1: àèìòù");
    g_assert_true(cpml_pair_equal(named_pair, &valid_pair));

    g_object_unref(model);
}

static void
_adg_test_dependency(void)
{
    AdgModel *model;
    AdgEntity *valid_entity, *invalid_entity;
    const GSList *dependencies;

    model = ADG_MODEL(adg_path_new());
    valid_entity = ADG_ENTITY(adg_logo_new());
    invalid_entity = adg_test_invalid_pointer();

    /* There is no need to add a reference to keep valid_entity alive:
     * the AdgModel implementation does not sink the dependencies, so
     * at least one reference will be left */

    /* Using the public APIs */
    adg_model_add_dependency(model, NULL);
    dependencies = adg_model_get_dependencies(model);
    g_assert_null(dependencies);

    adg_model_add_dependency(model, invalid_entity);
    dependencies = adg_model_get_dependencies(model);
    g_assert_null(dependencies);

    adg_model_add_dependency(model, valid_entity);
    dependencies = adg_model_get_dependencies(model);
    g_assert_nonnull(dependencies);
    g_assert_true(dependencies->data == valid_entity);

    adg_model_remove_dependency(model, valid_entity);
    dependencies = adg_model_get_dependencies(model);
    g_assert_null(dependencies);

    /* Using GObject property methods */
    g_object_set(model, "dependency", NULL, NULL);
    dependencies = adg_model_get_dependencies(model);
    g_assert_null(dependencies);

    g_object_set(model, "dependency", invalid_entity, NULL);
    dependencies = adg_model_get_dependencies(model);
    g_assert_null(dependencies);

    g_object_set(model, "dependency", valid_entity, NULL);
    dependencies = adg_model_get_dependencies(model);
    g_assert_nonnull(dependencies);
    g_assert_true(dependencies->data == valid_entity);

    adg_model_remove_dependency(model, valid_entity);
    dependencies = adg_model_get_dependencies(model);
    g_assert_null(dependencies);

    g_object_unref(model);
    adg_entity_destroy(valid_entity);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/model/named-pair", _adg_test_named_pair);
    adg_test_add_func("/adg/model/dependency", _adg_test_dependency);

    return g_test_run();
}
