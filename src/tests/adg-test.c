/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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


#include <adg/adg-internal.h>
#include <adg/adg-model.h>
#include <adg/adg-trail.h>
#include <adg/adg-path.h>
#include <adg/adg-container.h>
#include <adg/adg-stroke.h>
#include <adg/adg-table.h>
#include <adg/adg-title-block.h>
#include <adg/adg-canvas.h>
#include "adg-test.h"


typedef struct {
    GType type;
    gpointer instance;
} _BoxedData;

typedef struct {
    AdgTrapsFunc func;
    gint n_fragments;
} _TrapsData;

typedef struct {
    gpointer instance;
    gulong handler;
    gboolean flag;
} _SignalData;


/* Using adg_nop() would require to pull in the whole libadg stack:
 * better to replicate that trivial function instead.
 */
static void
_adg_nop(void)
{
}

void
adg_test_init(int *p_argc, char **p_argv[])
{
#if GLIB_CHECK_VERSION(2, 34, 0)
#else
    /* On GLib older than 2.34.0 g_type_init() *must* be called */
    g_type_init();
#endif
    g_test_init(p_argc, p_argv, NULL);

    g_log_set_always_fatal(0);

    /* When building in silent mode (default), the ADG_QUIET
     * environment variable is set to 1 by the Makefile and the
     * warnings are discarded to reduce visual cluttering.
     */
    if (g_getenv("ADG_QUIET") != NULL)
        g_log_set_default_handler((GLogFunc) _adg_nop, NULL);

    g_test_bug_base("https://track.entidi.com/%s");
}

const gpointer
adg_test_invalid_pointer(void)
{
    static int junk[10] = { 0 };
    return junk;
}

cairo_t *
adg_test_cairo_context(void)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    return cr;
}

int
adg_test_cairo_num_data(cairo_t *cr)
{
    cairo_path_t *path = cairo_copy_path(cr);
    int length = path->num_data;
    cairo_path_destroy(path);

    return length;
}

const cairo_path_t *
adg_test_path(void)
{
    static cairo_path_data_t data[] = {

        /* First segment: a valid segment with all primitive types */
        { .header = { CPML_MOVE, 2 }},
        { .point = { 0, 1 }},
        { .header = { CPML_LINE, 2 }},
        { .point = { 3, 1 }},
        { .header = { CPML_ARC, 3 }},
        { .point = { 4, 5 }},
        { .point = { 6, 7 }},
        { .header = { CPML_CURVE, 4 }},
        { .point = { 8, 9 }},
        { .point = { 10, 11 }},
        { .point = { -2, 2 }},
        { .header = { CPML_CLOSE, 1 }},

        /* Useless CPML_MOVE */
        { .header = { CPML_MOVE, 2 }},
        { .point = { 0, 0 }},

        /* Second segment: a couple of lines of length 1 and 2;
         * line 2 intersects line 1 of the first segment in (1, 1) */
        { .header = { CPML_MOVE, 2 }},
        { .point = { 0, 0 }},
        { .header = { CPML_LINE, 2 }},
        { .point = { 1, 0 }},
        { .header = { CPML_LINE, 2 }},
        { .point = { 1, 2 }},

        /* Another useless CPML_MOVE with useless embedded data */
        { .header = { CPML_MOVE, 4 }},
        { .point = { 1, 2 }},
        { .point = { 3, 4 }},
        { .point = { 5, 6 }},

        /* Third segment: a Bézier curve with a trailing CPML_CLOSE */
        { .header = { CPML_MOVE, 2 }},
        { .point = { 10, 13 }},
        { .header = { CPML_CURVE, 4 }},
        { .point = { 8, 9 }},
        { .point = { 10, 11 }},
        { .point = { 12, 13 }},
        { .header = { CPML_CLOSE, 1 }},

        /* A valid cairo segment considered invalid by CPML
         * because does not have a leading CPML_MOVE */
        { .header = { CPML_LINE, 2 }},
        { .point = { 10, 0 }},
        { .header = { CPML_CLOSE, 1 }},

        /* Another valid cairo segment invalid in CPML */
        { .header = { CPML_CLOSE, 1 }},

        /* Forth segment: a couple of arcs */
        { .header = { CPML_MOVE, 2 }},
        { .point = { 14, 15 }},
        { .header = { CPML_ARC, 3 }},
        { .point = { 17, 16 }},
        { .point = { 18, 19 }},
        { .header = { CPML_ARC, 3 }},
        { .point = { 21, 20 }},
        { .point = { 22, 23 }},

        /* Fifth segment: a floating CPML_CLOSE */
        { .header = { CPML_MOVE, 2 }},
        { .point = { 24, 25 }},
        { .header = { CPML_CLOSE, 1 }}
    };
    static cairo_path_t path = {
        CAIRO_STATUS_SUCCESS,
        data,
        G_N_ELEMENTS(data)
    };
    return &path;
}

