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


/**
 * SECTION:adg-path
 * @short_description: The basic model representing a generic path
 *
 * The #AdgPath model is a virtual path: in other words it is a
 * non-rendered #cairo_path_t struct. This class implements methods
 * to manipulate the underlying cairo path.
 *
 * Although some of the provided methods are clearly based on the
 * original cairo path manipulation API, their behavior could be
 * sligthly different. This is intentional, because the ADG provides
 * additional path manipulation algorithms, sometime quite complex,
 * and a more restrictive filter on the path quality is required.
 * Also, the ADG is designed to be used by technicians while cairo
 * targets a broader range of developers.
 *
 * As an example, following the rule of the less surprise, some
 * cairo functions guess the current point when it is not defined,
 * while the #AdgPath methods trigger a warning without other effect.
 * Furthermore, after a cairo_path_close_path() call a %MOVE_TO
 * primitive to the starting point of the segment is automatically
 * added by cairo while in ADG, after an adg_path_close(), the
 * current point is simply unset.
 **/

/**
 * AdgPath:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-path.h"
#include "adg-path-private.h"
#include "adg-primitive.h"
#include "adg-intl.h"

#include <math.h>


static void             finalize                (GObject        *object);
static void             changed                 (AdgModel       *model);
static void             clear_cairo_path        (AdgPath        *path);
static cairo_path_t *   get_cairo_path          (AdgPath        *path);
static CpmlPath *       get_cpml_path           (AdgPath        *path);
static GArray *         arc_to_curves           (GArray         *array,
                                                 const cairo_path_data_t
                                                                *src);
static void             append_primitive        (AdgPath        *path,
                                                 AdgPrimitive   *primitive);
static gint             needed_pairs            (CpmlPrimitiveType type,
                                                 gboolean        cp_is_valid);
static void             clear_operation         (AdgPath        *path);
static gboolean         append_operation        (AdgPath        *path,
                                                 AdgOperator     operator,
                                                 ...);
static void             do_operation            (AdgPath        *path,
                                                 cairo_path_data_t
                                                                *path_data);
static void             do_chamfer              (AdgPath        *path,
                                                 CpmlPrimitive  *current);
static void             do_fillet               (AdgPath        *path,
                                                 CpmlPrimitive  *current);
static gboolean         is_convex               (const CpmlPrimitive
                                                                *primitive1,
                                                 const CpmlPrimitive
                                                                *primitive2);


G_DEFINE_TYPE(AdgPath, adg_path, ADG_TYPE_MODEL);


static void
adg_path_class_init(AdgPathClass *klass)
{
    GObjectClass *gobject_class;
    AdgModelClass *model_class;

    gobject_class = (GObjectClass *) klass;
    model_class = (AdgModelClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgPathPrivate));

    gobject_class->finalize = finalize;

    model_class->changed = changed;
}

static void
adg_path_init(AdgPath *path)
{
    AdgPathPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(path, ADG_TYPE_PATH,
                                                       AdgPathPrivate);

    data->cp_is_valid = FALSE;
    data->path = g_array_new(FALSE, FALSE, sizeof(cairo_path_data_t));
    data->cairo_path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cairo_path.data = NULL;
    data->cairo_path.num_data = 0;
    data->operation.operator = ADG_OPERATOR_NONE;

    path->data = data;
}

static void
finalize(GObject *object)
{
    AdgPath *path;
    AdgPathPrivate *data;
    GObjectClass *object_class;

    path = (AdgPath *) object;
    data = path->data;
    object_class = (GObjectClass *) adg_path_parent_class;

    g_array_free(data->path, TRUE);
    clear_cairo_path(path);
    clear_operation(path);

    if (object_class->finalize != NULL)
        object_class->finalize(object);
}


/**
 * adg_path_new:
 *
 * Creates a new path model. The path must be constructed in the @callback
 * function: AdgPath will cache and reuse the cairo_copy_path() returned by
 * the cairo context after the @callback call.
 *
 * Returns: the new model
 **/
AdgModel *
adg_path_new(void)
{
    return (AdgModel *) g_object_new(ADG_TYPE_PATH, NULL);
}


