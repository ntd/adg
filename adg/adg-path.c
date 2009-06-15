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
 * SECTION:path
 * @title: AdgPath
 * @short_description: The basic model representing a generic path
 *
 * The #AdgPath model is a virtual path: in a few words, it is a
 * simple conceptual #cairo_path_t struct. This class implements
 * methods to manipulate the underlying cairo path.
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
 * Furthermore, after a cairo_path_close_path() call a MOVE_TO
 * primitive to the starting point of the segment is automatically
 * added by cairo while in the ADG, after an adg_path_close(), the
 * current point is simply unset.
 **/

#include "adg-path.h"
#include "adg-path-private.h"
#include "adg-intl.h"

#include <math.h>

#define PARENT_CLASS    ((AdgModelClass *) adg_path_parent_class)


static void             finalize                (GObject        *object);
static void             changed                 (AdgModel       *model);
static void             clear_cairo_path        (AdgPath        *path);
static cairo_path_t *   get_cairo_path          (AdgPath        *path);
static GArray *         arc_to_curves           (GArray         *array,
                                                 const cairo_path_data_t *src);
static void             append_valist           (AdgPath        *path,
                                                 cairo_path_data_type_t type,
                                                 int             length,
                                                 va_list         var_args);


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
    AdgPathPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(path, ADG_TYPE_PATH,
                                                       AdgPathPrivate);

    priv->cp_is_valid = FALSE;
    priv->path = g_array_new(FALSE, FALSE, sizeof(cairo_path_data_t));
    priv->cairo_path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    priv->cairo_path.data = NULL;
    priv->cairo_path.num_data = 0;

    path->priv = priv;
}

static void
finalize(GObject *object)
{
    AdgPath *path = (AdgPath *) object;

    g_array_free(path->priv->path, TRUE);
    clear_cairo_path(path);

    ((GObjectClass *) PARENT_CLASS)->finalize(object);
}


/**
 * adg_path_new:
 *
 * Creates a new path model. The path must be constructed in the @callback
 * function: AdgPath will cache and reuse the cairo_copy_path() returned by
 * the cairo context after the @callback call.
 *
 * Return value: the new model
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
 * Gets a pointer to the cairo path structure of @path. The return value
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
 * Return value: a pointer to the internal #cairo_path_t structure
 *               or %NULL on errors
 **/
const cairo_path_t *
adg_path_get_cairo_path(AdgPath *path)
{
    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    return get_cairo_path(path);
}

/**
 * adg_path_get_current_point:
 * @path: an #AdgPath
 * @x:    return value for x coordinate of the current point
 * @y:    return value for y coordinate of the current point
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
    g_return_if_fail(ADG_IS_PATH(path));

    if (path->priv->cp_is_valid) {
        *x = path->priv->cp.x;
        *y = path->priv->cp.y;
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
 * Return value: whether a current point is defined
 **/
gboolean
adg_path_has_current_point(AdgPath *path)
{
    g_return_val_if_fail(ADG_IS_PATH(path), FALSE);

    return path->priv->cp_is_valid;
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
    g_return_if_fail(ADG_IS_PATH(path));

    g_array_set_size(path->priv->path, 0);
    clear_cairo_path(path);
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
adg_path_append(AdgPath *path, cairo_path_data_type_t type, ...)
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
adg_path_append_valist(AdgPath *path, cairo_path_data_type_t type,
                       va_list var_args)
{
    gint length;

    g_return_if_fail(ADG_IS_PATH(path));

    switch (type) {

    case CAIRO_PATH_CLOSE_PATH:
        g_return_if_fail(path->priv->cp_is_valid);
        length = 1;
        break;

    case CAIRO_PATH_MOVE_TO:
        length = 2;
        break;

    case CAIRO_PATH_LINE_TO:
        g_return_if_fail(path->priv->cp_is_valid);
        length = 2;
        break;

    case CAIRO_PATH_ARC_TO:
        g_return_if_fail(path->priv->cp_is_valid);
        length = 3;
        break;

    case CAIRO_PATH_CURVE_TO:
        g_return_if_fail(path->priv->cp_is_valid);
        length = 4;
        break;

    default:
        g_assert_not_reached();
        return;
    }

    append_valist(path, type, length, var_args);
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
    AdgPair center, p[3];

    g_return_if_fail(ADG_IS_PATH(path));

    center.x = xc;
    center.y = yc;

    cpml_vector_from_angle(&p[0], start, r);
    cpml_vector_from_angle(&p[1], (end-start) / 2, r);
    cpml_vector_from_angle(&p[2], end, r);

    cpml_pair_add(&p[0], &center);
    cpml_pair_add(&p[1], &center);
    cpml_pair_add(&p[2], &center);

    if (!path->priv->cp_is_valid)
        adg_path_append(path, CAIRO_PATH_MOVE_TO, &p[0]);
    else if (p[0].x != path->priv->cp.x || p[0].y != path->priv->cp.y)
        adg_path_append(path, CAIRO_PATH_LINE_TO, &p[0]);

    adg_path_append(path, CAIRO_PATH_ARC_TO, &p[1], &p[2]);
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
        g_print("Invalid path data to dump!\n");
    } else {
        do {
            cpml_segment_dump(&segment);
        } while (cpml_segment_next(&segment));
    }
}


static void
changed(AdgModel *model)
{
    adg_path_clear((AdgPath *) model);

    PARENT_CLASS->changed(model);
}

static void
clear_cairo_path(AdgPath *path)
{
    cairo_path_t *cairo_path = &path->priv->cairo_path;

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
    cairo_path_t *cairo_path;
    const GArray *src;
    GArray *dst;
    const cairo_path_data_t *p_src;
    int i;

    /* Check for cached result */
    cairo_path = &path->priv->cairo_path;
    if (cairo_path->data != NULL)
        return cairo_path;

    src = path->priv->path;
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
append_valist(AdgPath *path, cairo_path_data_type_t type,
              int length, va_list var_args)
{
    AdgPathPrivate *priv;
    cairo_path_data_t item;

    priv = path->priv;

    /* Append the header item */
    item.header.type = type;
    item.header.length = length;
    priv->path = g_array_append_val(priv->path, item);
    priv->cp_is_valid = FALSE;

    /* Append the data items (that is, the AdgPair points) */
    while (--length) {
        cpml_pair_to_cairo(va_arg(var_args, AdgPair *), &item);
        priv->path = g_array_append_val(priv->path, item);
        priv->cp_is_valid = TRUE;
    }

    /* Save the last point as the current point */
    if (priv->cp_is_valid)
        cpml_pair_from_cairo(&priv->cp, &item);

    /* Invalidate cairo_path: should be recomputed */
    clear_cairo_path(path);
}