gpointer
adg_test_canvas(void)
{
    AdgPath *path;
    AdgStroke *stroke;
    AdgCanvas *canvas;

    path = adg_path_new();
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_line_to_explicit(path, 1, 1);
    stroke = adg_stroke_new(ADG_TRAIL(path));
    g_object_unref(path);

    canvas = adg_canvas_new();
    adg_canvas_set_margins(canvas, 0, 0, 0, 0);
    adg_canvas_set_paddings(canvas, 0, 0, 0, 0);

    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(stroke));

    return canvas;
}

static void
_adg_enum_checks(GType *p_type)
{
    GType type = *p_type;
    gpointer class;

    g_free(p_type);

    g_assert_true(G_TYPE_IS_ENUM(type));

    class = g_type_class_ref(type);
    g_assert_nonnull(class);

    g_assert_null(g_enum_get_value(class, -1));
    g_assert_nonnull(g_enum_get_value(class, 0));
    g_assert_null(g_enum_get_value_by_name(class, "unexistent value"));
}

void
adg_test_add_enum_checks(const gchar *testpath, GType type)
{
    GType *p_type = g_new(GType, 1);
    *p_type = type;
    g_test_add_data_func(testpath, p_type, (gpointer) _adg_enum_checks);
}

static void
_adg_boxed_checks(_BoxedData *boxed_data)
{
    gpointer replica;

    g_assert_true(G_TYPE_IS_BOXED(boxed_data->type));

    g_assert_null(g_boxed_copy(boxed_data->type, NULL));

    replica = g_boxed_copy(boxed_data->type, boxed_data->instance);
    g_assert_nonnull(replica);

    g_boxed_free(boxed_data->type, replica);
    g_boxed_free(boxed_data->type, boxed_data->instance);

    g_free(boxed_data);
}

void
adg_test_add_boxed_checks(const gchar *testpath, GType type, gpointer instance)
{
    _BoxedData *boxed_data = g_new(_BoxedData, 1);
    boxed_data->type = type;
    boxed_data->instance = instance;

    g_test_add_data_func(testpath, boxed_data,
                         (gpointer) _adg_boxed_checks);
}