/**
 * adg_path_get_cairo_path:
 * @path: an #AdgPath
 *
 * Gets a pointer to the cairo path structure of @path. The return path
 * is owned by @path and must be considered read-only.
 *
 * This function also converts %CAIRO_PATH_ARC_TO primitives, not
 * recognized by cairo, into approximated Bézier curves. The conversion
 * is cached so any furter request is O(1). This cache is cleared
 * whenever @path is modified (by adding a new primitive or by calling
 * adg_path_clear()).
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>Actually, the arcs are approximated to Bézier using the
 *           hardcoded max angle of PI/2. This should be customizable
 *           by adding, for instance, a property to the #AdgPath class
 *           with a default value of PI/2.</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: a pointer to the internal cairo path or %NULL on errors
 **/
const cairo_path_t *
adg_path_get_cairo_path(AdgPath *path)
{
    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    return get_cairo_path(path);
}

/**
 * adg_path_get_cpml_path:
 * @path: an #AdgPath
 *
 * Gets a pointer to the cairo path structure of @path. The return
 * value is owned by @path and must not be freed.
 *
 * This function is similar to adg_path_get_cairo_path() but with
 * two important differences: firstly the arc primitives are not
 * expanded to Bézier curves and secondly the returned path is
 * not read-only. This means it is allowed to modify the returned
 * path as long as its size is retained and its data contains a
 * valid path.
 *
 * Any changes to the @path instance will make the returned pointer
 * useless because probably the internal #CpmlPath will be relocated
 * and the old #CpmlPath will likely contain plain garbage.
 *
 * Returns: a pointer to the internal cpml path or %NULL on errors
 **/
CpmlPath *
adg_path_get_cpml_path(AdgPath *path)
{
    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    clear_cairo_path(path);

    return get_cpml_path(path);
}

/**
 * adg_path_get_segment:
 * @path: an #AdgPath
 * @segment: the destination #AdgSegment
 * @n: the segment number to retrieve
 *
 * Convenient function to get a segment from @path. The segment is
 * got from the CPML path: check out adg_path_get_cpml_path() for
 * further information.
 *
 * Returns: %TRUE on success or %FALSE on errors
 **/
gboolean
adg_path_get_segment(AdgPath *path, AdgSegment *segment, guint n)
{
    CpmlPath *cpml_path;
    guint cnt;

    g_return_val_if_fail(ADG_IS_PATH(path), FALSE);

    if (n == 0)
        return FALSE;

    cpml_path = get_cpml_path(path);

    cpml_segment_from_cairo(segment, cpml_path);
    for (cnt = 1; cnt < n; ++cnt)
        if (!cpml_segment_next(segment)) {
            g_warning("%s: segment `%u' out of range for type `%s'",
                      G_STRLOC, n, g_type_name(G_OBJECT_TYPE(path)));
            return FALSE;
        }

    return TRUE;
}

/**
 * adg_path_get_current_point:
 * @path: an #AdgPath
 * @x: where to store the x coordinate of the current point
 * @y: where to store the y coordinate of the current point
 *
 * Gets the current point of @path, which is conceptually the
 * final point reached by the path so far.
 *
 * If there is no defined current point, @x and @y will both be set
 * to 0 and a warning will be triggered. It is possible to check this
 * in advance with adg_path_has_current_point().
 *
 * Most #AdgPath methods alter the current point and most of them
 * expect a current point to be defined otherwise will fail triggering
 * a warning. Check the description of every method for specific details.
 **/
void
adg_path_get_current_point(AdgPath *path, gdouble *x, gdouble *y)
{
    AdgPathPrivate *data;

    g_return_if_fail(ADG_IS_PATH(path));

    data = path->data;

    if (data->cp_is_valid) {
        *x = data->cp.x;
        *y = data->cp.y;
    } else {
        *x = *y = 0.;
        g_return_if_reached();
    }
}

/**
 * adg_path_has_current_point:
 * @path: an #AdgPath
 *
 * Returns whether a current point is defined on @path.
 * See adg_path_get_current_point() for details on the current point.
 *
 * Returns: whether a current point is defined
 **/
gboolean
adg_path_has_current_point(AdgPath *path)
{
    AdgPathPrivate *data;

    g_return_val_if_fail(ADG_IS_PATH(path), FALSE);

    data = path->data;

    return data->cp_is_valid;
}

