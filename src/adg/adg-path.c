/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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
 * The #AdgPath model represents a virtual #cairo_path_t: this class
 * implements methods to create the path and provides additional
 * operations specific to technical drawings.
 *
 * #AdgPath overrides the <function>get_cairo_path</function> method
 * of the parent #AdgTrail class, avoiding the need of an
 * #AdgTrailCallback. The path is constructed programmatically: keep
 * in mind any method that modifies the path will invalidate the
 * #cairo_path_t returned by adg_trail_get_cairo_path().
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
 * Furthermore, after cairo_close_path() a %CPML_MOVE primitive to
 * the starting point of the segment is automatically added by cairo;
 * in ADG, after an adg_path_close() the current point is unset.
 *
 * Since: 1.0
 **/

/**
 * AdgPath:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-model.h"
#include "adg-trail.h"

#include "adg-path.h"
#include "adg-path-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_path_parent_class)
#define _ADG_OLD_MODEL_CLASS   ((AdgModelClass *) adg_path_parent_class)

#define REMAPPED(ptr, from, to) \
    (ptr) == NULL ? NULL : \
        (gpointer) ((guint8 *) (ptr) - (guint8 *) (from) + (guint8 *) (to))


G_DEFINE_TYPE(AdgPath, adg_path, ADG_TYPE_TRAIL)


static void             _adg_finalize           (GObject        *object);
static void             _adg_clear              (AdgModel       *model);
static void             _adg_clear_parent       (AdgModel       *model);
static void             _adg_changed            (AdgModel       *model);
static cairo_path_t *   _adg_get_cairo_path     (AdgTrail       *trail);
static cairo_path_t *   _adg_read_cairo_path    (AdgPath        *path);
static gint             _adg_primitive_length   (CpmlPrimitiveType type);
static void             _adg_primitive_remap    (CpmlPrimitive  *primitive,
                                                 gpointer        to,
                                                 const CpmlPrimitive
                                                                *old,
                                                 gconstpointer   from);
static void             _adg_rescan             (AdgPath        *path);
static void             _adg_append_primitive   (AdgPath        *path,
                                                 CpmlPrimitive  *primitive);
static void             _adg_clear_operation    (AdgPath        *path);
static gboolean         _adg_append_operation   (AdgPath        *path,
                                                 gint            action,
                                                 ...);
static void             _adg_do_operation       (AdgPath        *path,
                                                 cairo_path_data_t
                                                                *path_data);
static void             _adg_do_action          (AdgPath        *path,
                                                 AdgAction       action,
                                                 CpmlPrimitive  *primitive);
static void             _adg_do_chamfer         (AdgPath        *path,
                                                 CpmlPrimitive  *current);
static void             _adg_do_fillet          (AdgPath        *path,
                                                 CpmlPrimitive  *current);
static gboolean         _adg_is_convex          (const CpmlPrimitive
                                                                *primitive1,
                                                 const CpmlPrimitive
                                                                *primitive2);
static const gchar *    _adg_action_name        (AdgAction       action);
static void             _adg_get_named_pair     (AdgModel       *model,
                                                 const gchar    *name,
                                                 CpmlPair       *pair,
                                                 gpointer        user_data);
static void             _adg_dup_reverse_named_pairs
                                                (AdgModel       *model,
                                                 const cairo_matrix_t
                                                                *matrix);


static void
adg_path_class_init(AdgPathClass *klass)
{
    GObjectClass *gobject_class;
    AdgModelClass *model_class;
    AdgTrailClass *trail_class;

    gobject_class = (GObjectClass *) klass;
    model_class = (AdgModelClass *) klass;
    trail_class = (AdgTrailClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgPathPrivate));

    gobject_class->finalize = _adg_finalize;

    model_class->clear = _adg_clear;
    model_class->changed = _adg_changed;

    trail_class->get_cairo_path = _adg_get_cairo_path;
}

static void
adg_path_init(AdgPath *path)
{
    AdgPathPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(path, ADG_TYPE_PATH,
                                                       AdgPathPrivate);

    data->cp_is_valid = FALSE;
    data->cp.x = 0;
    data->cp.y = 0;
    data->cairo.path.status = CAIRO_STATUS_INVALID_PATH_DATA;
    data->cairo.path.data = NULL;
    data->cairo.path.num_data = 0;
    data->cairo.array = g_array_new(FALSE, FALSE, sizeof(cairo_path_data_t));
    data->last.segment = NULL;
    data->last.org = NULL;
    data->last.data = NULL;
    data->over.segment = NULL;
    data->over.org = NULL;
    data->over.data = NULL;
    data->operation.action = ADG_ACTION_NONE;

    path->data = data;
}

static void
_adg_finalize(GObject *object)
{
    AdgPath *path;
    AdgPathPrivate *data;

    path = (AdgPath *) object;
    data = path->data;

    g_array_free(data->cairo.array, TRUE);
    _adg_clear_operation(path);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}


/**
 * adg_path_new:
 *
 * Creates a new path model. The path should be constructed
 * programmatically by using the methods provided by #AdgPath.
 *
 * Returns: the newly created path model
 *
 * Since: 1.0
 **/
AdgPath *
adg_path_new(void)
{
    return g_object_new(ADG_TYPE_PATH, NULL);
}

/**
 * adg_path_get_current_point:
 * @path: an #AdgPath
 *
 * Gets the current point of @path, which is conceptually the
 * final point reached by the path so far.
 *
 * If there is no defined current point, <constant>NULL</constant> is returned.
 * It is possible to check this in advance with
 * adg_path_has_current_point().
 *
 * Most #AdgPath methods alter the current point and most of them
 * expect a current point to be defined otherwise will fail triggering
 * a warning. Check the description of every method for specific details.
 *
 * Returns: (transfer none): the current point or <constant>NULL</constant> on no current point set or errors.
 *
 * Since: 1.0
 **/
