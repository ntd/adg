/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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
_adg_alignment_factor(void)
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
    g_assert(cpml_pair_equal(factor, &null_factor));

    /* Using the public APIs */
    adg_alignment_set_factor(alignment, &identity_factor);
    factor = adg_alignment_get_factor(alignment);
    g_assert(cpml_pair_equal(factor, &identity_factor));

    adg_alignment_set_factor_explicit(alignment, 0, 0);
    factor = adg_alignment_get_factor(alignment);
    g_assert(cpml_pair_equal(factor, &null_factor));

    adg_alignment_set_factor(alignment, NULL);
    factor = adg_alignment_get_factor(alignment);
    g_assert(cpml_pair_equal(factor, &null_factor));

    /* Using GObject property methods */
    g_object_set(alignment, "factor", &identity_factor, NULL);
    g_object_get(alignment, "factor", &factor_dup, NULL);
    g_assert(cpml_pair_equal(factor_dup, &identity_factor));
    g_free(factor_dup);

    g_object_set(alignment, "factor", NULL, NULL);
    g_object_get(alignment, "factor", &factor_dup, NULL);
    g_assert(cpml_pair_equal(factor_dup, &identity_factor));
    g_free(factor_dup);

    g_object_set(alignment, "factor", &null_factor, NULL);
    g_object_get(alignment, "factor", &factor_dup, NULL);
    g_assert(cpml_pair_equal(factor_dup, &null_factor));
    g_free(factor_dup);

    g_object_unref(alignment);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/alignment/factor", _adg_alignment_factor);

    return g_test_run();
}