/**
 * adg_path_clear:
 * @path: an #AdgPath
 *
 * Releases the internal memory hold by @path and resets its status,
 * so that after this call @path contains an empty path.
 **/
void
adg_path_clear(AdgPath *path)
{
    AdgPathPrivate *data;

    g_return_if_fail(ADG_IS_PATH(path));

    data = path->data;

    g_array_set_size(data->path, 0);
    clear_cairo_path(path);
    clear_operation(path);
}


/**
 * adg_path_append:
 * @path: an #AdgPath
 * @type: a #cairo_data_type_t value
 * @...:  point data, specified as #AdgPair pointers
 *
 * Generic method to append a primitive to @path. The number of #AdgPair
 * structs depends on @type: there is no way with this function to
 * reserve more cairo_path_data_t structs than what is needed by the
 * primitive.
 *
 * This function accepts also the special %CAIRO_PATH_ARC_TO primitive.
 *
 * If @path has no current point while the requested primitive needs it,
 * a warning message will be triggered without other effect.
 **/
void
adg_path_append(AdgPath *path, CpmlPrimitiveType type, ...)
{
    va_list var_args;

    va_start(var_args, type);
    adg_path_append_valist(path, type, var_args);
    va_end(var_args);
}

/**
 * adg_path_append_valist:
 * @path:     an #AdgPath
 * @type:     a #cairo_data_type_t value
 * @var_args: point data, specified as #AdgPair pointers
 *
 * va_list version of adg_path_append().
 **/
void
adg_path_append_valist(AdgPath *path, CpmlPrimitiveType type, va_list var_args)
{
    AdgPathPrivate *data;
    AdgPrimitive primitive;
    gint length, cnt;
    cairo_path_data_t org;
    cairo_path_data_t *path_data;

    g_return_if_fail(ADG_IS_PATH(path));

    data = path->data;
    length = needed_pairs(type, data->cp_is_valid);
    if (length == 0)
        return;

    /* Set a copy of the current point as the primitive origin */
    cpml_pair_to_cairo(&data->cp, &org);
    primitive.org = &org;

    /* Build the cairo_path_data_t array */
    primitive.data = path_data = g_new(cairo_path_data_t, length);

    path_data->header.type = type;
    path_data->header.length = length;

    for (cnt = 1; cnt < length; ++ cnt) {
        ++ path_data;
        cpml_pair_to_cairo(va_arg(var_args, AdgPair *), path_data);
    }

    /* Terminate the creation of the temporary primitive */
    primitive.segment = NULL;

    /* Append this primitive to @path */
    append_primitive(path, &primitive);

    g_free(primitive.data);
}

/**
 * adg_path_append_primitive:
 * @path:      an #AdgPath
 * @primitive: the #AdgPrimitive to append
 *
 * Appends @primitive to @path. The primitive to add is considered the
 * continuation of the current path so the <structfield>org</structfield>
 * component of @primitive is not used. Anyway the current poins is
 * checked against it: they must be equal or the function will fail
 * without further processing.
 **/
void
adg_path_append_primitive(AdgPath *path, const AdgPrimitive *primitive)
{
    AdgPathPrivate *data;
    AdgPrimitive *primitive_dup;

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(primitive != NULL);

    data = path->data;

    g_return_if_fail(primitive->org->point.x == data->cp.x &&
                     primitive->org->point.y == data->cp.y);

    /* The primitive data could be modified by pending operations:
     * work on a copy */
    primitive_dup = adg_primitive_deep_dup(primitive);

    append_primitive(path, primitive_dup);

    g_free(primitive_dup);
}

/**
 * adg_path_append_segment:
 * @path:    an #AdgPath
 * @segment: the #AdgSegment to append
 *
 * Appends @segment to @path.
 **/
void
adg_path_append_segment(AdgPath *path, const AdgSegment *segment)
{
    AdgPathPrivate *data;

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(segment != NULL);

    data = path->data;

    clear_cairo_path(path);
    data->path = g_array_append_vals(data->path,
                                     segment->data, segment->num_data);
}

/**
 * adg_path_append_cairo_path:
 * @path:       an #AdgPath
 * @cairo_path: the #cairo_path_t path to append
 *
 * Appends a whole cairo path to @path.
 **/