const CpmlPair *
adg_path_get_current_point(AdgPath *path)
{
    AdgPathPrivate *data;

    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    data = path->data;

    if (!data->cp_is_valid)
        return NULL;

    return &data->cp;
}

/**
 * adg_path_has_current_point:
 * @path: an #AdgPath
 *
 * Returns whether a current point is defined on @path.
 * See adg_path_get_current_point() for details on the current point.
 *
 * Returns: whether a current point is defined
 *
 * Since: 1.0
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
 * adg_path_last_primitive:
 * @path: an #AdgPath
 *
 * Gets the last primitive appended to @path. The #CPML_MOVE type is
 * not considered a full-fledged primitive, i.e. adg_path_move_to()
 * or similar does not change the last primitive.
 *
 * The returned struct is owned by @path and should not be freed or
 * modified.
 *
 * Returns: (transfer none): a pointer to the last appended primitive or <constant>NULL</constant> on no last primitive or on errors.
 *
 * Since: 1.0
 **/
const CpmlPrimitive *
adg_path_last_primitive(AdgPath *path)
{
    AdgPathPrivate *data;

    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    data = path->data;

    /* Directly return NULL instead of returning an undefined primitive */
    if (data->last.org == NULL || data->last.data == NULL)
        return NULL;

    return &data->last;
}

/**
 * adg_path_over_primitive:
 * @path: an #AdgPath
 *
 * Gets the primitive before the last one appended to @path. The
 * "over" term comes from forth, where the <emphasis>OVER</emphasis>
 * operator works on the stack in the same way as
 * adg_path_over_primitive() works on @path. The #CPML_MOVE type is
 * not considered a full-fledged primitive, i.e. adg_path_move_to()
 * or similar does not change the over primitive.
 *
 * The returned struct is owned by @path and should not be freed or
 * modified.
 *
 * Returns: (transfer none): a pointer to the primitive before the last appended one or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const CpmlPrimitive *
adg_path_over_primitive(AdgPath *path)
{
    AdgPathPrivate *data;

    g_return_val_if_fail(ADG_IS_PATH(path), NULL);

    data = path->data;

    /* Directly return NULL instead of returning an undefined primitive */
    if (data->over.org == NULL || data->over.data == NULL)
        return NULL;

    return &data->over;
}

/**
 * adg_path_append:
 * @path: an #AdgPath
 * @type: (type CpmlPrimitiveType): a #cairo_data_type_t value
 * @...:  point data, specified as #CpmlPair pointers
 *
 * Generic method to append a primitive to @path. The number of #CpmlPair
 * pointers to pass as @Varargs depends on @type: %CPML_CLOSE does not
 * require any pair, %CPML_MOVE and %CPML_LINE require one pair,
 * %CPML_ARC two pairs, %CPML_CURVE three pairs and so on.
 *
 * All the needed pairs must be not <constant>NULL</constant> pointers,
 * otherwise the function will fail. The pairs in excess, if any, are ignored.
 *
 * Since: 1.0
 **/
void
adg_path_append(AdgPath *path, gint type, ...)
{
    va_list var_args;

    va_start(var_args, type);
    adg_path_append_valist(path, (CpmlPrimitiveType) type, var_args);
    va_end(var_args);
}

/**
 * adg_path_append_valist:
 * @path:     an #AdgPath
 * @type:     a #cairo_data_type_t value
 * @var_args: point data, specified as #CpmlPair pointers
 *
 * va_list version of adg_path_append().
 *
 * Since: 1.0
 **/
void
adg_path_append_valist(AdgPath *path, CpmlPrimitiveType type, va_list var_args)
{
    GPtrArray *array;
    const CpmlPair *pair;
    gint length;

    length = _adg_primitive_length(type);
    if (length == 0)
        return;

    array = g_ptr_array_sized_new(4);
    while (-- length) {
        pair = va_arg(var_args, const CpmlPair *);
        if (pair == NULL) {
            g_ptr_array_free(array, TRUE);
            g_return_if_reached();
            return;
        }
        g_ptr_array_add(array, (gpointer) pair);
    }

    /* The array must be NULL terminated */
    g_ptr_array_add(array, NULL);

    adg_path_append_array(path, type, (const CpmlPair **) array->pdata);

    g_ptr_array_free(array, TRUE);
}

/**
 * adg_path_append_array: (rename-to adg_path_append)
 * @path:  an #AdgPath
 * @type:  a #cairo_data_type_t value
 * @pairs: (array zero-terminated=1) (element-type Cpml.Pair) (transfer none): point data, specified as a <constant>NULL</constant> terminated array of #CpmlPair pointers.
 *
 * A bindingable version of adg_path_append() that uses a
 * <constant>NULL</constant> terminated array of pairs instead of variable
 * argument list and friends.
 *
 * Furthermore, because of the list is <constant>NULL</constant> terminated,
 * an arbitrary number of pairs can be passed in @pairs. This allows to embed
 * in a primitive element more data pairs than requested, something impossible
 * to do with adg_path_append() and adg_path_append_valist().
 *
 * Since: 1.0
 **/
