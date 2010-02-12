/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, 2009  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:cpml-curve
 * @Section_Id:CpmlCurve
 * @title: CpmlCurve
 * @short_description: Bézier cubic curve primitive management
 *
 * The following functions manipulate %CAIRO_PATH_CURVE_TO #CpmlPrimitive.
 * No validation is made on the input so use the following methods
 * only when you are sure the <varname>primitive</varname> argument
 * is effectively a cubic Bézier curve.
 **/


#include "cpml-internal.h"
#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-primitive-private.h"
#include "cpml-curve.h"


const _CpmlPrimitiveClass *
_cpml_curve_get_class(void)
{
    static _CpmlPrimitiveClass *p_class = NULL;

    if (p_class == NULL) {
        static _CpmlPrimitiveClass class_data = {
            "curve", 4,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        };
        p_class = &class_data;
    }

    return p_class;
}


/**
 * cpml_curve_get_length:
 * @curve: the #CpmlPrimitive curve data
 *
 * Given the @curve primitive, returns the approximated length of
 * the Bézier curve.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: the requested length
 **/
double
cpml_curve_get_length(const CpmlPrimitive *curve)
{
    return 0.;
}

/**
 * cpml_curve_put_extents:
 * @curve: the #CpmlPrimitive curve data
 * @extents: where to store the extents
 *
 * Given a @curve primitive, returns its boundary box in @extents.
 **/
void
cpml_curve_put_extents(const CpmlPrimitive *curve, CpmlExtents *extents)
{
    CpmlPair p1, p2, p3, p4;

    extents->is_defined = 0;

    cpml_pair_from_cairo(&p1, cpml_primitive_get_point(curve, 0));
    cpml_pair_from_cairo(&p2, cpml_primitive_get_point(curve, 1));
    cpml_pair_from_cairo(&p3, cpml_primitive_get_point(curve, 2));
    cpml_pair_from_cairo(&p4, cpml_primitive_get_point(curve, 3));

    cpml_extents_pair_add(extents, &p1);
    cpml_extents_pair_add(extents, &p2);
    cpml_extents_pair_add(extents, &p3);
    cpml_extents_pair_add(extents, &p4);
}

/**
 * cpml_curve_put_pair_at_time:
 * @curve: the #CpmlPrimitive curve data
 * @t:     the "time" value
 * @pair:  the destination pair
 *
 * Given the @curve Bézier cubic, finds the coordinates at time @t
 * (where 0 is the start and 1 is the end) and stores the result
 * in @pair. Keep in mind @t is not homogeneous, so 0.5 does not
 * necessarily means the mid point.
 *
 * The relation 0 < @t < 1 must be satisfied, as interpolating on
 * cubic curves is not allowed.
 **/
void
cpml_curve_put_pair_at_time(const CpmlPrimitive *curve, double t,
                            CpmlPair *pair)
{
    cairo_path_data_t *p1, *p2, *p3, *p4;
    double t_2, t_3, t1, t1_2, t1_3;

    p1 = cpml_primitive_get_point(curve, 0);
    p2 = cpml_primitive_get_point(curve, 1);
    p3 = cpml_primitive_get_point(curve, 2);
    p4 = cpml_primitive_get_point(curve, 3);

    t_2 = t * t;
    t_3 = t_2 * t;
    t1 = 1 - t;
    t1_2 = t1 * t1;
    t1_3 = t1_2 * t1;

    pair->x = t1_3 * p1->point.x + 3 * t1_2 * t * p2->point.x
              + 3 * t1 * t_2 * p3->point.x + t_3 * p4->point.x;
    pair->y = t1_3 * p1->point.y + 3 * t1_2 * t * p2->point.y
              + 3 * t1 * t_2 * p3->point.y + t_3 * p4->point.y;
}

