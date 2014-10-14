/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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
 * <listitem>the <function>get_length</function> method must be
 *           implemented;</listitem>
 * <listitem>actually the <function>put_extents</function> method is
 *           implemented by computing the bounding box of the control
 *           polygon and this will likely include some empty space:
 *           there is room for improvements;</listitem>
 * <listitem>the <function>put_pair_at</function> method must be
 *           implemented;</listitem>
 * <listitem>the <function>put_vector_at</function> method must be
 *           implemented;</listitem>
 * <listitem>the <function>get_closest_pos</function> method must be
 *           implemented;</listitem>
 * <listitem>the <function>put_intersections</function> method must be
 *           implemented;</listitem>
 * </itemizedlist>
 * </important>
 *
 * Since: 1.0
 **/


/**
 * CpmlCurveOffsetAlgorithm:
 * @CPML_CURVE_OFFSET_ALGORITHM_NONE: unknown or no specific algorithm
 * @CPML_CURVE_OFFSET_ALGORITHM_DEFAULT: default algorithm
 * @CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT: handcraft algorithm
 * @CPML_CURVE_OFFSET_ALGORITHM_BAIOCA: B.A.I.O.C.A. algorithm
 *
 * Enumeration of all available algorithms for offsetting a cubic Bézier
 * curve.
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"
#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-primitive-private.h"
#include "cpml-curve.h"

/* TODO: will be offset_baioca() once it is implemented */
#define DEFAULT_ALGORITHM   offset_handcraft


static void     put_extents             (const CpmlPrimitive    *curve,
                                         CpmlExtents            *extents); static void     offset_handcraft        (CpmlPrimitive          *curve,
                                         double                  offset);
static void     offset_baioca           (CpmlPrimitive          *curve,
                                         double                  offset);

/* class_data is outside get_class so it can be modified by other methods */
static _CpmlPrimitiveClass class_data = {
    "curve to", 4,
    NULL,
    put_extents,
    NULL,
    NULL,
    NULL,
    NULL,
    DEFAULT_ALGORITHM,
    NULL
};


const _CpmlPrimitiveClass *
_cpml_curve_get_class(void)
{
    return &class_data;
}

/**
 * cpml_curve_offset_algorithm:
 * @new_algorithm: the new algorithm to use
 *
 * Selects the algorithm to use for offsetting Bézier curves and
 * returns the old algorithm.
 *
 * You can use #CPML_CURVE_OFFSET_ALGORITHM_NONE (that does not
 * change the current algorithm) if you are only interested in
 * knowing which is the current algorithm used.
 *
 * <important><para>
 * This function is <emphasis>not thread-safe</emphasis>. If you
 * are changing the algorithm in a thread environment you must
 * ensure by yourself no other threads are calling #CpmlCurve
 * methods in the meantime.
 * </para></important>
 *
 * Returns: the previous algorithm used.
 *
 * Since: 1.0
 **/
CpmlCurveOffsetAlgorithm
cpml_curve_offset_algorithm(CpmlCurveOffsetAlgorithm new_algorithm)
{
    CpmlCurveOffsetAlgorithm old_algorithm;

    /* Reverse lookup of the algorithm used */
    if (class_data.offset == offset_baioca) {
        old_algorithm = CPML_CURVE_OFFSET_ALGORITHM_BAIOCA;
    } else if (class_data.offset == offset_handcraft) {
        old_algorithm = CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT;
    } else {
        old_algorithm = CPML_CURVE_OFFSET_ALGORITHM_NONE;
    }

    switch (new_algorithm) {
    case CPML_CURVE_OFFSET_ALGORITHM_NONE:
        break;
    case CPML_CURVE_OFFSET_ALGORITHM_DEFAULT:
        class_data.offset = DEFAULT_ALGORITHM;
        break;
    case CPML_CURVE_OFFSET_ALGORITHM_BAIOCA:
        class_data.offset = offset_baioca;
        break;
    case CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT:
        class_data.offset = offset_handcraft;
        break;
    }

    return old_algorithm;
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
    CpmlPair p1, p2, p3, p4;
    double t_2, t_3, t1, t1_2, t1_3;

    cpml_primitive_put_point(curve, 0, &p1);
    cpml_primitive_put_point(curve, 1, &p2);
    cpml_primitive_put_point(curve, 2, &p3);
    cpml_primitive_put_point(curve, 3, &p4);

    t_2 = t * t;
    t_3 = t_2 * t;
    t1 = 1 - t;
    t1_2 = t1 * t1;
    t1_3 = t1_2 * t1;

    pair->x = t1_3 * p1.x + 3 * t1_2 * t * p2.x
              + 3 * t1 * t_2 * p3.x + t_3 * p4.x;
    pair->y = t1_3 * p1.y + 3 * t1_2 * t * p2.y
              + 3 * t1 * t_2 * p3.y + t_3 * p4.y;
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
    CpmlPair p1, p2, p3, p4;
    CpmlPair p21, p32, p43;
    double t1, t1_2, t_2;

    cpml_primitive_put_point(curve, 0, &p1);
    cpml_primitive_put_point(curve, 1, &p2);
    cpml_primitive_put_point(curve, 2, &p3);
    cpml_primitive_put_point(curve, 3, &p4);

    p21.x = p2.x - p1.x;
    p21.y = p2.y - p1.y;
    p32.x = p3.x - p2.x;
    p32.y = p3.y - p2.y;
    p43.x = p4.x - p3.x;
    p43.y = p4.y - p3.y;

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

    cpml_primitive_put_point(curve, 0, &p1);
    cpml_primitive_put_point(curve, 1, &p2);
    cpml_primitive_put_point(curve, 2, &p3);
    cpml_primitive_put_point(curve, 3, &p4);

    cpml_extents_pair_add(extents, &p1);
    cpml_extents_pair_add(extents, &p2);
    cpml_extents_pair_add(extents, &p3);
    cpml_extents_pair_add(extents, &p4);
}

static void
offset_handcraft(CpmlPrimitive *curve, double offset)
{
    double m, mm;
    CpmlVector v0, v3, vm, vtmp;
    CpmlPair p0, p1, p2, p3, r;

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

    /* r = point in C(m) offseted the requested @offset distance */
    cpml_curve_put_vector_at_time(curve, m, &vm);
    cpml_vector_set_length(&vm, offset);
    cpml_vector_normal(&vm);
    cpml_curve_put_pair_at_time(curve, m, &r);
    r.x += vm.x;
    r.y += vm.y;

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
        CpmlPair s;
        double k0, k3;

        s.x = (r.x - mm*mm*(1+m+m)*p0.x - m*m*(1+mm+mm)*p3.x) / (3*m*(1-m));
        s.y = (r.y - mm*mm*(1+m+m)*p0.y - m*m*(1+mm+mm)*p3.y) / (3*m*(1-m));

        if (v0.x != 0) {
            k3 = (s.y - s.x*v0.y / v0.x) / (m*(v3.y - v3.x*v0.y / v0.x));
            k0 = (s.x - m*k3*v3.x) / (mm*v0.x);
        } else {
            k0 = (s.y - s.x*v3.y / v3.x) / (mm*(v0.y - v0.x*v3.y / v3.x));
            k3 = (s.x - mm*k0*v0.x) / (m*v3.x);
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

static void
offset_baioca(CpmlPrimitive *curve, double offset)
{
    /* TODO */
}