void
adg_path_append_cairo_path(AdgPath *path, const cairo_path_t *cairo_path)
{
    AdgPathPrivate *data;

    g_return_if_fail(ADG_IS_PATH(path));

    data = path->data;

    clear_cairo_path(path);
    data->path = g_array_append_vals(data->path,
                                     cairo_path->data, cairo_path->num_data);
}

/**
 * adg_path_move_to:
 * @path: an #AdgPath
 * @x:    the new x coordinate
 * @y:    the new y coordinate
 *
 * Begins a new segment. After this call the current point will be (@x, @y).
 **/
void
adg_path_move_to(AdgPath *path, gdouble x, gdouble y)
{
    AdgPair p;

    p.x = x;
    p.y = y;

    adg_path_append(path, CAIRO_PATH_MOVE_TO, &p);
}

/**
 * adg_path_line_to:
 * @path: an #AdgPath
 * @x:    the new x coordinate
 * @y:    the new y coordinate
 *
 * Adds a line to @path from the current point to position (@x, @y).
 * After this call the current point will be (@x, @y).
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 **/
void
adg_path_line_to(AdgPath *path, gdouble x, gdouble y)
{
    AdgPair p;

    p.x = x;
    p.y = y;

    adg_path_append(path, CAIRO_PATH_LINE_TO, &p);
}

/**
 * adg_path_arc_to:
 * @path: an #AdgPath
 * @x1:   the x coordinate of an intermediate point
 * @y1:   the y coordinate of an intermediate point
 * @x2:   the x coordinate of the end of the arc
 * @y2:   the y coordinate of the end of the arc
 *
 * Adds an arc to the path from the current point to (@x2, @y2),
 * passing throught (@x1, @y1). After this call the current point
 * will be (@x2, @y2).
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 **/
void
adg_path_arc_to(AdgPath *path, gdouble x1, gdouble y1, gdouble x2, gdouble y2)
{
    AdgPair p[2];

    p[0].x = x1;
    p[0].y = y1;
    p[1].x = x2;
    p[1].y = y2;

    adg_path_append(path, CAIRO_PATH_ARC_TO, &p[0], &p[1]);
}

/**
 * adg_path_curve_to:
 * @path: an #AdgPath
 * @x1:   the x coordinate of the first control point
 * @y1:   the y coordinate of the first control point
 * @x2:   the x coordinate of the second control point
 * @y2:   the y coordinate of the second control point
 * @x3:   the x coordinate of the end of the curve
 * @y3:   the y coordinate of the end of the curve
 *
 * Adds a cubic Bézier curve to the path from the current point to
 * position (@x3, @y3), using (@x1, @y1) and (@x2, @y2) as the
 * control points. After this call the current point will be (@x3, @y3).
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 **/
void
adg_path_curve_to(AdgPath *path, gdouble x1, gdouble y1,
                  gdouble x2, gdouble y2, gdouble x3, gdouble y3)
{
    AdgPair p[3];

    p[0].x = x1;
    p[0].y = y1;
    p[1].x = x2;
    p[1].y = y2;
    p[2].x = x3;
    p[2].y = y3;

    adg_path_append(path, CAIRO_PATH_CURVE_TO, &p[0], &p[1], &p[2]);
}

/**
 * adg_path_close:
 * @path: an #AdgPath
 *
 * Adds a line segment to the path from the current point to the
 * beginning of the current segment, (the most recent point passed
 * to an adg_path_move_to()), and closes this segment.
 * After this call the current point will be unset.
 *
 * The behavior of adg_path_close() is distinct from simply calling
 * adg_line_to() with the coordinates of the segment starting point.
 * When a closed segment is stroked, there are no caps on the ends.
 * Instead, there is a line join connecting the final and initial
 * primitive of the segment.
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 **/
void
adg_path_close(AdgPath *path)
{
    adg_path_append(path, CAIRO_PATH_CLOSE_PATH);
}