static void
_adg_object_checks(GType *p_type)
{
    GType type = *p_type;

    g_free(p_type);

    g_assert_true(G_TYPE_IS_OBJECT(type));

    if (! G_TYPE_IS_ABSTRACT(type)) {
        gpointer result = GINT_TO_POINTER(0xdead);
        GObject *object = g_object_new(type, NULL);

        g_assert_nonnull(object);
        g_assert_true(G_IS_OBJECT(object));

        /* Convert the floating reference, if present, to a hard reference so
         * we can handle GTK+ widgets (that usually have a floating reference)
         * and other object (that do not usually have floating references) in
         * the same way.
         */
        if (g_object_is_floating(object)) {
            g_object_ref_sink(object);
        }

        /* Check that unknown or unexistent properties does
         * not return values (result should pass unmodified)
         */
        g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);
        g_object_set(object, "unknown", NULL, NULL);
        g_object_get(object, "unknown", &result, NULL);
        g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);
        g_object_get(object, "unexistent", &result, NULL);
        g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);

        /* Check object lifetime: this can be done with a weak pointer
         * that will "nullifies" result after object is destructed
         */
        g_object_add_weak_pointer(object, &result);
        g_object_ref(object);
        g_object_unref(object);
        g_assert_nonnull(result);
        g_object_unref(object);
        g_assert_null(result);

        result = GINT_TO_POINTER(0xdead);
        object = g_object_new(type, NULL);

        g_object_add_weak_pointer(object, &result);
        g_assert_nonnull(result);
        g_object_ref(object);
        g_object_ref(object);
        g_object_run_dispose(object);
        g_assert_null(result);
    }
}

void
adg_test_add_object_checks(const gchar *testpath, GType type)
{
    GType *p_type = g_new(GType, 1);
    *p_type = type;
    g_test_add_data_func(testpath, p_type, (gpointer) _adg_object_checks);
}

static void
_adg_test_set(gboolean *p_flag)
{
    *p_flag = TRUE;
}

static _SignalData signal_data;

void
adg_test_signal(gpointer instance, const gchar *detailed_signal)
{
    signal_data.instance = instance;

    /* The swapped variant *must* be used, otherwise the position of flag
     * would depend on the number of parameters of the signal. This also
     * implies I cannot use _adg_increment() here. */
    signal_data.handler = g_signal_connect_swapped(instance,
                                                   detailed_signal,
                                                   G_CALLBACK(_adg_test_set),
                                                   &signal_data.flag);
    signal_data.flag = FALSE;
}

gboolean
adg_test_signal_check(gboolean disconnect)
{
    gboolean last_flag;

    if (disconnect) {
        /* A handler cannot be disconnected twice */
        g_assert(signal_data.instance != NULL);
        g_signal_handler_disconnect(signal_data.instance, signal_data.handler);
        signal_data.instance = NULL;
    }

    last_flag = signal_data.flag;
    signal_data.flag = FALSE;

    return last_flag;
}

static void
_adg_model_checks(GType *p_type)
{
    GType type = *p_type;

    g_free(p_type);

    g_assert_true(g_type_is_a(type, ADG_TYPE_MODEL));

    if (! G_TYPE_IS_ABSTRACT(type)) {
        AdgModel *model = g_object_new(type, NULL);

        g_assert_nonnull(model);
        g_assert_true(ADG_IS_MODEL(model));

        /* Check the clear signal */
        adg_test_signal(model, "clear");
        adg_model_clear(model);
        g_assert_true(adg_test_signal_check(TRUE));

        /* Check the reset signal */
        adg_test_signal(model, "reset");
        adg_model_reset(model);
        g_assert_true(adg_test_signal_check(TRUE));

        /* Check the reset signal triggers the clear signal */
        adg_test_signal(model, "clear");
        adg_model_reset(model);
        g_assert_true(adg_test_signal_check(TRUE));

        /* Check the changed signal */
        adg_test_signal(model, "changed");
        adg_model_changed(model);
        g_assert_true(adg_test_signal_check(TRUE));

        g_object_unref(model);
    }
}

void
adg_test_add_model_checks(const gchar *testpath, GType type)
{
    GType *p_type = g_new(GType, 1);
    *p_type = type;
    g_test_add_data_func(testpath, p_type, (gpointer) _adg_model_checks);
}

