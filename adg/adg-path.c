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
#define ARC_MAX_ANGLE   (M_PI / 2.)


static void             finalize                (GObject        *object);
static void             changed                 (AdgModel       *model);
static cairo_path_t *   get_cairo_path          (AdgPath        *path);
static void             append_item             (AdgPath        *path,
                                                 cairo_path_data_type_t type,
                                                 int             length,
                                                 ...);
static void             arc                     (AdgPath        *path,
                                                 gdouble         xc,
                                                 gdouble         yc,
                                                 gdouble         r,
                                                 gdouble         angle1,
                                                 gdouble         angle2,
                                                 gboolean        reverse);


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

    path->priv = priv;
}

static void
finalize(GObject *object)
{
    AdgPath *path = (AdgPath *) object;

    g_array_free(path->priv->path, TRUE);

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
 * @x: return value for x coordinate of the current point
 * @y: return value for y coordinate of the current point
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
 * adg_path_move_to:
 * @path: an #AdgPath
 * @x: the new x coordinate
 * @y: the new y coordinate
 *
 * Begins a new segment. After this call the current point will be (@x, @y).
 **/
void
adg_path_move_to(AdgPath *path, gdouble x, gdouble y)
{
    g_return_if_fail(ADG_IS_PATH(path));

    append_item(path, CAIRO_PATH_MOVE_TO, 2, x, y);
}

/**
 * adg_path_line_to:
 * @path: an #AdgPath
 * @x: the new x coordinate
 * @y: the new y coordinate
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
    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(path->priv->cp_is_valid);

    append_item(path, CAIRO_PATH_LINE_TO, 2, x, y);
}

/**
 * adg_path_arc_to:
 * @path: an #AdgPath
 * @x1: the x coordinate of an intermediate point
 * @y1: the y coordinate of an intermediate point
 * @x2: the x coordinate of the end of the arc
 * @y2: the y coordinate of the end of the arc
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
    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(path->priv->cp_is_valid);

    append_item(path, CAIRO_PATH_ARC_TO, 3, x1, y1, x2, y2);
}

/**
 * adg_path_curve_to:
 * @path: an #AdgPath
 * @x1: the x coordinate of the first control point
 * @y1: the y coordinate of the first control point
 * @x2: the x coordinate of the second control point
 * @y2: the y coordinate of the second control point
 * @x3: the x coordinate of the end of the curve
 * @y3: the y coordinate of the end of the curve
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
    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(path->priv->cp_is_valid);

    append_item(path, CAIRO_PATH_CURVE_TO, 4, x1, y1, x2, y2, x3, y3);
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
 * cairo_line_to() with the coordinates of the segment starting point.
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
    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(path->priv->cp_is_valid);

    append_item(path, CAIRO_PATH_CLOSE_PATH, 1);
}

/**
 * adg_path_arc
 * @path: an #AdgPath
 * @xc: x position of the center of the arc
 * @yc: y position of the center of the arc
 * @r: the radius of the arc
 * @angle1: the start angle, in radians
 * @angle2: the end angle, in radians
 *
 * Adds an arc to @path by explicitely specify start and end angle
 * (@angle1 and @angle2) and the radius (@r). After this call
 * the current point will be the computed end point of the arc.
 * The arc is computed in clockwise direction from @angle1.
 *
 * If @path has no current point before this call, a %CAIRO_PATH_MOVE_TO
 * to the start point of the arc will be automatically prepended
 * to the arc. Instead, if the start point of the arc is different
 * from the current point, a %CAIRO_PATH_LINE_TO will be prepended.
 **/
void
adg_path_arc(AdgPath *path, gdouble xc, gdouble yc, gdouble r,
             gdouble angle1, gdouble angle2)
{
    g_return_if_fail(ADG_IS_PATH(path));

    arc(path, xc, yc, r, angle1, angle2, FALSE);
}

/**
 * adg_path_arc_negative:
 * @path: an #AdgPath
 * @xc: X position of the center of the arc
 * @yc: Y position of the center of the arc
 * @r: the r of the arc
 * @angle1: the start angle, in radians
 * @angle2: the end angle, in radians
 *
 * Adds an arc to @path by explicitely specify start and end angle
 * (@angle1 and @angle2) and the radius (@r). After this call
 * the current point will be the computed end point of the arc.
 * This function is similar to adg_path_arc(): the only difference
 * is the angle is computed in counter-clockwise direction.
 *
 * If @path has no current point before this call, a %CAIRO_PATH_MOVE_TO
 * to the start point of the arc will be automatically prepended
 * to the arc. Instead, if the start point of the arc is different
 * from the current point, a %CAIRO_PATH_LINE_TO will be prepended.
 **/
void
adg_path_arc_negative (AdgPath *path, gdouble xc, gdouble yc, gdouble r,
                       gdouble angle1, gdouble angle2)
{
    g_return_if_fail(ADG_IS_PATH(path));

    arc(path, xc, yc, r, angle2, angle1, TRUE);
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
    AdgPath *path = (AdgPath *) model;

    g_array_set_size(path->priv->path, 0);
    path->priv->cairo_path.status = CAIRO_STATUS_INVALID_PATH_DATA;

    PARENT_CLASS->changed(model);
}

static cairo_path_t *
get_cairo_path(AdgPath *path)
{
    AdgPathPrivate *priv = path->priv;

    priv->cairo_path.status = CAIRO_STATUS_SUCCESS;
    priv->cairo_path.data = (cairo_path_data_t *) priv->path->data;
    priv->cairo_path.num_data = priv->path->len;
}

static void
append_item(AdgPath *path, cairo_path_data_type_t type, int length, ...)
{
    AdgPathPrivate *priv;
    cairo_path_data_t item;
    va_list var_args;
    gint n;

    priv = path->priv;
    n = length;

    /* Append the header item */
    item.header.type = type;
    item.header.length = length;
    priv->path = g_array_append_val(priv->path, item);

    va_start(var_args, length);

    /* Append the data items (that is, the points) */
    while (-- n) {
        item.point.x = va_arg(var_args, gdouble);
        item.point.y = va_arg(var_args, gdouble);
        priv->path = g_array_append_val(priv->path, item);
    }

    va_end(var_args);

    priv->cp_is_valid = length > 1;
    if (priv->cp_is_valid) {
        /* Save the last point as the current point */
        priv->cp.x = item.point.x;
        priv->cp.y = item.point.y;
    }
}

static void
arc(AdgPath *path, gdouble xc, gdouble yc, gdouble r,
    gdouble angle1, gdouble angle2, gboolean reverse)
{
    gdouble angle12;
    CpmlPair center, p0, p1, p2;

    if (r <= 0. || angle1 == angle2)
	return;

    center.x = xc;
    center.y = yc;
    angle12 = (angle1 + angle2) / 2.;
    if (reverse)
        angle12 += M_PI;

    cpml_vector_from_angle(&p0, angle1, r);
    cpml_vector_from_angle(&p1, angle12, r);
    cpml_vector_from_angle(&p2, angle2, r);

    cpml_pair_add(&p0, &center);
    cpml_pair_add(&p1, &center);
    cpml_pair_add(&p2, &center);

    if (!path->priv->cp_is_valid)
        adg_path_move_to(path, p0.x, p0.y);
    else if (path->priv->cp.x != p0.x || path->priv->cp.y != p0.y)
        adg_path_line_to(path, p0.x, p0.y);

    adg_path_arc_to(path, p1.x, p1.y, p2.x, p2.y);
}