/**
 * adg_path_arc
 * @path:  an #AdgPath
 * @xc:    x position of the center of the arc
 * @yc:    y position of the center of the arc
 * @r:     the radius of the arc
 * @start: the start angle, in radians
 * @end:   the end angle, in radians
 *
 * A more usual way to add an arc to @path. After this call, the current
 * point will be the computed end point of the arc. The arc will be
 * rendered in increasing angle, accordling to @start and @end. This means
 * if @start is less than @end, the arc will be rendered in clockwise
 * direction (accordling to the default cairo coordinate system) while if
 * @start is greather than @end, the arc will be rendered in couterclockwise
 * direction.
 *
 * By explicitely setting the whole arc data, the start point could be
 * different from the current point. In this case, if @path has no
 * current point before the call a %CAIRO_PATH_MOVE_TO to the start
 * point of the arc will be automatically prepended to the arc.
 * If @path has a current point, a %CAIRO_PATH_LINE_TO to the start
 * point of the arc will be used instead of the moveto.
 **/
void
adg_path_arc(AdgPath *path, gdouble xc, gdouble yc, gdouble r,
             gdouble start, gdouble end)
{
    AdgPathPrivate *data;
    AdgPair center, p[3];

    g_return_if_fail(ADG_IS_PATH(path));

    data = path->data;
    center.x = xc;
    center.y = yc;

    cpml_vector_from_angle(&p[0], start, r);
    cpml_vector_from_angle(&p[1], (end-start) / 2, r);
    cpml_vector_from_angle(&p[2], end, r);

    cpml_pair_add(&p[0], &center);
    cpml_pair_add(&p[1], &center);
    cpml_pair_add(&p[2], &center);

    if (!data->cp_is_valid)
        adg_path_append(path, CAIRO_PATH_MOVE_TO, &p[0]);
    else if (p[0].x != data->cp.x || p[0].y != data->cp.y)
        adg_path_append(path, CAIRO_PATH_LINE_TO, &p[0]);

    adg_path_append(path, CAIRO_PATH_ARC_TO, &p[1], &p[2]);
}

/**
 * adg_path_chamfer
 * @path:   an #AdgPath
 * @delta1: the distance from the intersection point of the current primitive
 * @delta2: the distance from the intersection point of the next primitive
 *
 * A binary operator that generates a chamfer between two primitives.
 * The first primitive involved is the current primitive, the second will
 * be the next primitive appended to @path after this call. The second
 * primitive is required: if the chamfer operation is not properly
 * terminated (by not providing the second primitive), any API accessing
 * the path in reading mode will raise a warning.
 *
 * The chamfer operation requires two lengths: @delta1 specifies the
 * "quantity" to trim on the first primitive while @delta2 is the same
 * applied on the second primitive. The term "quantity" means the length
 * of the portion to cut out from the original primitive (that is the
 * primitive as would be without the chamfer).
 **/
void
adg_path_chamfer(AdgPath *path, gdouble delta1, gdouble delta2)
{
    g_return_if_fail(ADG_IS_PATH(path));

    if (!append_operation(path, ADG_OPERATOR_CHAMFER, delta1, delta2))
        return;
}

/**
 * adg_path_fillet:
 * @path:   an #AdgPath
 * @radius: the radius of the fillet
 *
 *
 * A binary operator that joins to primitives with an arc.
 * The first primitive involved is the current primitive, the second will
 * be the next primitive appended to @path after this call. The second
 * primitive is required: if the fillet operation is not properly
 * terminated (by not providing the second primitive), any API accessing
 * the path in reading mode will raise a warning.
 **/
void
adg_path_fillet(AdgPath *path, gdouble radius)
{
    g_return_if_fail(ADG_IS_PATH(path));

    if (!append_operation(path, ADG_OPERATOR_FILLET, radius))
        return;
}


/**
 * adg_path_dump:
 * @path: an #AdgPath
 *
 * Dumps the data content of @path to stdout in a human readable format.
 **/
void
adg_path_dump(AdgPath *path)
{
    CpmlSegment segment;
    cairo_path_t *cairo_path;

    g_return_if_fail(ADG_IS_PATH(path));

    cairo_path = get_cairo_path(path);

    g_return_if_fail(cairo_path != NULL);

    if (!cpml_segment_from_cairo(&segment, cairo_path)) {
        g_warning("Invalid path data to dump!\n");
    } else {
        do {
            cpml_segment_dump(&segment);
        } while (cpml_segment_next(&segment));
    }
}


static void
changed(AdgModel *model)
{
    AdgModelClass *model_class = (AdgModelClass *) adg_path_parent_class;

    adg_path_clear((AdgPath *) model);

    if (model_class->changed != NULL)
        model_class->changed(model);
}