void
adg_path_append_array(AdgPath *path, CpmlPrimitiveType type,
                      const CpmlPair **pairs)
{
    gint length;
    GArray *array;
    const CpmlPair **pair;
    cairo_path_data_t path_data;

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(pairs != NULL);

    length = _adg_primitive_length(type);
    if (length == 0)
        return;

    array = g_array_new(FALSE, FALSE, sizeof(path_data));
    for (pair = pairs; *pair != NULL; ++ pair) {
        cpml_pair_to_cairo(*pair, &path_data);
        g_array_append_val(array, path_data);
    }

    if (array->len < length - 1) {
        /* Not enough pairs have been provided */
        g_warning(_("%s: null pair caught while parsing arguments"), G_STRLOC);
    } else {
        AdgPathPrivate *data;
        CpmlPrimitive primitive;
        cairo_path_data_t org;

        /* Save a copy of the current point as primitive origin */
        data = path->data;
        cpml_pair_to_cairo(&data->cp, &org);

        /* Prepend the cairo header */
        path_data.header.type = type;
        path_data.header.length = array->len + 1;
        g_array_prepend_val(array, path_data);

        /* Append a new primitive to @path */
        primitive.segment = NULL;
        primitive.org = &org;
        primitive.data = (cairo_path_data_t *) array->data;
        _adg_append_primitive(path, &primitive);
    }

    g_array_free(array, TRUE);
}


/**
 * adg_path_append_primitive:
 * @path:      an #AdgPath
 * @primitive: the #CpmlPrimitive to append
 *
 * Appends @primitive to @path. The primitive to add is considered the
 * continuation of the current path so the <structfield>org</structfield>
 * component of @primitive is not used. Anyway the current point is
 * checked against it: they must be equal or the function will fail
 * without further processing.
 *
 * Since: 1.0
 **/
void
adg_path_append_primitive(AdgPath *path, const CpmlPrimitive *primitive)
{
    AdgPathPrivate *data;
    CpmlPrimitive *primitive_dup;

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(primitive != NULL);
    g_return_if_fail(primitive->org != NULL);
    g_return_if_fail(primitive->data != NULL);

    data = path->data;

    g_return_if_fail(primitive->org->point.x == data->cp.x &&
                     primitive->org->point.y == data->cp.y);

    /* The primitive data could be modified by pending operations:
     * work on a copy */
    primitive_dup = cpml_primitive_deep_dup(primitive);

    _adg_append_primitive(path, primitive_dup);

    g_free(primitive_dup);
}

/**
 * adg_path_append_segment:
 * @path:    an #AdgPath
 * @segment: the #CpmlSegment to append
 *
 * Appends @segment to @path.
 *
 * Since: 1.0
 **/
void
adg_path_append_segment(AdgPath *path, const CpmlSegment *segment)
{
    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(segment != NULL);

    if (segment->num_data > 0) {
        AdgPathPrivate *data;

        g_return_if_fail(segment->data != NULL);

        data = path->data;

        _adg_clear_parent((AdgModel *) path);
        data->cairo.array = g_array_append_vals(data->cairo.array,
                                                segment->data, segment->num_data);
        _adg_rescan(path);
    }
}

/**
 * adg_path_append_cairo_path:
 * @path:       an #AdgPath
 * @cairo_path: the #cairo_path_t path to append
 *
 * Appends a whole #cairo_path_t to @path.
 *
 * Since: 1.0
 **/
void
adg_path_append_cairo_path(AdgPath *path, const cairo_path_t *cairo_path)
{
    AdgPathPrivate *data;

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(cairo_path != NULL);

    data = path->data;

    _adg_clear_parent((AdgModel *) path);
    data->cairo.array = g_array_append_vals(data->cairo.array,
                                            cairo_path->data,
                                            cairo_path->num_data);
    _adg_rescan(path);
}

/**
 * adg_path_append_trail:
 * @path:  an #AdgPath
 * @trail: an #AdgTrail instance
 *
 * Appends the content of @trail to @path. It is similar to
 * adg_path_append_cairo_path() but it also appends to @path the named pairs
 * eventually defined in @trail.
 *
 * Since: 1.0
 **/
void
adg_path_append_trail(AdgPath *path, AdgTrail *trail)
{
    GSList *named_pairs;
    AdgNamedPair *named_pair;

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(ADG_IS_TRAIL(trail));

    adg_path_append_cairo_path(path, adg_trail_get_cairo_path(trail));

    /* Populate named_pairs with all the named pairs of trail */
    named_pairs = NULL;
    adg_model_foreach_named_pair((AdgModel *)trail,
                                 _adg_get_named_pair, &named_pairs);

    /* Readd the pairs to path */
    while (named_pairs) {
        named_pair = (AdgNamedPair *) named_pairs->data;
        adg_model_set_named_pair((AdgModel *) path,
                                 named_pair->name, &named_pair->pair);
        named_pairs = g_slist_delete_link(named_pairs, named_pairs);
    }
}

/**
 * adg_path_move_to:
 * @path: an #AdgPath
 * @pair: the destination coordinates
 *
 * Begins a new segment. After this call the current point will be @pair.
 *
 * Since: 1.0
 **/
void
adg_path_move_to(AdgPath *path, const CpmlPair *pair)
{
    adg_path_append(path, CPML_MOVE, pair);
}

/**
 * adg_path_move_to_explicit:
 * @path: an #AdgPath
 * @x:    the new x coordinate
 * @y:    the new y coordinate
 *
 * Convenient function to call adg_path_move_to() using explicit
 * coordinates instead of #CpmlPair.
 *
 * Since: 1.0
 **/
void
adg_path_move_to_explicit(AdgPath *path, gdouble x, gdouble y)
{
    CpmlPair p;

    p.x = x;
    p.y = y;

    adg_path_append(path, CPML_MOVE, &p);
}

/**
 * adg_path_line_to:
 * @path: an #AdgPath
 * @pair: the destination coordinates
 *
 * Adds a line to @path from the current point to @pair. After this
 * call the current point will be @pair.
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 *
 * Since: 1.0
 **/
