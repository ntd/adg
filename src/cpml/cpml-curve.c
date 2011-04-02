/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 * The following functions manipulate %CPML_CURVE #CpmlPrimitive.
 * No validation is made on the input so use the following methods
 * only when you are sure the <varname>primitive</varname> argument
 * is effectively a cubic Bézier curve.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>the get_length() method must be implemented;</listitem>
 * <listitem>actually the put_extents() method is implemented by computing
 *           the bounding box of the control polygon and this will likely
 *           include some empty space: there is room for improvements;</listitem>
 * <listitem>the put_pair_at() method must be implemented;</listitem>
 * <listitem>the put_vector_at() method must be implemented;</listitem>
 * <listitem>the get_closest_pos() method must be implemented;</listitem>
 * <listitem>the put_intersections() method must be implemented;</listitem>
 * <listitem>by default, the offset curve is calculated by using the point
 *           at t=0.5 as reference: use a better candidate;</listitem>
 * <listitem>in the offset() implementation, when the equations are
 *           inconsistent, the alternative approach performs very bad
 *           if <varname>v0</varname> and <varname>v3</varname> are
 *           opposite or staggered.</listitem>
 * </itemizedlist>
 * </important>
 * </para>
 * <refsect2 id="offset">
 * <title>Offseting algorithm</title>
 * <para>
 * Given a cubic Bézier primitive, it must be found the approximated
 * Bézier curve parallel to the original one at a specific distance
 * (the so called "offset curve"). The four points needed to build
 * the new curve must be returned.
 *
 * To solve the offset problem, a custom algorithm is used. First, the
 * resulting curve MUST have the same slope at the start and end point.
 * These constraints are not sufficient to resolve the system, so I let
 * the curve pass thought a given point (pm, known and got from the
 * original curve) at a given time (m, now hardcoded to 0.5).
 *
 * Firstly, some useful variables are defined:
 *
 * |[
 * v0 = unitvector(p[1] &minus; p[0]) &times; offset;
 * v3 = unitvector(p[3] &minus; p[2]) &times; offset;
 * p0 = p[0] + normal v0;
 * p3 = p[3] + normal v3.
 * ]|
 *
 * The resulting curve must have the same slopes than the original
 * one at the start and end points. Forcing the same slopes means:
 *
 * |[
 * p1 = p0 + k0 v0.
 * ]|
 *
 * where %k0 is an arbitrary factor. Decomposing for %x and %y:
 *
 * |[
 * p1.x = p0.x + k0 v0.x;
 * p1.y = p0.y + k0 v0.y.
 * ]|
 *
 * and doing the same for the end point:
 *
 * |[
 * p2.x = p3.x + k3 v3.x;
 * p2.y = p3.y + k3 v3.y.
 * ]|
 *
 * This does not give a resolvable system, though. The curve will be
 * interpolated by forcing its path to pass throught %pm when
 * <varname>time</varname> is %m, where <code>0 &le; m &le; 1</code>.
 * Knowing the function of the cubic Bézier:
 *
 * |[
 * C(t) = (1 &minus; t)³p0 + 3t(1 &minus; t)²p1 + 3t²(1 &minus; t)p2 + t³p3.
 * ]|
 *
 * and forcing <code>t = m</code> and <code>C(t) = pm</code>:
 *
 * |[
 * pm = (1 &minus; m)³p0 + 3m(1 &minus; m)²p1 + 3m²(1 &minus; m)p2 + m³p3.
 *
 * (1 &minus; m) p1 + m p2 = (pm &minus; (1 &minus; m)³p0 &minus; m³p3) / (3m (1 &minus; m)).
 * ]|
 *
 * gives this final system:
 *
 * |[
 * p1.x = p0.x + k0 v0.x;
 * p1.y = p0.y + k0 v0.y;
 * p2.x = p3.x + k3 v3.x;
 * p2.y = p3.y + k3 v3.y;
 * (1 &minus; m) p1.x + m p2.x = (pm.x &minus; (1 &minus; m)³p0.x &minus; m³p3.x) / (3m (1 &minus; m));
 * (1 &minus; m) p1.y + m p2.y = (pm.y &minus; (1 &minus; m)³p0.y &minus; m³p3.y) / (3m (1 &minus; m)).
 * ]|
 *
 * Substituting and resolving for %k0 and %k3:
 *
 * |[
 * (1 &minus; m) k0 v0.x + m k3 v3.x = (pm.x &minus; (1 &minus; m)³p0.x &minus; m³p3.x) / (3m (1 &minus; m)) &minus; (1 &minus; m) p0.x &minus; m p3.x;
 * (1 &minus; m) k0 v0.y + m k3 v3.y = (pm.y &minus; (1 &minus; m)³p0.y &minus; m³p3.y) / (3m (1 &minus; m)) &minus; (1 &minus; m) p0.y &minus; m p3.y.
 *
 * (1 &minus; m) k0 v0.x + m k3 v3.x = (pm.x &minus; (1 &minus; m)²(1+2m) p0.x &minus; m²(3 &minus; 2m) p3.x) / (3m (1 &minus; m));
 * (1 &minus; m) k0 v0.y + m k3 v3.y = (pm.y &minus; (1 &minus; m)²(1+2m) p0.y &minus; m²(3 &minus; 2m) p3.y) / (3m (1 &minus; m)).
 * ]|
 *
 * Letting:
 *
 * |[
 * pk = (pm &minus; (1 &minus; m)²(1+2m) p0 &minus; m²(3 &minus; 2m) p3) / (3m (1 &minus; m)).
 * ]|
 *
 * reduces the above to this final equations:
 *
 * |[
 * (1 &minus; m) k0 v0.x + m k3 v3.x = pk.x;
 * (1 &minus; m) k0 v0.y + m k3 v3.y = pk.y.
 * ]|
 *
 * If <code>v0.x &ne; 0</code>, the system can be resolved for %k0 and
 * %k3 calculated accordingly:
 *
 * |[
 * k0 = (pk.x &minus; m k3 v3.x) / ((1 &minus; m) v0.x);
 * (pk.x &minus; m k3 v3.x) v0.y / v0.x + m k3 v3.y = pk.y.
 *
 * k0 = (pk.x &minus; m k3 v3.x) / ((1 &minus; m) v0.x);
 * k3 m (v3.y &minus; v3.x v0.y / v0.x) = pk.y &minus; pk.x v0.y / v0.x.
 *
 * k3 = (pk.y &minus; pk.x v0.y / v0.x) / (m (v3.y &minus; v3.x v0.y / v0.x));
 * k0 = (pk.x &minus; m k3 v3.x) / ((1 &minus; m) v0.x).
 * ]|
 *
 * Otherwise, if <code>v3.x &ne; 0</code>, the system can be solved
 * for %k3 and %k0 calculated accordingly:
 *
 * |[
 * k3 = (pk.x &minus; (1 &minus; m) k0 v0.x) / (m v3.x);
 * (1 &minus; m) k0 v0.y + (pk.x &minus; (1 &minus; m) k0 v0.x) v3.y / v3.x = pk.y.
 *
 * k3 = (pk.x &minus; (1 &minus; m) k0 v0.x) / (m v3.x);
 * k0 (1 &minus; m) (v0.y &minus; k0 v0.x v3.y / v3.x) = pk.y &minus; pk.x v3.y / v3.x.
 *
 * k0 = (pk.y &minus; pk.x v3.y / v3.x) / ((1 &minus; m) (v0.y &minus; v0.x v3.y / v3.x));
 * k3 = (pk.x &minus; (1 &minus; m) k0 v0.x) / (m v3.x).
 * ]|
 *
 * The whole process must be guarded against division by 0 exceptions.
 * If either <code>v0.x</code> and <code>v3.x</code> are %0, the first
 * equation will be inconsistent. More in general, the
 * <code>v0.x &times; v3.y = v3.x &times; v3.y</code> condition must
 * be avoided. This is the first situation to avoid, in which case
 * an alternative approach should be used.
 *
 * </para>
 * </refsect2>
 * <para>
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"
#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-primitive-private.h"
#include "cpml-curve.h"