static void
clear_cairo_path(AdgPath *path)
{
    AdgPathPrivate *data;
    cairo_path_t *cairo_path;

    data = path->data;
    cairo_path = &data->cairo_path;

    if (cairo_path->data == NULL)
        return;

    g_free(cairo_path->data);

    cairo_path->status = CAIRO_STATUS_INVALID_PATH_DATA;
    cairo_path->data = NULL;
    cairo_path->num_data = 0;
}

static cairo_path_t *
get_cairo_path(AdgPath *path)
{
    AdgPathPrivate *data;
    cairo_path_t *cairo_path;
    const GArray *src;
    GArray *dst;
    const cairo_path_data_t *p_src;
    int i;

    data = path->data;
    cairo_path = &data->cairo_path;

    /* Check for cached result */
    if (cairo_path->data != NULL)
        return cairo_path;

    src = data->path;
    dst = g_array_sized_new(FALSE, FALSE, sizeof(cairo_path_data_t), src->len);

    /* Cycle the path and convert arcs to Bézier curves */
    for (i = 0; i < src->len; i += p_src->header.length) {
        p_src = (const cairo_path_data_t *) src->data + i;

        if (p_src->header.type == CAIRO_PATH_ARC_TO)
            dst = arc_to_curves(dst, p_src);
        else
            dst = g_array_append_vals(dst, p_src, p_src->header.length);
    }

    cairo_path->status = CAIRO_STATUS_SUCCESS;
    cairo_path->num_data = dst->len;
    cairo_path->data = (cairo_path_data_t *) g_array_free(dst, FALSE);

    return cairo_path;
}

static CpmlPath *
get_cpml_path(AdgPath *path)
{
    AdgPathPrivate *data;
    CpmlPath *cpml_path;

    data = path->data;
    cpml_path = &data->cpml_path;

    cpml_path->status = CAIRO_STATUS_SUCCESS;
    cpml_path->data = (cairo_path_data_t *) data->path->data;
    cpml_path->num_data = data->path->len;

    return cpml_path;
}

static GArray *
arc_to_curves(GArray *array, const cairo_path_data_t *src)
{
    CpmlPrimitive arc;
    double start, end;

    /* Build the arc primitive: the arc origin is supposed to be the previous
     * point (src-1): this means a primitive must exist before the arc */
    arc.segment = NULL;
    arc.org = (cairo_path_data_t *) (src-1);
    arc.data = (cairo_path_data_t *) src;

    if (cpml_arc_info(&arc, NULL, NULL, &start, &end)) {
        CpmlSegment segment;
        int n_curves;
        cairo_path_data_t *curves;

        n_curves = ceil(fabs(end-start) / M_PI_2);
        curves = g_new(cairo_path_data_t, n_curves * 4);
        segment.data = curves;
        cpml_arc_to_curves(&arc, &segment, n_curves);

        array = g_array_append_vals(array, curves, n_curves * 4);

        g_free(curves);
    }

    return array;
}

static void
append_primitive(AdgPath *path, AdgPrimitive *current)
{
    AdgPathPrivate *data;
    cairo_path_data_t *path_data;
    int length;

    data = path->data;
    path_data = current->data;
    length = path_data[0].header.length;

    /* Execute any pending operation */
    do_operation(path, path_data);

    /* Append the path data to the internal path array */
    data->path = g_array_append_vals(data->path, path_data, length);

    /* Set path data to point to the recently appended cairo_path_data_t
     * primitive: the first struct is the header */
    path_data = (cairo_path_data_t *) data->path->data +
                data->path->len - length;

    /* Set the last primitive for subsequent binary operations */
    data->last.org = data->cp_is_valid ? path_data - 1 : NULL;
    data->last.segment = NULL;
    data->last.data = path_data;

    /* Save the last point as the current point, if applicable */
    data->cp_is_valid = length > 1;
    if (length > 1)
        cpml_pair_from_cairo(&data->cp, &path_data[length-1]);

    /* Invalidate cairo_path: should be recomputed */
    clear_cairo_path(path);
}

