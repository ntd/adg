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


#include "adg-internal.h"
#include "adg-test.h"


typedef struct {
    void (*func)(gpointer);
    gpointer data;
} _FuncData;

typedef struct {
    GType type;
    gpointer instance;
} _BoxedData;


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

    g_test_bug_base("http://dev.entidi.com/p/adg/issues/%s/");
}

const gpointer
adg_test_invalid_pointer(void)
{
    static int junk[10] = { 0 };
    return junk;
}

static void
_adg_test_func(_FuncData *func_data)
{
    func_data->func(func_data->data);
    g_free(func_data);
}

void
adg_test_add_func_full(const char *testpath,
                       GCallback test_func, gpointer user_data)
{
    _FuncData *func_data = g_new(_FuncData, 1);
    func_data->func = (gpointer) test_func;
    func_data->data = user_data;

    g_test_add_data_func(testpath, func_data, (GTestDataFunc) _adg_test_func);
}

void
adg_test_add_func(const char *testpath, GCallback test_func)
{
    adg_test_add_func_full(testpath, test_func, NULL);
}

static void
_adg_enum_checks(gconstpointer user_data)
{
    GType type;
    gpointer class;

    type = GPOINTER_TO_INT(user_data);
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
    adg_test_add_func_full(testpath, G_CALLBACK(_adg_enum_checks), GINT_TO_POINTER(type));
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

    adg_test_add_func_full(testpath, G_CALLBACK(_adg_boxed_checks), boxed_data);
}

static void
_adg_object_checks(gconstpointer user_data)
{
    GType type = GPOINTER_TO_INT(user_data);
    g_assert_true(G_TYPE_IS_OBJECT(type));

    if (! G_TYPE_IS_ABSTRACT(type)) {
        gpointer result = GINT_TO_POINTER(0xdead);
        GObject *object = g_object_new(type, NULL);

        g_assert_nonnull(object);
        g_assert_true(G_IS_OBJECT(object));

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
    adg_test_add_func_full(testpath,
                           G_CALLBACK(_adg_object_checks),
                           GINT_TO_POINTER(type));
}

static void
_adg_entity_checks(gconstpointer user_data)
{
    GType type = GPOINTER_TO_INT(user_data);
    g_assert_true(g_type_is_a(type, ADG_TYPE_ENTITY));

    if (! G_TYPE_IS_ABSTRACT(type)) {
        AdgEntity *entity = g_object_new(type, NULL);
        const CpmlExtents *extents;

        g_assert_nonnull(entity);
        g_assert_true(ADG_IS_ENTITY(entity));

        /* Ensure that the entity extents are not initially defined */
        extents = adg_entity_get_extents(entity);
        g_assert_false(extents->is_defined);

        adg_entity_arrange(entity);
        extents = adg_entity_get_extents(entity);
        if (extents->is_defined) {
            /* RENDERABLE ENTITY */
            /* Check that invalidate() clears the cached extents */
            adg_entity_invalidate(entity);
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
    adg_test_add_func_full(testpath,
                           G_CALLBACK(_adg_entity_checks),
                           GINT_TO_POINTER(type));
}

static cairo_t *
_adg_cairo_context(void)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    return cr;
}

static void
_adg_global_space_checks(AdgEntity *entity)
{
    cairo_t *cr;
    const CpmlExtents *extents;
    cairo_matrix_t scale2X;
    gdouble width, height;

    cr = _adg_cairo_context();
    cairo_matrix_init_scale(&scale2X, 2, 2);

    /* Store the original extents size in width/height */
    adg_entity_render(entity, cr);
    extents = adg_entity_get_extents(entity);
    g_assert_true(extents->is_defined);
    width = extents->size.x;
    height = extents->size.y;

    /* Check that a zoom in global space scales is roughly equivalent to
     * the same zoom on extents too (not excactly because of fonts) */
    adg_entity_transform_global_map(entity, &scale2X, ADG_TRANSFORM_BEFORE);
    adg_entity_invalidate(entity);
    g_assert_false(extents->is_defined);
    adg_entity_render(entity, cr);
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
    adg_test_add_func_full(testpath, G_CALLBACK(_adg_global_space_checks), entity);
}

static void
_adg_local_space_checks(AdgEntity *entity)
{
    cairo_t *cr;
    const CpmlExtents *extents;
    cairo_matrix_t scale2X;
    gdouble width, height;

    cr = _adg_cairo_context();
    cairo_matrix_init_scale(&scale2X, 2, 2);

    /* Store the original extents size in width/height */
    adg_entity_render(entity, cr);
    extents = adg_entity_get_extents(entity);
    g_assert_true(extents->is_defined);
    width = extents->size.x;
    height = extents->size.y;

    /* Check that a scale in local space somewhat scales the extents too */
    adg_entity_transform_local_map(entity, &scale2X, ADG_TRANSFORM_BEFORE);
    adg_entity_invalidate(entity);
    g_assert_false(extents->is_defined);
    adg_entity_render(entity, cr);
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
    adg_test_add_func_full(testpath, G_CALLBACK(_adg_local_space_checks), entity);
}