/**
 * cpml_curve_put_pair_at:
 * @curve: the #CpmlPrimitive curve data
 * @pos:   the position value
 * @pair:  the destination #CpmlPair
 *
 * Given the @curve Bézier cubic, finds the coordinates at position
 * @pos (where 0 is the start and 1 is the end) and stores the result
 * in @pair. It is similar to cpml_curve_put_pair_at_time() but the @pos
 * value is evenly distribuited, that is 0.5 is exactly the mid point.
 * If you do not need this feature, use cpml_curve_put_pair_at_time()
 * as it is considerable faster.
 *
 * The relation 0 < @pos < 1 must be satisfied, as interpolating on
 * cubic curves is not allowed.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 **/
void
cpml_curve_put_pair_at(const CpmlPrimitive *curve, double pos, CpmlPair *pair)
{
}

/**
 * cpml_curve_put_vector_at_time:
 * @curve:  the #CpmlPrimitive curve data
 * @t:      the "time" value
 * @vector: the destination vector
 *
 * Given the @curve Bézier cubic, finds the slope at time @t
 * (where 0 is the start and 1 is the end) and stores the result
 * in @vector. Keep in mind @t is not homogeneous, so 0.5
 * does not necessarily means the mid point.
 *
 * @t must be inside the range 0 .. 1, as interpolating is not
 * allowed.
 **/
void
cpml_curve_put_vector_at_time(const CpmlPrimitive *curve,
                              double t, CpmlVector *vector)
{
    cairo_path_data_t *p1, *p2, *p3, *p4;
    CpmlPair p21, p32, p43;
    double t1, t1_2, t_2;

    p1 = cpml_primitive_get_point(curve, 0);
    p2 = cpml_primitive_get_point(curve, 1);
    p3 = cpml_primitive_get_point(curve, 2);
    p4 = cpml_primitive_get_point(curve, 3);

    p21.x = p2->point.x - p1->point.x;
    p21.y = p2->point.y - p1->point.y;
    p32.x = p3->point.x - p2->point.x;
    p32.y = p3->point.y - p2->point.y;
    p43.x = p4->point.x - p3->point.x;
    p43.y = p4->point.y - p3->point.y;

    t1 = 1 - t;
    t1_2 = t1 * t1;
    t_2 = t * t;

    vector->x = 3 * t1_2 * p21.x + 6 * t1 * t * p32.x + 3 * t_2 * p43.x;
    vector->y = 3 * t1_2 * p21.y + 6 * t1 * t * p32.y + 3 * t_2 * p43.y;
}

/**
 * cpml_curve_put_vector_at:
 * @curve:  the #CpmlPrimitive curve data
 * @pos:    the position value
 * @vector: the destination vector
 *
 * Given the @curve Bézier cubic, finds the slope at position @pos
 * (where 0 is the start and 1 is the end) and stores the result
 * in @vector. It is similar to cpml_curve_put_vector_at_time()
 * but the @pos value is evenly distribuited, that is 0.5 is
 * exactly the mid point. If you do not need this feature, use
 * cpml_curve_put_vector_at_time() as it is considerable faster.
 *
 * @pos must be inside the range 0 .. 1, as interpolating is not
 * allowed.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 **/
void
cpml_curve_put_vector_at(const CpmlPrimitive *curve, double pos,
                         CpmlVector *vector)
{
}

/**
 * cpml_curve_get_closest_pos:
 * @curve: the #CpmlPrimitive curve data
 * @pair:  the coordinates of the subject point
 *
 * Returns the pos value of the point on @curve nearest to @pair.
 * The returned value is always between 0 and 1.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: the pos value, always between 0 and 1
 **/
double
cpml_curve_get_closest_pos(const CpmlPrimitive *curve, const CpmlPair *pair)
{
    /* TODO */

    return 0;
}

