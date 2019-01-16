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
#include <cpml.h>


static cairo_path_data_t data[] = {
    { .header = { CPML_MOVE, 2 } },
    { .point = { 1, 2 } },
    { .header = { CPML_LINE, 2 } },
    { .point = { 3, 4 } }
};
static cairo_path_t path = {
    CAIRO_STATUS_SUCCESS,
    data,
    G_N_ELEMENTS(data)
};


static void
_cpml_method_primitive_deep_dup(void)
{
    CpmlSegment segment = { &path, data, G_N_ELEMENTS(data) };
    CpmlPrimitive primitive, *dup_primitive;

    g_assert_null(cpml_primitive_deep_dup(NULL));

    /* Test full duplication */
    primitive.segment = &segment;
    primitive.org = &data[1];
    primitive.data = &data[2];

    dup_primitive = cpml_primitive_deep_dup(&primitive);
    g_assert_nonnull(dup_primitive);
    g_assert_nonnull(dup_primitive->segment);
    g_assert_nonnull(dup_primitive->org);
    g_assert_nonnull(dup_primitive->data);
    g_assert_false(dup_primitive->segment == primitive.segment);
    g_assert_false(dup_primitive->org == primitive.org);
    g_assert_false(dup_primitive->data == primitive.data);
    g_free(dup_primitive);

    /* Test duplication with NULL fields */
    primitive.segment = NULL;

    dup_primitive = cpml_primitive_deep_dup(&primitive);
    g_assert_nonnull(dup_primitive);
    g_assert_null(dup_primitive->segment);
    g_assert_nonnull(dup_primitive->org);
    g_assert_nonnull(dup_primitive->data);
    g_free(dup_primitive);

    primitive.org = NULL;

    dup_primitive = cpml_primitive_deep_dup(&primitive);
    g_assert_nonnull(dup_primitive);
    g_assert_null(dup_primitive->segment);
    g_assert_null(dup_primitive->org);
    g_assert_nonnull(dup_primitive->data);
    g_free(dup_primitive);

    primitive.data = NULL;

    dup_primitive = cpml_primitive_deep_dup(&primitive);
    g_assert_nonnull(dup_primitive);
    g_assert_null(dup_primitive->segment);
    g_assert_null(dup_primitive->org);
    g_assert_null(dup_primitive->data);
    g_free(dup_primitive);
}

static void
_cpml_method_segment_deep_dup(void)
{
    CpmlSegment segment, *dup_segment;

    g_assert_null(cpml_segment_deep_dup(NULL));

    /* Test full duplication */
    segment.path = &path;
    segment.data = data;
    segment.num_data = G_N_ELEMENTS(data);

    dup_segment = cpml_segment_deep_dup(&segment);
    g_assert_nonnull(dup_segment);
    g_assert_null(dup_segment->path);
    g_assert_nonnull(dup_segment->data);
    g_assert_false(dup_segment->data == segment.data);
    g_assert_cmpint(dup_segment->num_data, ==, segment.num_data);
    g_free(dup_segment);

    /* Test duplication with NULL fields */
    segment.path = NULL;

    dup_segment = cpml_segment_deep_dup(&segment);
    g_assert_nonnull(dup_segment);
    g_assert_null(dup_segment->path);
    g_assert_nonnull(dup_segment->data);
    g_assert_false(dup_segment->data == segment.data);
    g_assert_cmpint(dup_segment->num_data, ==, segment.num_data);
    g_free(dup_segment);

    segment.data = NULL;

    dup_segment = cpml_segment_deep_dup(&segment);
    g_assert_nonnull(dup_segment);
    g_assert_cmpint(dup_segment->num_data, ==, 0);
    g_assert_null(dup_segment->data);
    g_free(dup_segment);

    segment.data = data;
    segment.num_data = 0;

    dup_segment = cpml_segment_deep_dup(&segment);
    g_assert_nonnull(dup_segment);
    g_assert_cmpint(dup_segment->num_data, ==, 0);
    g_assert_null(dup_segment->data);
    g_free(dup_segment);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_boxed_checks("/cpml/pair/type/boxed", CPML_TYPE_PAIR, g_new0(CpmlPair, 1));

    adg_test_add_boxed_checks("/cpml/primitive/type/boxed", CPML_TYPE_PRIMITIVE, g_new0(CpmlPrimitive, 1));
    g_test_add_func("/cpml/primitive/method/deep-dup", _cpml_method_primitive_deep_dup);

    adg_test_add_boxed_checks("/cpml/segment/type/boxed", CPML_TYPE_SEGMENT, g_new0(CpmlSegment, 1));
    g_test_add_func("/cpml/segment/method/deep-dup", _cpml_method_segment_deep_dup);

    adg_test_add_enum_checks("/cpml/cpml-primitive-type/type/enum", CPML_TYPE_PRIMITIVE_TYPE);

    adg_test_add_enum_checks("/cpml/curve-offset-algorithm/type/enum", CPML_TYPE_CURVE_OFFSET_ALGORITHM);

    return g_test_run();
}