void
adg_path_line_to(AdgPath *path, const CpmlPair *pair)
{
    adg_path_append(path, CPML_LINE, pair);
}

/**
 * adg_path_line_to_explicit:
 * @path: an #AdgPath
 * @x:    the new x coordinate
 * @y:    the new y coordinate
 *
 * Convenient function to call adg_path_line_to() using explicit
 * coordinates instead of #CpmlPair.
 *
 * Since: 1.0
 **/
void
adg_path_line_to_explicit(AdgPath *path, gdouble x, gdouble y)
{
    CpmlPair p;

    p.x = x;
    p.y = y;

    adg_path_append(path, CPML_LINE, &p);
}

/**
 * adg_path_arc_to:
 * @path:     an #AdgPath
 * @throught: an arbitrary point on the arc
 * @pair:     the destination coordinates
 *
 * Adds an arc to the path from the current point to @pair, passing
 * throught @throught. After this call the current point will be @pair.
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 *
 * Since: 1.0
 **/
void
adg_path_arc_to(AdgPath *path, const CpmlPair *throught, const CpmlPair *pair)
{
    adg_path_append(path, CPML_ARC, throught, pair);
}

/**
 * adg_path_arc_to_explicit:
 * @path: an #AdgPath
 * @x1:   the x coordinate of an intermediate point
 * @y1:   the y coordinate of an intermediate point
 * @x2:   the x coordinate of the end of the arc
 * @y2:   the y coordinate of the end of the arc
 *
 * Convenient function to call adg_path_arc_to() using explicit
 * coordinates instead of #CpmlPair.
 *
 * Since: 1.0
 **/
void
adg_path_arc_to_explicit(AdgPath *path, gdouble x1, gdouble y1,
                         gdouble x2, gdouble y2)
{
    CpmlPair p[2];

    p[0].x = x1;
    p[0].y = y1;
    p[1].x = x2;
    p[1].y = y2;

    adg_path_append(path, CPML_ARC, &p[0], &p[1]);
}

/**
 * adg_path_curve_to:
 * @path: an #AdgPath
 * @control1: the first control point of the curve
 * @control2: the second control point of the curve
 * @pair:     the destination coordinates
 *
 * Adds a cubic Bézier curve to the path from the current point to
 * position @pair, using @control1 and @control2 as control points.
 * After this call the current point will be @pair.
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 *
 * Since: 1.0
 **/
void
adg_path_curve_to(AdgPath *path, const CpmlPair *control1,
                  const CpmlPair *control2, const CpmlPair *pair)
{
    adg_path_append(path, CPML_CURVE, control1, control2, pair);
}

/**
 * adg_path_curve_to_explicit:
 * @path: an #AdgPath
 * @x1:   the x coordinate of the first control point
 * @y1:   the y coordinate of the first control point
 * @x2:   the x coordinate of the second control point
 * @y2:   the y coordinate of the second control point
 * @x3:   the x coordinate of the end of the curve
 * @y3:   the y coordinate of the end of the curve
 *
 * Convenient function to call adg_path_curve_to() using explicit
 * coordinates instead of #CpmlPair.
 *
 * Since: 1.0
 **/