/**
 * cpml_curve_put_intersections:
 * @curve:  the first curve
 * @curve2: the second curve
 * @max:    maximum number of intersections to return
 *          (that is, the size of @dest)
 * @dest:   a vector of #CpmlPair
 *
 * Given two Bézier cubic curves (@curve and @curve2), gets their
 * intersection points and store the result in @dest. Because two
 * curves can have 4 intersections, @dest MUST be at least an array
 * of 4 #CpmlPair.
 *
 * If @max is 0, the function returns 0 immediately without any
 * further processing. If @curve and @curve2 are cohincident,
 * their intersections are not considered.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: the number of intersections found (max 4)
 *               or 0 if the primitives do not intersect
 **/
int
cpml_curve_put_intersections(const CpmlPrimitive *curve,
                             const CpmlPrimitive *curve2,
                             int max, CpmlPair *dest)
{
    return 0;
}

/**
 * cpml_curve_put_intersections_with_arc:
 * @curve: a curve
 * @arc:   an arc
 * @max:   maximum number of intersections to return
 *         (that is, the size of @dest)
 * @dest:  a vector of #CpmlPair
 *
 * Given a Bézier cubic @curve and an @arc, gets their intersection
 * points and store the result in @dest. Because an arc and a cubic
 * curve can have up to 4 intersections, @dest MUST be at least an
 * array of 4 #CpmlPair.
 *
 * If @max is 0, the function returns 0 immediately without any
 * further processing.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: the number of intersections found (max 4)
 *          or 0 if the primitives do not intersect
 **/
int
cpml_curve_put_intersections_with_arc(const CpmlPrimitive *curve,
                                      const CpmlPrimitive *arc,
                                      int max, CpmlPair *dest)
{
    return 0;
}

/**
 * cpml_curve_put_intersections_with_line:
 * @curve: a curve
 * @line:  a line
 * @max:   maximum number of intersections to return
 *         (that is, the size of @dest)
 * @dest:  a vector of #CpmlPair
 *
 * Given a Bézier cubic @curve and a @line, gets their intersection
 * points and store the result in @dest. Because a line and a cubic
 * curve can have up to 4 intersections, @dest MUST be at least an
 * array of 4 #CpmlPair.
 *
 * If @max is 0, the function returns 0 immediately without any
 * further processing.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Returns: the number of intersections found (max 4)
 *          or 0 if the primitives do not intersect
 **/
int
cpml_curve_put_intersections_with_line(const CpmlPrimitive *curve,
                                       const CpmlPrimitive *line,
                                       int max, CpmlPair *dest)
{
    return 0;
}