static void
_adg_entity_checks(GType *p_type)
{
    GType type = *p_type;

    g_free(p_type);

    g_assert_true(g_type_is_a(type, ADG_TYPE_ENTITY));

    if (! G_TYPE_IS_ABSTRACT(type)) {
        AdgEntity *entity = g_object_new(type, NULL);
        const CpmlExtents *extents;

        g_assert_nonnull(entity);
        g_assert_true(ADG_IS_ENTITY(entity));

        /* Ensure that the entity extents are not initially defined */
        extents = adg_entity_get_extents(entity);
        g_assert_false(extents->is_defined);

        adg_test_signal(entity, "arrange");
        adg_entity_arrange(entity);
        g_assert_true(adg_test_signal_check(TRUE));

        extents = adg_entity_get_extents(entity);
        if (extents->is_defined) {
            /* RENDERABLE ENTITY */
            /* Check that invalidate() clears the cached extents */
            adg_test_signal(entity, "invalidate");
            adg_entity_invalidate(entity);
            g_assert_true(adg_test_signal_check(TRUE));

            extents = adg_entity_get_extents(entity);
            g_assert_false(extents->is_defined);
        } else {
            /* NON-RENDERABLE ENTITY (e.g., empty container) */
        }

        g_object_unref(entity);
    }
}

void
adg_test_add_entity_checks(const gchar *testpath, GType type)
{
    GType *p_type = g_new(GType, 1);
    *p_type = type;
    g_test_add_data_func(testpath, p_type, (gpointer) _adg_entity_checks);
}

static void
_adg_increment(GObject *object, gint *counter)
{
    ++(*counter);
}

static void
_adg_container_checks(GType *p_type)
{
    GType type = *p_type;

    g_free(p_type);

    g_assert_true(g_type_is_a(type, ADG_TYPE_CONTAINER));

    if (! G_TYPE_IS_ABSTRACT(type)) {
        AdgContainer *container;
        AdgPath *path;
        AdgStroke *stroke;
        const CpmlExtents *extents;
        gint counter;

        container = g_object_new(type, NULL);

        g_assert_nonnull(container);
        g_assert_true(ADG_IS_CONTAINER(container));

        counter = 0;
        adg_container_foreach(container, G_CALLBACK(_adg_increment), &counter);
        g_assert_cmpint(counter, ==, 0);

        path = adg_path_new();
        adg_path_move_to_explicit(path, -123456, -789012);
        adg_path_line_to_explicit(path, 654321, 210987);
        stroke = adg_stroke_new(ADG_TRAIL(path));
        g_object_unref(path);

        /* stroke has a floating reference that will be owned by container */
        g_assert_null(adg_entity_get_parent(ADG_ENTITY(stroke)));
        adg_test_signal(container, "add");
        adg_container_add(container, ADG_ENTITY(stroke));
        g_assert_true(adg_test_signal_check(TRUE));
        g_assert_true(adg_entity_get_parent(ADG_ENTITY(stroke)) == ADG_ENTITY(container));

        /* Ensure container-add add a reference to stroke */
        g_assert_true(ADG_IS_STROKE(stroke));

        /* The following command should basically be a no-op */
        adg_container_add(container, ADG_ENTITY(stroke));

        counter = 0;
        adg_container_foreach(container, G_CALLBACK(_adg_increment), &counter);
        g_assert_cmpint(counter, ==, 1);

        /* Check the extents are *at least* as big as stroke. We cannot use
         * equality because some container can have margins (e.g. AdgCanvas) */
        adg_entity_arrange(ADG_ENTITY(container));
        extents = adg_entity_get_extents(ADG_ENTITY(container));
        g_assert_true(extents->is_defined);
        g_assert_cmpfloat(extents->org.x, <=, -123456);
        g_assert_cmpfloat(extents->org.y, <=, -789012);
        g_assert_cmpfloat(extents->size.x, >=, 123456 + 654321);
        g_assert_cmpfloat(extents->size.y, >=, 789012 + 210987);

        /* Keep stroke around */
        g_object_ref(stroke);

        adg_test_signal(container, "remove");
        adg_container_remove(container, ADG_ENTITY(stroke));
        g_assert_true(adg_test_signal_check(TRUE));
        g_assert_null(adg_entity_get_parent(ADG_ENTITY(stroke)));

        counter = 0;
        adg_container_foreach(container, G_CALLBACK(_adg_increment), &counter);
        g_assert_cmpint(counter, ==, 0);

        adg_entity_arrange(ADG_ENTITY(container));
        g_assert_cmpfloat(extents->org.x, >, -123456);
        g_assert_cmpfloat(extents->org.y, >, -789012);
        g_assert_cmpfloat(extents->size.x, <, 123456 + 654321);
        g_assert_cmpfloat(extents->size.y, <, 789012 + 210987);

        /* Check destroying a child remove it from container */
        adg_container_add(container, ADG_ENTITY(stroke));
        g_object_unref(stroke);

        counter = 0;
        adg_container_foreach(container, G_CALLBACK(_adg_increment), &counter);
        g_assert_cmpint(counter, ==, 1);

        g_object_run_dispose(G_OBJECT(stroke));

        counter = 0;
        adg_container_foreach(container, G_CALLBACK(_adg_increment), &counter);
        g_assert_cmpint(counter, ==, 0);

        g_object_unref(container);
    }
}