void
adg_path_curve_to_explicit(AdgPath *path, gdouble x1, gdouble y1,
                           gdouble x2, gdouble y2, gdouble x3, gdouble y3)
{
    CpmlPair p[3];

    p[0].x = x1;
    p[0].y = y1;
    p[1].x = x2;
    p[1].y = y2;
    p[2].x = x3;
    p[2].y = y3;

    adg_path_append(path, CPML_CURVE, &p[0], &p[1], &p[2]);
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
 * adg_path_line_to() with the coordinates of the segment starting
 * point. When a closed segment is stroked, there are no caps on the
 * ends. Instead, there is a line join connecting the final and
 * initial primitive of the segment.
 *
 * If @path has no current point before this call, this function will
 * trigger a warning without other effect.
 *
 * Since: 1.0
 **/
void
adg_path_close(AdgPath *path)
{
    adg_path_append(path, CPML_CLOSE);
}

/**
 * adg_path_arc:
 * @path:  an #AdgPath
 * @center: coordinates of the center of the arc
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
 * current point before the call a %CPML_MOVE to the start point of
 * the arc will be automatically prepended to the arc. If @path has a
 * current point, a %CPML_LINE to the start point of the arc will be
 * used instead of the "move to" primitive.
 *
 * Since: 1.0
 **/
void
adg_path_arc(AdgPath *path, const CpmlPair *center, gdouble r,
             gdouble start, gdouble end)
{
    AdgPathPrivate *data;
    CpmlPair p[3];

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(center != NULL);

    data = path->data;
    cpml_vector_from_angle(&p[0], start);
    cpml_vector_from_angle(&p[1], (start+end) / 2);
    cpml_vector_from_angle(&p[2], end);

    cpml_vector_set_length(&p[0], r);
    cpml_vector_set_length(&p[1], r);
    cpml_vector_set_length(&p[2], r);

    p[0].x += center->x;
    p[0].y += center->y;
    p[1].x += center->x;
    p[1].y += center->y;
    p[2].x += center->x;
    p[2].y += center->y;

    if (!data->cp_is_valid)
        adg_path_append(path, CPML_MOVE, &p[0]);
    else if (p[0].x != data->cp.x || p[0].y != data->cp.y)
        adg_path_append(path, CPML_LINE, &p[0]);

    adg_path_append(path, CPML_ARC, &p[1], &p[2]);
}

/**
 * adg_path_arc_explicit:
 * @path:  an #AdgPath
 * @xc:    x position of the center of the arc
 * @yc:    y position of the center of the arc
 * @r:     the radius of the arc
 * @start: the start angle, in radians
 * @end:   the end angle, in radians
 *
 * Convenient function to call adg_path_arc() using explicit
 * coordinates instead of #CpmlPair.
 *
 * Since: 1.0
 **/
void
adg_path_arc_explicit(AdgPath *path, gdouble xc, gdouble yc, gdouble r,
                      gdouble start, gdouble end)
{
    CpmlPair center;

    center.x = xc;
    center.y = yc;

    adg_path_arc(path, &center, r, start, end);
}

/**
 * adg_path_chamfer:
 * @path:   an #AdgPath
 * @delta1: the distance from the intersection point of the current primitive
 * @delta2: the distance from the intersection point of the next primitive
 *
 * A binary action that generates a chamfer between two primitives.
 * The first primitive involved is the current primitive, the second will
 * be the next primitive appended to @path after this call. The second
 * primitive is required: if the chamfer operation is not properly
 * terminated (by not providing the second primitive), any API accessing
 * the path in reading mode will raise a warning.
 *
 * An exception is a chamfer after a %CPML_CLOSE primitive. In this case,
 * the second primitive is not required: the current close path is used
 * as first operand while the first primitive of the current segment is
 * used as second operand.
 *
 * The chamfer operation requires two lengths: @delta1 specifies the
 * "quantity" to trim on the first primitive while @delta2 is the same
 * applied on the second primitive. The term "quantity" means the length
 * of the portion to cut out from the original primitive (that is the
 * primitive as would be without the chamfer).
 *
 * Since: 1.0
 **/
void
adg_path_chamfer(AdgPath *path, gdouble delta1, gdouble delta2)
{
    g_return_if_fail(ADG_IS_PATH(path));

    if (!_adg_append_operation(path, ADG_ACTION_CHAMFER, delta1, delta2))
        return;
}

/**
 * adg_path_fillet:
 * @path:   an #AdgPath
 * @radius: the radius of the fillet
 *
 * A binary action that joins to primitives with an arc.
 * The first primitive involved is the current primitive, the second will
 * be the next primitive appended to @path after this call. The second
 * primitive is required: if the fillet operation is not properly
 * terminated (by not providing the second primitive), any API accessing
 * the path in reading mode will raise a warning.
 *
 * An exception is a fillet after a %CPML_CLOSE primitive. In this case,
 * the second primitive is not required: the current close path is used
 * as first operand while the first primitive of the current segment is
 * used as second operand.
 *
 * Since: 1.0
 **/
void
adg_path_fillet(AdgPath *path, gdouble radius)
{
    g_return_if_fail(ADG_IS_PATH(path));

    if (!_adg_append_operation(path, ADG_ACTION_FILLET, radius))
        return;
}

/**
 * adg_path_join:
 * @path: an #AdgPath
 *
 * Joins all the segments of @path. After the call there will be only one
 * single segment.
 *
 * This operation is roughly equivalent to converting embedded %CPML_MOVE
 * primitives into %CPML_LINE ones.
 *
 * Since: 1.0
 **/
void
adg_path_join(AdgPath *path)
{
    cairo_path_t *cairo_path;
    cairo_path_data_t *data;
    gboolean pen_down;

    g_return_if_fail(ADG_IS_PATH(path));

    cairo_path = _adg_read_cairo_path(path);
    pen_down = FALSE;
    data = cairo_path->data;

    while (data - cairo_path->data < cairo_path->num_data) {
        if (data->header.type != CPML_MOVE) {
            pen_down = TRUE;
        } else if (pen_down) {
            data->header.type = CPML_LINE;
        }
        data += data->header.length;
    }
}

/**
 * adg_path_reflect:
 * @path:                 an #AdgPath
 * @vector: (allow-none): the slope of the axis
 *
 * Reflects the first segment or @path around the axis passing
 * throught (0, 0) and with a @vector slope. The internal segment
 * is duplicated and the proper transformation (computed from
 * @vector) to mirror the segment is applied on all its points.
 * The result is then reversed with cpml_segment_reverse() and
 * appended to the original path with adg_path_append_segment().
 *
 * For convenience, if @vector is <constant>NULL</constant> the
 * path is reversed around the x axis <constant>(y = 0)</constant>.
 *
 * Since: 1.0
 **/
void
adg_path_reflect(AdgPath *path, const CpmlVector *vector)
{
    AdgModel *model;
    AdgTrail *trail;
    cairo_matrix_t matrix;
    CpmlSegment segment, *dup_segment;
    gint n;

    g_return_if_fail(ADG_IS_PATH(path));
    g_return_if_fail(vector == NULL || vector->x != 0 || vector->y != 0);

    model = (AdgModel *) path;
    trail = (AdgTrail *) path;

    if (vector == NULL) {
        cairo_matrix_init_scale(&matrix, 1, -1);
    } else {
        CpmlVector slope;
        gdouble cos2angle, sin2angle;

        cpml_pair_copy(&slope, vector);
        cpml_vector_set_length(&slope, 1);

        if (slope.x == 0 && slope.y == 0) {
            g_warning(_("%s: the axis of the reflection is not known"),
                      G_STRLOC);
            return;
        }

        sin2angle = 2. * vector->x * vector->y;
        cos2angle = 2. * vector->x * vector->x - 1;

        cairo_matrix_init(&matrix, cos2angle, sin2angle,
                          sin2angle, -cos2angle, 0, 0);
    }

    for (n = adg_trail_n_segments(trail); n > 0; --n) {
        adg_trail_put_segment(trail, n, &segment);

        /* No need to reverse an empty segment */
        if (segment.num_data == 0 || segment.num_data == 0)
            continue;

        dup_segment = cpml_segment_deep_dup(&segment);
        if (dup_segment == NULL)
            return;

        cpml_segment_reverse(dup_segment);
        cpml_segment_transform(dup_segment, &matrix);
        dup_segment->data[0].header.type = CPML_MOVE;

        adg_path_append_segment(path, dup_segment);

        g_free(dup_segment);
    }

    _adg_dup_reverse_named_pairs(model, &matrix);
}

/**
 * adg_path_reflect_explicit:
 * @path: an #AdgPath
 * @x:    the vector x component
 * @y:    the vector y component
 *
 * Convenient function to call adg_path_reflect() using explicit
 * vector components instead of #CpmlVector.
 *
 * Since: 1.0
 **/
void
adg_path_reflect_explicit(AdgPath *path, gdouble x, gdouble y)
{
    CpmlVector vector;

    vector.x = x;
    vector.y = y;

    adg_path_reflect(path, &vector);
}


static void
_adg_clear(AdgModel *model)
{
    AdgPath *path;
    AdgPathPrivate *data;

    path = (AdgPath *) model;
    data = path->data;

    g_array_set_size(data->cairo.array, 0);
    _adg_clear_operation(path);
    _adg_clear_parent(model);
}

static void
_adg_clear_parent(AdgModel *model)
{
    if (_ADG_OLD_MODEL_CLASS->clear)
        _ADG_OLD_MODEL_CLASS->clear(model);
}

static void
_adg_changed(AdgModel *model)
{
    _adg_clear_parent(model);

    if (_ADG_OLD_MODEL_CLASS->changed)
        _ADG_OLD_MODEL_CLASS->changed(model);
}

static cairo_path_t *
_adg_get_cairo_path(AdgTrail *trail)
{
    _adg_clear_parent((AdgModel *) trail);
    return _adg_read_cairo_path((AdgPath *) trail);
}

static cairo_path_t *
_adg_read_cairo_path(AdgPath *path)
{
    AdgPathPrivate *data = path->data;
    cairo_path_t *cairo_path = &data->cairo.path;
    GArray *array = data->cairo.array;

    /* Always regenerate the cairo_path_t as it is a trivial operation */
    cairo_path->status = CAIRO_STATUS_SUCCESS;
    cairo_path->data = (cairo_path_data_t *) array->data;
    cairo_path->num_data = array->len;

    return cairo_path;
}

static gint
_adg_primitive_length(CpmlPrimitiveType type)
{
    switch (type) {

    case CPML_CLOSE:
        return 1;

    case CPML_MOVE:
        return 2;

    default:
        return cpml_primitive_type_get_n_points(type);
    }
}

static void
_adg_primitive_remap(CpmlPrimitive *primitive, gpointer to,
                     const CpmlPrimitive *old, gconstpointer from)
{
    primitive->org =     REMAPPED(old->org,     from, to);
    primitive->segment = REMAPPED(old->segment, from, to);
    primitive->data =    REMAPPED(old->data,    from, to);
}

static void
_adg_rescan(AdgPath *path)
{
    AdgPathPrivate *data;
    CpmlSegment segment;
    CpmlPrimitive current, *last, *over;

    data = path->data;
    last = &data->last;
    over = &data->over;

    last->segment = NULL;
    last->org = NULL;
    last->data = NULL;
    over->segment = NULL;
    over->org = NULL;
    over->data = NULL;

    /* When no data is present, just bail out */
    if (! cpml_segment_from_cairo(&segment, _adg_read_cairo_path(path)))
        return;

    do {
        cpml_primitive_from_segment(&current, &segment);
        do {
            cpml_primitive_copy(over, last);
            cpml_primitive_copy(last, &current);
        } while (cpml_primitive_next(&current));
    } while (cpml_segment_next(&segment));
}

static void
_adg_append_primitive(AdgPath *path, CpmlPrimitive *current)
{
    AdgPathPrivate *data;
    cairo_path_data_t *path_data;
    CpmlPrimitiveType type;
    int length;
    gconstpointer old_data;
    gpointer new_data;

    data = path->data;
    path_data = current->data;
    length = path_data->header.length;
    type = path_data->header.type;

    /* Execute any pending operation */
    _adg_do_operation(path, path_data);

    /* Append the path data to the internal path array */
    old_data = (data->cairo.array)->data;
    data->cairo.array = g_array_append_vals(data->cairo.array,
                                            path_data, length);
    new_data = (data->cairo.array)->data;

    /* Set path data to point to the recently appended cairo_path_data_t
     * primitive: the first struct is the header */
    path_data = (cairo_path_data_t *) new_data +
                (data->cairo.array)->len - length;

    if (type == CPML_MOVE) {
        /* Remap last and over, but do not change their content */
        _adg_primitive_remap(&data->last, new_data, &data->last, old_data);
        _adg_primitive_remap(&data->over, new_data, &data->over, old_data);
    } else {
        /* Store the last primitive into over */
        _adg_primitive_remap(&data->over, new_data, &data->last, old_data);

        /* Set the last primitive for subsequent binary operations */
        /* TODO: the assumption path_data - 1 is the last point is not true
         * e.g. when there are embeeded data in primitives */
        data->last.org = data->cp_is_valid ? path_data - 1 : NULL;
        data->last.segment = NULL;
        data->last.data = path_data;
    }

    data->cp_is_valid = type != CPML_CLOSE;
    if (data->cp_is_valid) {
        /* Save the last point in the current point */
        size_t n = type == CPML_MOVE ? 1 : cpml_primitive_type_get_n_points(type) - 1;
        cpml_pair_from_cairo(&data->cp, &path_data[n]);
    }

    /* Invalidate cairo_path: should be recomputed */
    _adg_clear_parent((AdgModel *) path);
}

static void
_adg_clear_operation(AdgPath *path)
{
    AdgPathPrivate *data;
    AdgOperation *operation;

    data = path->data;
    operation = &data->operation;

    if (operation->action != ADG_ACTION_NONE) {
        g_warning(_("%s: a '%s' operation is still active while clearing the path"),
                  G_STRLOC, _adg_action_name(operation->action));
        operation->action = ADG_ACTION_NONE;
    }

    data->cp_is_valid = FALSE;
    data->last.data = NULL;
    data->over.data = NULL;
}

static gboolean
_adg_append_operation(AdgPath *path, gint action, ...)
{
    AdgPathPrivate *data;
    AdgOperation *operation;
    AdgAction real_action;
    va_list var_args;

    real_action = (AdgAction) action;
    data = path->data;

    if (data->last.data == NULL) {
        g_warning(_("%s: requested a '%s' operation on a path without current primitive"),
                  G_STRLOC, _adg_action_name(real_action));
        return FALSE;
    }

    operation = &data->operation;
    if (operation->action != ADG_ACTION_NONE) {
        g_warning(_("%s: requested a '%s' operation while a '%s' operation was active"),
                  G_STRLOC, _adg_action_name(real_action),
                  _adg_action_name(operation->action));
        /* XXX: http://dev.entidi.com/p/adg/issues/50/ */
        return FALSE;
    }

    va_start(var_args, action);

    switch (real_action) {

    case ADG_ACTION_CHAMFER:
        operation->data.chamfer.delta1 = va_arg(var_args, double);
        operation->data.chamfer.delta2 = va_arg(var_args, double);
        break;

    case ADG_ACTION_FILLET:
        operation->data.fillet.radius = va_arg(var_args, double);
        break;

    case ADG_ACTION_NONE:
        va_end(var_args);
        return TRUE;

    default:
        g_warning(_("%s: %d path operation not recognized"), G_STRLOC, real_action);
        va_end(var_args);
        return FALSE;
    }

    operation->action = real_action;
    va_end(var_args);

    if (data->last.data[0].header.type == CPML_CLOSE) {
        /* Special case: an action with the close primitive should
         * be resolved now by changing the close primitive to a
         * line-to and using it as second operand and use the first
         * primitive of the current segment as first operand */
        guint length;
        cairo_path_data_t *path_data;
        CpmlSegment segment;
        CpmlPrimitive current;

        length = data->cairo.array->len;

        /* Ensure the close path primitive is not the only data */
        g_return_val_if_fail(length > 1, FALSE);

        /* Allocate one more item once for all to accept the
         * conversion from a close to line-to primitive */
        data->cairo.array = g_array_set_size(data->cairo.array, length + 1);
        path_data = (cairo_path_data_t *) data->cairo.array->data;
        --data->cairo.array->len;

        /* Set segment and current (the first primitive of segment) */
        cpml_segment_from_cairo(&segment, _adg_read_cairo_path(path));
        while (cpml_segment_next(&segment))
            ;
        cpml_primitive_from_segment(&current, &segment);

        /* Convert close path to a line-to primitive */
        ++data->cairo.array->len;
        path_data[length - 1].header.type = CPML_LINE;
        path_data[length - 1].header.length = 2;
        path_data[length] = *current.org;

        data->last.segment = &segment;
        data->last.org = &path_data[length - 2];
        data->last.data = &path_data[length - 1];

        _adg_do_action(path, real_action, &current);
    }

    return TRUE;
}

static void
_adg_do_operation(AdgPath *path, cairo_path_data_t *path_data)
{
    AdgPathPrivate *data;
    AdgAction action;
    CpmlSegment segment;
    CpmlPrimitive current;
    cairo_path_data_t current_org;

    data = path->data;
    action = data->operation.action;
    cpml_segment_from_cairo(&segment, _adg_read_cairo_path(path));

    /* Construct the current primitive, that is the primitive to be
     * mixed with the last primitive with the specified operation.
     * Its org is a copy of the end point of the last primitive: it can be
     * modified without affecting anything else. It is expected the operation
     * functions will add to @path the primitives required but NOT to add
     * @current, as this one will be inserted automatically. */
    current.segment = &segment;
    current.org = &current_org;
    current.data = path_data;
    cpml_pair_to_cairo(&data->cp, &current_org);

    _adg_do_action(path, action, &current);
}

static void
_adg_do_action(AdgPath *path, AdgAction action, CpmlPrimitive *primitive)
{
    switch (action) {
    case ADG_ACTION_NONE:
        return;
    case ADG_ACTION_CHAMFER:
        _adg_do_chamfer(path, primitive);
        break;
    case ADG_ACTION_FILLET:
        _adg_do_fillet(path, primitive);
        break;
    default:
        g_return_if_reached();
    }
}

static void
_adg_do_chamfer(AdgPath *path, CpmlPrimitive *current)
{
    AdgPathPrivate *data;
    CpmlPrimitive *last;
    gdouble delta1, delta2;
    gdouble len1, len2;
    CpmlPair pair;

    data = path->data;
    last = &data->last;
    delta1 = data->operation.data.chamfer.delta1;
    len1 = cpml_primitive_get_length(last);

    if (delta1 >= len1) {
        g_warning(_("%s: first chamfer delta of %lf is greather than the available %lf length"),
                  G_STRLOC, delta1, len1);
        return;
    }

    delta2 = data->operation.data.chamfer.delta2;
    len2 = cpml_primitive_get_length(current);

    if (delta2 >= len2) {
        g_warning(_("%s: second chamfer delta of %lf is greather than the available %lf length"),
                  G_STRLOC, delta1, len1);
        return;
    }

    /* Change the end point of the last primitive */
    cpml_primitive_put_pair_at(last, 1. - delta1 / len1, &pair);
    cpml_primitive_set_point(last, -1, &pair);

    /* Change the start point of the current primitive */
    cpml_primitive_put_pair_at(current, delta2 / len2, &pair);
    cpml_primitive_set_point(current, 0, &pair);

    /* Add the chamfer line */
    data->operation.action = ADG_ACTION_NONE;
    adg_path_append(path, CPML_LINE, &pair);
}

static void
_adg_do_fillet(AdgPath *path, CpmlPrimitive *current)
{
    AdgPathPrivate *data;
    CpmlPrimitive *last, *current_dup, *last_dup;
    gdouble radius, offset, pos;
    CpmlPair center, vector, p[3];

    data = path->data;
    last = &data->last;
    current_dup = cpml_primitive_deep_dup(current);
    last_dup = cpml_primitive_deep_dup(last);
    radius = data->operation.data.fillet.radius;
    offset = _adg_is_convex(last_dup, current_dup) ? -radius : radius;

    /* Find the center of the fillet from the intersection between
     * the last and current primitives offseted by radius */
    cpml_primitive_offset(current_dup, offset);
    cpml_primitive_offset(last_dup, offset);
    if (cpml_primitive_put_intersections(current_dup, last_dup, 1, &center) == 0) {
        g_warning(_("%s: fillet with radius of %lf is not applicable here"),
                  G_STRLOC, radius);
        g_free(current_dup);
        g_free(last_dup);
        return;
    }

    /* Compute the start point of the fillet */
    pos = cpml_primitive_get_closest_pos(last_dup, &center);
    cpml_primitive_put_vector_at(last_dup, pos, &vector);
    cpml_vector_set_length(&vector, offset);
    cpml_vector_normal(&vector);
    p[0].x = center.x - vector.x;
    p[0].y = center.y - vector.y;

    /* Compute the mid point of the fillet */
    cpml_pair_from_cairo(&vector, current->org);
    vector.x -= center.x;
    vector.y -= center.y;
    cpml_vector_set_length(&vector, radius);
    p[1].x = center.x + vector.x;
    p[1].y = center.y + vector.y;

    /* Compute the end point of the fillet */
    pos = cpml_primitive_get_closest_pos(current_dup, &center);
    cpml_primitive_put_vector_at(current_dup, pos, &vector);
    cpml_vector_set_length(&vector, offset);
    cpml_vector_normal(&vector);
    p[2].x = center.x - vector.x;
    p[2].y = center.y - vector.y;

    g_free(current_dup);
    g_free(last_dup);

    /* Change the end point of the last primitive */
    cpml_primitive_set_point(last, -1, &p[0]);

    /* Change the start point of the current primitive */
    cpml_primitive_set_point(current, 0, &p[2]);

    /* Add the fillet arc */
    data->operation.action = ADG_ACTION_NONE;
    adg_path_append(path, CPML_ARC, &p[1], &p[2]);
}

static gboolean
_adg_is_convex(const CpmlPrimitive *primitive1, const CpmlPrimitive *primitive2)
{
    CpmlVector v1, v2;
    gdouble angle1, angle2;

    cpml_primitive_put_vector_at(primitive1, -1, &v1);
    cpml_primitive_put_vector_at(primitive2, 0, &v2);

    /* Probably there is a smarter way to get this without trygonometry */
    angle1 = cpml_vector_angle(&v1);
    angle2 = cpml_vector_angle(&v2);

    if (angle1 > angle2)
        angle1 -= G_PI*2;

    return angle2-angle1 > G_PI;
}

static const gchar *
_adg_action_name(AdgAction action)
{
    switch (action) {
    case ADG_ACTION_NONE:
        return "NULL";
    case ADG_ACTION_CHAMFER:
        return "CHAMFER";
    case ADG_ACTION_FILLET:
        return "FILLET";
    }

    return "undefined";
}

static void
_adg_get_named_pair(AdgModel *model, const gchar *name,
                    CpmlPair *pair, gpointer user_data)
{
    GSList **named_pairs;
    AdgNamedPair *named_pair;

    named_pairs = user_data;

    named_pair = g_new(AdgNamedPair, 1);
    named_pair->name = name;
    named_pair->pair = *pair;

    *named_pairs = g_slist_prepend(*named_pairs, named_pair);
}

static void
_adg_dup_reverse_named_pairs(AdgModel *model, const cairo_matrix_t *matrix)
{
    AdgNamedPair *old_named_pair;
    AdgNamedPair named_pair;
    GSList *named_pairs;

    /* Populate named_pairs with all the named pairs of model */
    named_pairs = NULL;
    adg_model_foreach_named_pair(model, _adg_get_named_pair, &named_pairs);

    /* Readd the pairs applying the reversing transformation matrix to
     * their coordinates and prepending a "-" to their name */
    while (named_pairs) {
        old_named_pair = named_pairs->data;

        named_pair.name = g_strdup_printf("-%s", old_named_pair->name);
        named_pair.pair = old_named_pair->pair;
        cpml_pair_transform(&named_pair.pair, matrix);

        adg_model_set_named_pair(model, named_pair.name, &named_pair.pair);

        g_free((gpointer) named_pair.name);
        named_pairs = g_slist_delete_link(named_pairs, named_pairs);
    }
}