static void     put_extents             (const CpmlPrimitive    *curve,
                                         CpmlExtents            *extents);
static void     offset                  (CpmlPrimitive          *curve,
                                         double                  offset);


const _CpmlPrimitiveClass *
_cpml_curve_get_class(void)
{
    static _CpmlPrimitiveClass *p_class = NULL;

    if (p_class == NULL) {
        static _CpmlPrimitiveClass class_data = {
            "curve to", 4,
            NULL,
            put_extents,
            NULL,
            NULL,
            NULL,
            NULL,
            offset,
            NULL
        };
        p_class = &class_data;
    }

    return p_class;
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
 * Since: 1.0
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
 * Since: 1.0
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


static void
put_extents(const CpmlPrimitive *curve, CpmlExtents *extents)
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

static void
offset(CpmlPrimitive *curve, double offset)
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
    v0.x = p1.x - p0.x;
    v0.y = p1.y - p0.y;

    /* v3 = p3-p2 */
    v3.x = p3.x - p2.x;
    v3.y = p3.y - p2.y;

    /* pm = point in C(m) offseted the requested @offset distance */
    cpml_curve_put_vector_at_time(curve, m, &vm);
    cpml_vector_set_length(&vm, offset);
    cpml_vector_normal(&vm);
    cpml_curve_put_pair_at_time(curve, m, &pm);
    pm.x += vm.x;
    pm.y += vm.y;

    /* p0 = p0 + normal of v0 of @offset magnitude (exact value) */
    cpml_pair_copy(&vtmp, &v0);
    cpml_vector_set_length(&vtmp, offset);
    cpml_vector_normal(&vtmp);
    p0.x += vtmp.x;
    p0.y += vtmp.y;

    /* p3 = p3 + normal of v3 of @offset magnitude, as done for p0 */
    cpml_pair_copy(&vtmp, &v3);
    cpml_vector_set_length(&vtmp, offset);
    cpml_vector_normal(&vtmp);
    p3.x += vtmp.x;
    p3.y += vtmp.y;

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