void
adg_test_add_container_checks(const gchar *testpath, GType type)
{
    GType *p_type = g_new(GType, 1);
    *p_type = type;
    g_test_add_data_func(testpath, p_type, (gpointer) _adg_container_checks);
}

static void
_adg_global_space_checks(AdgEntity *entity)
{
    cairo_t *cr;
    const CpmlExtents *extents;
    cairo_matrix_t scale2X;
    gdouble width, height;

    cr = adg_test_cairo_context();
    cairo_matrix_init_scale(&scale2X, 2, 2);

    adg_switch_extents(g_test_rand_bit());

    /* Store the original extents size in width/height */
    adg_entity_render(entity, cr);
    extents = adg_entity_get_extents(entity);
    g_assert_true(extents->is_defined);
    width = extents->size.x;
    height = extents->size.y;

    /* Check explicit global-changed emission */
    adg_test_signal(entity, "global-changed");
    adg_entity_global_changed(entity);
    g_assert_true(adg_test_signal_check(FALSE));

    /* Check that a zoom in global space scales is roughly equivalent to
     * the same zoom on extents too (not excactly because of fonts) */
    adg_entity_transform_global_map(entity, &scale2X, ADG_TRANSFORM_BEFORE);
    /* The "global-changed" signal handler has not been called here because
     * its emission is lazy, that is it will be emitted only when needed */
    g_assert_false(adg_test_signal_check(FALSE));

    adg_entity_invalidate(entity);
    g_assert_false(extents->is_defined);
    /* Still no global-changed signal emitted */
    g_assert_false(adg_test_signal_check(FALSE));

    adg_entity_arrange(entity);
    /* The "global-changed" signal has been emitted in the arrange phase */
    g_assert_true(adg_test_signal_check(TRUE));

    adg_test_signal(entity, "render");
    adg_entity_render(entity, cr);
    g_assert_true(adg_test_signal_check(TRUE));

    /* Let's call render twice to check caching does not break anything */
    adg_test_signal(entity, "render");
    adg_entity_render(entity, cr);
    g_assert_true(adg_test_signal_check(TRUE));

    extents = adg_entity_get_extents(entity);
    g_assert_cmpfloat(extents->size.x, >, width * 1.7);
    g_assert_cmpfloat(extents->size.x, <, width * 2.3);
    g_assert_cmpfloat(extents->size.y, >, height * 1.7);
    g_assert_cmpfloat(extents->size.y, <, height * 2.3);

    /* Restore the original global scale */
    cairo_matrix_invert(&scale2X);
    adg_entity_transform_global_map(entity, &scale2X, ADG_TRANSFORM_BEFORE);
    adg_entity_invalidate(entity);
    g_assert_false(extents->is_defined);
    adg_entity_render(entity, cr);
    extents = adg_entity_get_extents(entity);
    g_assert_cmpfloat(extents->size.x, ==, width);
    g_assert_cmpfloat(extents->size.y, ==, height);

    cairo_destroy(cr);
    g_object_unref(entity);
}

