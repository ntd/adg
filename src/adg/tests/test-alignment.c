/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
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
_adg_property_factor(void)
{
    AdgAlignment *alignment;
    CpmlPair null_factor, identity_factor;
    const CpmlPair *factor;
    CpmlPair *factor_dup;

    alignment = adg_alignment_new(NULL);
    null_factor.x = 0;
    null_factor.y = 0;
    identity_factor.x = 1;
    identity_factor.y = 1;

    /* By default, the alignment should be initialized with a null factor */
    factor = adg_alignment_get_factor(alignment);
    g_assert_true(cpml_pair_equal(factor, &null_factor));

    /* Using the public APIs */
    adg_alignment_set_factor(alignment, &identity_factor);
    factor = adg_alignment_get_factor(alignment);
    g_assert_true(cpml_pair_equal(factor, &identity_factor));

    adg_alignment_set_factor_explicit(alignment, 0, 0);
    factor = adg_alignment_get_factor(alignment);
    g_assert_true(cpml_pair_equal(factor, &null_factor));

    adg_alignment_set_factor(alignment, NULL);
    factor = adg_alignment_get_factor(alignment);
    g_assert_true(cpml_pair_equal(factor, &null_factor));

    /* Using GObject property methods */
    g_object_set(alignment, "factor", &identity_factor, NULL);
    g_object_get(alignment, "factor", &factor_dup, NULL);
    g_assert_true(cpml_pair_equal(factor_dup, &identity_factor));
    g_free(factor_dup);

    g_object_set(alignment, "factor", NULL, NULL);
    g_object_get(alignment, "factor", &factor_dup, NULL);
    g_assert_true(cpml_pair_equal(factor_dup, &identity_factor));
    g_free(factor_dup);

    g_object_set(alignment, "factor", &null_factor, NULL);
    g_object_get(alignment, "factor", &factor_dup, NULL);
    g_assert_true(cpml_pair_equal(factor_dup, &null_factor));
    g_free(factor_dup);

    g_object_unref(alignment);
}


int
main(int argc, char *argv[])
{
    AdgAlignment *alignment;

    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/alignment/type/object", ADG_TYPE_ALIGNMENT);
    adg_test_add_entity_checks("/adg/alignment/type/entity", ADG_TYPE_ALIGNMENT);
    adg_test_add_container_checks("/adg/alignment/type/container", ADG_TYPE_ALIGNMENT);

    alignment = adg_alignment_new_explicit(0.5, 0.5);
    adg_container_add(ADG_CONTAINER(alignment), ADG_ENTITY(adg_logo_new()));
    adg_test_add_global_space_checks("/adg/alignment/behavior/global-space", alignment);
    alignment = adg_alignment_new_explicit(0.5, 0.5);
    adg_container_add(ADG_CONTAINER(alignment), ADG_ENTITY(adg_logo_new()));
    adg_test_add_local_space_checks("/adg/alignment/behavior/local-space", alignment);

    g_test_add_func("/adg/alignment/property/factor", _adg_property_factor);

    return g_test_run();
}
