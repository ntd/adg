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
_adg_test_model(void)
{
    AdgMarker *marker;
    AdgModel *valid_model, *invalid_model, *model;

    marker = ADG_MARKER(adg_arrow_new());
    valid_model = ADG_MODEL(adg_path_new());
    invalid_model = adg_test_invalid_pointer();

    g_object_ref(valid_model);

    /* Using the public APIs */
    adg_marker_set_model(marker, NULL);
    model = adg_marker_get_model(marker);
    g_assert(model == NULL);

    adg_marker_set_model(marker, valid_model);
    model = adg_marker_get_model(marker);
    g_assert(model == valid_model);

    adg_marker_set_model(marker, invalid_model);
    model = adg_marker_get_model(marker);
    g_assert(model == valid_model);

    adg_marker_set_model(marker, NULL);
    model = adg_marker_get_model(marker);
    g_assert(model == NULL);

    /* Using GObject property methods */
    g_object_set(marker, "model", NULL, NULL);
    g_object_get(marker, "model", &model, NULL);
    g_assert(model == NULL);

    g_object_set(marker, "model", valid_model, NULL);
    g_object_get(marker, "model", &model, NULL);
    g_assert(model == valid_model);
    g_object_unref(model);

    g_object_set(marker, "model", invalid_model, NULL);
    g_object_get(marker, "model", &model, NULL);
    g_assert(model == valid_model);
    g_object_unref(model);

    g_object_set(marker, "model", NULL, NULL);
    g_object_get(marker, "model", &model, NULL);
    g_assert(model == NULL);

    adg_entity_destroy(ADG_ENTITY(marker));
    g_object_unref(valid_model);
}

static void
_adg_test_n_segment(void)
{
    AdgMarker *marker;
    AdgTrail *trail;
    guint valid_n_segment, invalid_n_segment, n_segment;

    marker = ADG_MARKER(adg_arrow_new());
    trail = ADG_TRAIL(adg_path_new());
    valid_n_segment = 1;
    invalid_n_segment = 2;

    /* Define a segment in "trail" */
    adg_path_move_to_explicit(ADG_PATH(trail), 1, 2);
    adg_path_line_to_explicit(ADG_PATH(trail), 3, 4);

    adg_marker_set_trail(marker, trail);

    /* Using the public APIs */
    adg_marker_set_n_segment(marker, valid_n_segment);
    n_segment = adg_marker_get_n_segment(marker);
    g_assert_cmpint(n_segment, ==, valid_n_segment);

    adg_marker_set_n_segment(marker, invalid_n_segment);
    n_segment = adg_marker_get_n_segment(marker);
    g_assert_cmpint(n_segment, ==, valid_n_segment);

    adg_marker_set_n_segment(marker, 0);
    n_segment = adg_marker_get_n_segment(marker);
    g_assert_cmpint(n_segment, ==, 0);
    g_assert_cmpint(n_segment, !=, valid_n_segment);

    /* Using GObject property methods */
    g_object_set(marker, "n-segment", valid_n_segment, NULL);
    g_object_get(marker, "n-segment", &n_segment, NULL);
    g_assert_cmpint(n_segment, ==, valid_n_segment);

    g_object_set(marker, "n-segment", invalid_n_segment, NULL);
    g_object_get(marker, "n-segment", &n_segment, NULL);
    g_assert_cmpint(n_segment, ==, valid_n_segment);

    adg_entity_destroy(ADG_ENTITY(marker));
    g_object_unref(trail);
}

static void
_adg_test_pos(void)
{
    AdgMarker *marker;
    gdouble valid_pos1, valid_pos2, invalid_pos, pos;

    marker = ADG_MARKER(adg_arrow_new());
    valid_pos1 = 1;
    valid_pos2 = 0;
    invalid_pos = -1;

    /* Using the public APIs */
    adg_marker_set_pos(marker, valid_pos1);
    pos = adg_marker_get_pos(marker);
    g_assert_cmpfloat(pos, ==, valid_pos1);

    adg_marker_set_pos(marker, invalid_pos);
    pos = adg_marker_get_pos(marker);
    g_assert_cmpfloat(pos, ==, valid_pos1);

    adg_marker_set_pos(marker, valid_pos2);
    pos = adg_marker_get_pos(marker);
    g_assert_cmpfloat(pos, ==, valid_pos2);

    /* Using GObject property methods */
    g_object_set(marker, "pos", valid_pos1, NULL);
    g_object_get(marker, "pos", &pos, NULL);
    g_assert_cmpfloat(pos, ==, valid_pos1);

    g_object_set(marker, "pos", invalid_pos, NULL);
    g_object_get(marker, "pos", &pos, NULL);
    g_assert_cmpfloat(pos, ==, valid_pos1);

    g_object_set(marker, "pos", valid_pos2, NULL);
    g_object_get(marker, "pos", &pos, NULL);
    g_assert_cmpfloat(pos, ==, valid_pos2);

    adg_entity_destroy(ADG_ENTITY(marker));
}