static gint
needed_pairs(CpmlPrimitiveType type, gboolean cp_is_valid)
{
    switch (type) {

    case CAIRO_PATH_CLOSE_PATH:
        g_return_val_if_fail(cp_is_valid, 0);
        return 1;

    case CAIRO_PATH_MOVE_TO:
        return 2;

    case CAIRO_PATH_LINE_TO:
        g_return_val_if_fail(cp_is_valid, 0);
        return 2;

    case CAIRO_PATH_ARC_TO:
        g_return_val_if_fail(cp_is_valid, 0);
        return 3;

    case CAIRO_PATH_CURVE_TO:
        g_return_val_if_fail(cp_is_valid, 0);
        return 4;

    default:
        g_return_val_if_reached(0);
    }

    return 0;
}

static void
clear_operation(AdgPath *path)
{
    AdgPathPrivate *data;
    AdgOperation *operation;

    data = path->data;
    operation = &data->operation;

    if (operation->operator == ADG_OPERATOR_NONE)
        return;

    g_warning("An operation is still active while clearing the path "
              "(operator `%d')", operation->operator);
    operation->operator = ADG_OPERATOR_NONE;
}

static gboolean
append_operation(AdgPath *path, AdgOperator operator, ...)
{
    AdgPathPrivate *data;
    AdgOperation *operation;
    va_list var_args;

    data = path->data;

    if (!data->cp_is_valid) {
        g_warning("Operation requested but path has no current primitive "
                  "(operator `%d')", operator);
        return FALSE;
    }

    operation = &data->operation;
    if (operation->operator != ADG_OPERATOR_NONE) {
        /* TODO: this is a rude semplification, as a lot of operators can
         * and may cohexist. As an example, a fillet followed by a
         * polar chamfer is not difficult to compute */
        g_warning("Operation requested but another operation is yet active"
                  "(operators: new `%d', old `%d')",
                  operator, operation->operator);
        return FALSE;
    }

    va_start(var_args, operator);

    switch (operator) {

    case ADG_OPERATOR_CHAMFER:
        operation->data.chamfer.delta1 = va_arg(var_args, double);
        operation->data.chamfer.delta2 = va_arg(var_args, double);
        break;

    case ADG_OPERATOR_FILLET:
        operation->data.fillet.radius = va_arg(var_args, double);
        break;

    case ADG_OPERATOR_NONE:
        va_end(var_args);
        return TRUE;

    default:
        g_warning("Operation not recognized (operator `%d')", operator);
        va_end(var_args);
        return FALSE;
    }

    operation->operator = operator;
    va_end(var_args);

    return TRUE;
}

static void
do_operation(AdgPath *path, cairo_path_data_t *path_data)
{
    AdgPathPrivate *data;
    AdgOperator operator;
    CpmlSegment segment;
    CpmlPrimitive current;
    cairo_path_data_t current_org;

    data = path->data;
    operator = data->operation.operator;
    cpml_segment_from_cairo(&segment, get_cpml_path(path));

    /* Construct the current primitive, that is the primitive to be inserted.
     * Its org is a copy of the end point of the last primitive: it can be
     * modified without affecting anything else. It is expected the operation
     * functions will add to @path the primitives required but NOT to add
     * @current, as this one will be inserted automatically. */
    current.segment = &segment;
    current.org = &current_org;
    current.data = path_data;
    cpml_pair_to_cairo(&data->cp, &current_org);

    switch (operator) {

    case ADG_OPERATOR_NONE:
        return;

    case ADG_OPERATOR_CHAMFER:
        do_chamfer(path, &current);
        break;

    case ADG_OPERATOR_FILLET:
        do_fillet(path, &current);
        break;

    default:
        g_warning("Operation not implemented (operator `%d')", operator);
        return;
    }
}