/**
 * cpml_curve_offset:
 * @curve:  the #CpmlPrimitive curve data
 * @offset: distance for the computed parallel curve
 *
 * Given a cubic Bézier primitive in @curve, this function finds
 * the approximated Bézier curve parallel to @curve at distance
 * @offset (an offset curve). The four points needed to build the
 * new curve are returned in the @curve struct.
 *
 * To solve the offset problem, a custom algorithm is used. First, the
 * resulting curve MUST have the same slope at the start and end point.
 * These constraints are not sufficient to resolve the system, so I let
 * the curve pass thought a given point (pm, known and got from the
 * original curve) at a given time (m, now hardcoded to 0.5).
 *
 * Firstly, I define some useful variables:
 *
 * v0 = unitvector(p[1]-p[0]) * offset;
 * v3 = unitvector(p[3]-p[2]) * offset;
 * p0 = p[0] + normal v0;
 * p3 = p[3] + normal v3.
 *
 * Now I want the curve to have the specified slopes at the start
 * and end point. Forcing the same slope at the start point means:
 *
 * p1 = p0 + k0 v0.
 *
 * where k0 is an arbitrary factor. Decomposing for x and y components:
 *
 * p1.x = p0.x + k0 v0.x;
 * p1.y = p0.y + k0 v0.y.
 *
 * Doing the same for the end point gives:
 *
 * p2.x = p3.x + k3 v3.x;
 * p2.y = p3.y + k3 v3.y.
 *
 * Now I interpolate the curve by forcing it to pass throught pm
 * when "time" is m, where 0 < m < 1. The cubic Bézier function is:
 *
 * C(t) = (1-t)³p0 + 3t(1-t)²p1 + 3t²(1-t)p2 + t³p3.
 *
 * and forcing t=m and C(t) = pm:
 *
 * pm = (1-m)³p0 + 3m(1-m)²p1 + 3m²(1-m)p2 + m³p3.
 *
 * (1-m) p1 + m p2 = (pm - (1-m)³p0 - m³p3) / (3m (1-m)).
 *
 * So the final system is:
 *
 * p1.x = p0.x + k0 v0.x;
 * p1.y = p0.y + k0 v0.y;
 * p2.x = p3.x + k3 v3.x;
 * p2.y = p3.y + k3 v3.y;
 * (1-m) p1.x + m p2.x = (pm.x - (1-m)³p0.x - m³p3.x) / (3m (1-m));
 * (1-m) p1.y + m p2.y = (pm.y - (1-m)³p0.y - m³p3.y) / (3m (1-m)).
 *
 * Substituting and resolving for k0 and k3:
 *
 * (1-m) k0 v0.x + m k3 v3.x =
 *     (pm.x - (1-m)³p0.x - m³p3.x) / (3m (1-m)) - (1-m) p0.x - m p3.x;
 * (1-m) k0 v0.y + m k3 v3.y =
 *     (pm.y - (1-m)³p0.y - m³p3.y) / (3m (1-m)) - (1-m) p0.y - m p3.y.
 *
 * (1-m) k0 v0.x + m k3 v3.x =
 *     (pm.x - (1-m)²(1+2m) p0.x - m²(3-2m) p3.x) / (3m (1-m));
 * (1-m) k0 v0.y + m k3 v3.y =
 *     (pm.y - (1-m)²(1+2m) p0.y - m²(3-2m) p3.y) / (3m (1-m)).
 *
 * Let:
 *
 * pk = (pm - (1-m)²(1+2m) p0 - m²(3-2m) p3) / (3m (1-m)).
 *
 * gives the following system:
 *
 * (1-m) k0 v0.x + m k3 v3.x = pk.x;
 * (1-m) k0 v0.y + m k3 v3.y = pk.y.
 *
 * Now I should avoid division by 0 troubles. If either v0.x and v3.x
 * are 0, the first equation will be inconsistent. More in general the
 * v0.x*v3.y = v3.x*v3.y condition should be avoided. This is the first
 * case to check, in which case an alternative approach is used. In the
 * other cases the above system can be used.
 *
 * If v0.x != 0 I can resolve for k0 and then find k3:
 *
 * k0 = (pk.x - m k3 v3.x) / ((1-m) v0.x);
 * (pk.x - m k3 v3.x) v0.y / v0.x + m k3 v3.y = pk.y.
 *
 * k0 = (pk.x - m k3 v3.x) / ((1-m) v0.x);
 * k3 m (v3.y - v3.x v0.y / v0.x) = pk.y - pk.x v0.y / v0.x.
 *
 * k3 = (pk.y - pk.x v0.y / v0.x) / (m (v3.y - v3.x v0.y / v0.x));
 * k0 = (pk.x - m k3 v3.x) / ((1-m) v0.x).
 *
 * If v3.x != 0 I can resolve for k3 and then find k0:
 *
 * k3 = (pk.x - (1-m) k0 v0.x) / (m v3.x);
 * (1-m) k0 v0.y + (pk.x - (1-m) k0 v0.x) v3.y / v3.x = pk.y.
 *
 * k3 = (pk.x - (1-m) k0 v0.x) / (m v3.x);
 * k0 (1-m) (v0.y - k0 v0.x v3.y / v3.x) = pk.y - pk.x v3.y / v3.x.
 *
 * k0 = (pk.y - pk.x v3.y / v3.x) / ((1-m) (v0.y - v0.x v3.y / v3.x));
 * k3 = (pk.x - (1-m) k0 v0.x) / (m v3.x).
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>By default, interpolation of the new curve is made by offseting
 *           the mid point: use a better candidate.</listitem>
 * <listitem>When the equations are inconsistent, the alternative approach
 *           performs very bad if <varname>v0</varname> and
 *           <varname>v3</varname> are opposite or staggered.</listitem>
 * </itemizedlist>
 * </important>
 **/