void
adg_test_add_global_space_checks(const gchar *testpath, gpointer entity)
{
    g_test_add_data_func(testpath, entity,
                         (gpointer) _adg_global_space_checks);
}

static void
_adg_local_space_checks(AdgEntity *entity)
{
    cairo_t *cr;
    const CpmlExtents *extents;
    cairo_matrix_t scale2X;
    gdouble width, height;

    cr = adg_test_cairo_context();
    cairo_matrix_init_scale(&scale2X, 2, 2);

    /* Store the original extents size in width/height */
    adg_test_signal(entity, "render");
    adg_entity_render(entity, cr);
    g_assert_true(adg_test_signal_check(TRUE));

    extents = adg_entity_get_extents(entity);
    g_assert_true(extents->is_defined);
    width = extents->size.x;
    height = extents->size.y;

    /* Check that a scale in local space somewhat scales the extents too */
    adg_test_signal(entity, "local-changed");
    adg_entity_transform_local_map(entity, &scale2X, ADG_TRANSFORM_BEFORE);
    g_assert_false(adg_test_signal_check(FALSE));

    adg_entity_invalidate(entity);
    g_assert_false(extents->is_defined);
    adg_entity_render(entity, cr);
    /* The "local-changed" signal has been emitted here because "render" must
     * also call "arrange" that, in turn, should trigger "local-changed" */
    g_assert_true(adg_test_signal_check(TRUE));

    extents = adg_entity_get_extents(entity);
    g_assert_cmpfloat(extents->size.x, >, width);
    g_assert_cmpfloat(extents->size.y, >, height);

    /* Restore the original local scale */
    cairo_matrix_invert(&scale2X);
    adg_entity_transform_local_map(entity, &scale2X, ADG_TRANSFORM_BEFORE);
    adg_entity_invalidate(entity);
    g_assert_false(extents->is_defined);
    adg_entity_render(entity, cr);
    extents = adg_entity_get_extents(entity);
    g_assert_cmpfloat(extents->size.x, ==, width);
    g_assert_cmpfloat(extents->size.y, ==, height);

    cairo_destroy(cr);
    g_object_unref(entity);
}

void
adg_test_add_local_space_checks(const gchar *testpath, gpointer entity)
{
    g_test_add_data_func(testpath, entity,
                         (gpointer) _adg_local_space_checks);
}

#if GLIB_CHECK_VERSION(2, 38, 0)

static void
_adg_trap(AdgTrapsFunc func, gint i)
{
    if (g_test_subprocess()) {
        func(i);
        return;
    }
    g_test_trap_subprocess(NULL, 0, 0);
    g_print("\b\b\b%2d ", i);
    func(0);
}

#else

#include <stdlib.h>

#define ADG_TEST_TRAP_FLAGS     (G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR)

static void
_adg_trap(AdgTrapsFunc func, gint i)
{
    if (g_test_trap_fork(0, ADG_TEST_TRAP_FLAGS)) {
        func(i);
        exit(0);
    }
    g_print("\b\b\b%2d ", i);
    func(0);
}

#endif

static void
_adg_traps(_TrapsData *traps_data)
{
    AdgTrapsFunc func = traps_data->func;
    gint n_fragments = traps_data->n_fragments;
    gint i;

    g_free(traps_data);

    for (i = 1; i <= n_fragments; ++i) {
        _adg_trap(func, i);
    }
}

void
adg_test_add_traps(const gchar *testpath, AdgTrapsFunc func, gint n_fragments)
{
    _TrapsData *traps_data;

    g_return_if_fail(n_fragments > 0);

    traps_data = g_new(_TrapsData, 1);
    traps_data->func = func;
    traps_data->n_fragments = n_fragments;
    g_test_add_data_func(testpath, traps_data, (gpointer) _adg_traps);
}