static void
do_chamfer(AdgPath *path, CpmlPrimitive *current)
{
    AdgPathPrivate *data;
    CpmlPrimitive *last;
    gdouble delta1, delta2;
    gdouble len1, len2;
    AdgPair pair;
    cairo_path_data_t line[2];

    data = path->data;
    last = &data->last;
    delta1 = data->operation.data.chamfer.delta1;
    len1 = cpml_primitive_length(last);

    if (delta1 >= len1) {
        g_warning("Chamfer too big for the last primitive (%lf >= %lf)",
                  delta1, len1);
        return;
    }

    delta2 = data->operation.data.chamfer.delta2;
    len2 = cpml_primitive_length(current);

    if (delta2 >= len2) {
        g_warning("Chamfer too big for the current primitive (%lf >= %lf)",
                  delta2, len2);
        return;
    }

    /* Change the end point of the last primitive */
    cpml_primitive_pair_at(last, &pair, 1. - delta1 / len1);
    cpml_pair_to_cairo(&pair, cpml_primitive_get_point(last, -1));

    /* Change the start point of the current primitive */
    cpml_primitive_pair_at(current, &pair, delta2 / len2);
    cpml_pair_to_cairo(&pair, cpml_primitive_get_point(current, 0));

    /* Add the chamfer line */
    line[0].header.type = CAIRO_PATH_LINE_TO;
    line[0].header.length = 2;
    line[1].point.x = pair.x;
    line[1].point.y = pair.y;
    data->path = g_array_append_vals(data->path, line, 2);

    data->operation.operator = ADG_OPERATOR_NONE;
}

static void
do_fillet(AdgPath *path, CpmlPrimitive *current)
{
    AdgPathPrivate *data;
    CpmlPrimitive *last, *current_dup, *last_dup;
    gdouble radius, offset, pos;
    AdgPair center, vector, p[3];
    cairo_path_data_t arc[3];

    data = path->data;
    last = &data->last;
    current_dup = adg_primitive_deep_dup(current);

    /* Force current_dup to point to the original segment so a
     * CAIRO_PATH_CLOSE_PATH primitive will work as expected */
    current_dup->segment = current->segment;

    last_dup = adg_primitive_deep_dup(last);
    radius = data->operation.data.fillet.radius;
    offset = is_convex(last_dup, current_dup) ? -radius : radius;

    /* Find the center of the fillet from the intersection between
     * the last and current primitives offseted by radius */
    cpml_primitive_offset(current_dup, offset);
    cpml_primitive_offset(last_dup, offset);
    if (cpml_primitive_intersection(current_dup, last_dup,
                                    &center, 1) == 0) {
        g_warning("Fillet not applicable (radius = %lf)", radius);
        g_free(current_dup);
        g_free(last_dup);
        return;
    }

    /* Compute the start point of the fillet */
    pos = cpml_primitive_near_pos(last_dup, &center);
    cpml_primitive_vector_at(last_dup, &vector, pos);
    cpml_vector_set_length(&vector, offset);
    cpml_vector_normal(&vector);
    cpml_pair_sub(cpml_pair_copy(&p[0], &center), &vector);

    /* Compute the mid point of the fillet */
    cpml_pair_from_cairo(&vector, current->org);
    cpml_pair_sub(&vector, &center);
    cpml_vector_set_length(&vector, radius);
    cpml_pair_add(cpml_pair_copy(&p[1], &center), &vector);

    /* Compute the end point of the fillet */
    pos = cpml_primitive_near_pos(current_dup, &center);
    cpml_primitive_vector_at(current_dup, &vector, pos);
    cpml_vector_set_length(&vector, offset);
    cpml_vector_normal(&vector);
    cpml_pair_sub(cpml_pair_copy(&p[2], &center), &vector);

    g_free(current_dup);
    g_free(last_dup);

    /* Modify the end point of the last primitive */
    cpml_pair_to_cairo(&p[0], cpml_primitive_get_point(last, -1));

    /* Add the fillet arc */
    arc[0].header.type = CAIRO_PATH_ARC_TO;
    arc[0].header.length = 3;
    cpml_pair_to_cairo(&p[1], &arc[1]);
    cpml_pair_to_cairo(&p[2], &arc[2]);
    data->path = g_array_append_vals(data->path, arc, 3);

    data->operation.operator = ADG_OPERATOR_NONE;
}

static gboolean
is_convex(const CpmlPrimitive *primitive1, const CpmlPrimitive *primitive2)
{
    CpmlVector v1, v2;
    gdouble angle1, angle2;

    cpml_primitive_vector_at(primitive1, &v1, -1);
    cpml_primitive_vector_at(primitive2, &v2, 0);

    /* Probably there is a smarter way to get this without trygonometry */
    angle1 = cpml_vector_angle(&v1);
    angle2 = cpml_vector_angle(&v2);

    if (angle1 > angle2)
        angle1 -= M_PI*2;

    return angle2-angle1 > M_PI;
}
