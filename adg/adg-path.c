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
 * simple #cairo_path_t struct lying in a single space.
 **/

#include "adg-path.h"
#include "adg-path-private.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgModelClass *) adg_path_parent_class)


static void	finalize		(GObject	*object);
static void     changed                 (AdgModel       *model);
static void     append_item             (AdgPath        *path,
                                         cairo_path_data_type_t type,
                                         int             length,
                                         ...);


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
 * If @path has no current point before the call, this method
 * will behave exactly as adg_path_move_to().
 **/
void
adg_path_line_to(AdgPath *path, gdouble x, gdouble y)
{
    g_return_if_fail(ADG_IS_PATH(path));

    if (!path->priv->cp_is_valid)
        append_item(path, CAIRO_PATH_MOVE_TO, 2, x, y);
    else
        append_item(path, CAIRO_PATH_LINE_TO, 2, x, y);
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
 * behave as if preceded by a call to adg_path_move_to() on (@x1, @y1).
 **/
void
adg_path_curve_to(AdgPath *path, gdouble x1, gdouble y1,
                  gdouble x2, gdouble y2, gdouble x3, gdouble y3)
{
    g_return_if_fail(ADG_IS_PATH(path));

    if (!path->priv->cp_is_valid)
        append_item(path, CAIRO_PATH_MOVE_TO, 1, x1, y1);

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
 * have no effect. 
 **/
void
adg_path_close(AdgPath *path)
{
    g_return_if_fail(ADG_IS_PATH(path));

    if (path->priv->cp_is_valid)
        append_item(path, CAIRO_PATH_CLOSE_PATH, 1);
}


/**
 * adg_path_get_cairo_path:
 * @path: an #AdgPath
 *
 * Gets a pointer to the cairo path structure of @path. The return value
 * is owned by @path and must be considered read-only.
 *
 * Return value: a pointer to the internal #cairo_path_t structure
 **/
const cairo_path_t *
adg_path_get_cairo_path(AdgPath *path)
{
    AdgPathPrivate *priv;

    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    priv = path->priv;

    priv->cairo_path.status = CAIRO_STATUS_SUCCESS;
    priv->cairo_path.data = (cairo_path_data_t *) priv->path->data;
    priv->cairo_path.num_data = priv->path->len;

    return &priv->cairo_path;
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
 * If there is no defined current point, @x and @y will both be set to 0.
 * It is possible to check this in advance with adg_path_has_current_point().
 *
 * Most #AdgPath methods alter the current point: see their description
 * for further details.
 **/
void
adg_path_get_current_point(AdgPath *path, double *x, double *y)
{
    AdgPathPrivate *priv;

    g_return_if_fail(ADG_IS_PATH(path));

    priv = path->priv;

    if (priv->cp_is_valid) {
        *x = priv->cp.x;
        *y = priv->cp.y;
    } else {
        *x = 0.;
        *y = 0.;
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

    cairo_path = (cairo_path_t *) adg_path_get_cairo_path(path);
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
        item.point.x = va_arg(var_args, double);
        item.point.y = va_arg(var_args, double);
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