void
cpml_curve_offset(CpmlPrimitive *curve, double offset)
{
    double m, mm;
    CpmlVector v0, v3, vm, vtmp;
    CpmlPair p0, p1, p2, p3, pm;

    m = 0.5;
    mm = 1-m;

    /* Firstly, convert the curve points from cairo format to cpml format
     * and store them (temporary) in p0..p3 */
    cpml_pair_from_cairo(&p0, curve->org);
    cpml_pair_from_cairo(&p1, &curve->data[1]);
    cpml_pair_from_cairo(&p2, &curve->data[2]);
    cpml_pair_from_cairo(&p3, &curve->data[3]);

    /* v0 = p1-p0 */
    cpml_pair_copy(&v0, &p1);
    cpml_pair_sub(&v0, &p0);

    /* v3 = p3-p2 */
    cpml_pair_copy(&v3, &p3);
    cpml_pair_sub(&v3, &p2);

    /* pm = point in C(m) offseted the requested @offset distance */
    cpml_curve_put_vector_at_time(curve, m, &vm);
    cpml_vector_set_length(&vm, offset);
    cpml_vector_normal(&vm);
    cpml_curve_put_pair_at_time(curve, m, &pm);
    cpml_pair_add(&pm, &vm);

    /* p0 = p0 + normal of v0 of @offset magnitude (exact value) */
    cpml_pair_copy(&vtmp, &v0);
    cpml_vector_set_length(&vtmp, offset);
    cpml_vector_normal(&vtmp);
    cpml_pair_add(&p0, &vtmp);

    /* p3 = p3 + normal of v3 of @offset magnitude, as done for p0 */
    cpml_pair_copy(&vtmp, &v3);
    cpml_vector_set_length(&vtmp, offset);
    cpml_vector_normal(&vtmp);
    cpml_pair_add(&p3, &vtmp);

    if (v0.x*v3.y == v3.x*v0.y) {
        /* Inconsistent equations: use the alternative approach */
        p1.x = p0.x + v0.x + vm.x * 4/3;
        p1.y = p0.y + v0.y + vm.y * 4/3;
        p2.x = p3.x - v3.x + vm.x * 4/3;
        p2.y = p3.y - v3.y + vm.y * 4/3;
    } else {
        CpmlPair pk;
        double k0, k3;

        pk.x = (pm.x - mm*mm*(1+m+m)*p0.x - m*m*(1+mm+mm)*p3.x) / (3*m*(1-m));
        pk.y = (pm.y - mm*mm*(1+m+m)*p0.y - m*m*(1+mm+mm)*p3.y) / (3*m*(1-m));

        if (v0.x != 0) {
            k3 = (pk.y - pk.x*v0.y / v0.x) / (m*(v3.y - v3.x*v0.y / v0.x));
            k0 = (pk.x - m*k3*v3.x) / (mm*v0.x);
        } else {
            k0 = (pk.y - pk.x*v3.y / v3.x) / (mm*(v0.y - v0.x*v3.y / v3.x));
            k3 = (pk.x - mm*k0*v0.x) / (m*v3.x);
        }

        p1.x = p0.x + k0*v0.x;
        p1.y = p0.y + k0*v0.y;
        p2.x = p3.x + k3*v3.x;
        p2.y = p3.y + k3*v3.y;
    }

    /* Return the new curve in the original array */
    cpml_pair_to_cairo(&p0, curve->org);
    cpml_pair_to_cairo(&p1, &curve->data[1]);
    cpml_pair_to_cairo(&p2, &curve->data[2]);
    cpml_pair_to_cairo(&p3, &curve->data[3]);
}