static void
_adg_test_trail(void)
{
    AdgMarker *marker;
    AdgTrail *valid_trail, *invalid_trail, *trail;

    marker = ADG_MARKER(adg_arrow_new());
    valid_trail = ADG_TRAIL(adg_path_new());
    invalid_trail = adg_test_invalid_pointer();

    /* Using the public APIs */
    adg_marker_set_trail(marker, NULL);
    trail = adg_marker_get_trail(marker);
    g_assert(trail == NULL);

    adg_marker_set_n_segment(marker, 1);
    adg_marker_set_trail(marker, valid_trail);
    trail = adg_marker_get_trail(marker);
    g_assert(trail == NULL);

    adg_marker_set_n_segment(marker, 0);
    adg_marker_set_trail(marker, valid_trail);
    trail = adg_marker_get_trail(marker);
    g_assert(trail == valid_trail);

    adg_marker_set_trail(marker, invalid_trail);
    trail = adg_marker_get_trail(marker);
    g_assert(trail == valid_trail);

    adg_marker_set_trail(marker, NULL);
    trail = adg_marker_get_trail(marker);
    g_assert(trail == NULL);

    /* Using GObject property methods */
    g_object_set(marker, "trail", NULL, NULL);
    g_object_get(marker, "trail", &trail, NULL);
    g_assert(trail == NULL);

    g_object_set(marker, "n-segment", 1, "trail", valid_trail, NULL);
    trail = adg_marker_get_trail(marker);
    g_assert(trail == NULL);

    g_object_set(marker, "n-segment", 0, "trail", valid_trail, NULL);
    g_object_get(marker, "trail", &trail, NULL);
    g_assert(trail == valid_trail);
    g_object_unref(trail);

    g_object_set(marker, "trail", invalid_trail, NULL);
    g_object_get(marker, "trail", &trail, NULL);
    g_assert(trail == valid_trail);
    g_object_unref(trail);

    g_object_set(marker, "trail", NULL, NULL);
    g_object_get(marker, "trail", &trail, NULL);
    g_assert(trail == NULL);

    adg_entity_destroy(ADG_ENTITY(marker));
    g_object_unref(valid_trail);
}

static void
_adg_test_size(void)
{
    AdgMarker *marker;
    gdouble valid_size1, valid_size2, invalid_size, size;

    marker = ADG_MARKER(adg_arrow_new());
    valid_size1 = 10;
    valid_size2 = 0;
    invalid_size = -1;

    /* Using the public APIs */
    adg_marker_set_size(marker, valid_size1);
    size = adg_marker_get_size(marker);
    g_assert_cmpfloat(size, ==, valid_size1);

    adg_marker_set_size(marker, invalid_size);
    size = adg_marker_get_size(marker);
    g_assert_cmpfloat(size, ==, valid_size1);

    adg_marker_set_size(marker, valid_size2);
    size = adg_marker_get_size(marker);
    g_assert_cmpfloat(size, ==, valid_size2);

    /* Using GObject property methods */
    g_object_set(marker, "size", valid_size1, NULL);
    g_object_get(marker, "size", &size, NULL);
    g_assert_cmpfloat(size, ==, valid_size1);

    g_object_set(marker, "size", invalid_size, NULL);
    g_object_get(marker, "size", &size, NULL);
    g_assert_cmpfloat(size, ==, valid_size1);

    g_object_set(marker, "size", valid_size2, NULL);
    g_object_get(marker, "size", &size, NULL);
    g_assert_cmpfloat(size, ==, valid_size2);

    adg_entity_destroy(ADG_ENTITY(marker));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/marker/model", _adg_test_model);
    adg_test_add_func("/adg/marker/n-segment", _adg_test_n_segment);
    adg_test_add_func("/adg/marker/pos", _adg_test_pos);
    adg_test_add_func("/adg/marker/size", _adg_test_size);
    adg_test_add_func("/adg/marker/trail", _adg_test_trail);

    return g_test_run();
}
