/* CPML - Cairo Path Manipulation Library
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
 * @CPML_CURVE_OFFSET_ALGORITHM_GEOMETRICAL: geometrical algorithm
 * @CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT: handcraft algorithm
 * @CPML_CURVE_OFFSET_ALGORITHM_BAIOCA: B.A.I.O.C.A. algorithm
 *
 * Enumeration of all available algorithms for offsetting the B(t) cubic
 * Bézier curve.
 *
 * The geometrical algorithm offset the control polygon, further applying a
 * factor of 4/3 on the control points along the vector normal to B(0.5). That
 * factor mix well with curves approximating arcs (the most common case). It
 * is a stable algorithm, i.e. it can be used for every Bézier.
 *
 * The handcraft algorithm offsets the point at B(0.5) and it forces the
 * condition the offset curve at t=0.5 must pass through that point. If it
 * fails, falls back to geometrical.
 *
 * The BAIOCA algorithm offsets a specific set of t values and try to minimize
 * the error between those points and the offset curve at the same t values.
 * If it fails, falls back to geometrical. By default the set of t values is
 * { 0, 0.25, 0.5, 0.75, 1 }. As implied by this description, using the set
 * { 0, 0.5, 1 } is logically equivalent to the handcraft algorithm.
 *
 * The default algorith is #CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT.
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"
#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-primitive-private.h"
#include "cpml-curve.h"

#define DEFAULT_ALGORITHM   offset_handcraft


static void     put_extents             (const CpmlPrimitive    *curve,
                                         CpmlExtents            *extents);
static void     offset_geometrical      (CpmlPrimitive          *curve,
                                         double                  offset);
static void     offset_handcraft        (CpmlPrimitive          *curve,
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
    if (class_data.offset == offset_handcraft) {
        old_algorithm = CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT;
    } else if (class_data.offset == offset_baioca) {
        old_algorithm = CPML_CURVE_OFFSET_ALGORITHM_BAIOCA;
    } else if (class_data.offset == offset_geometrical) {
        old_algorithm = CPML_CURVE_OFFSET_ALGORITHM_GEOMETRICAL;
    } else {
        old_algorithm = CPML_CURVE_OFFSET_ALGORITHM_NONE;
    }

    switch (new_algorithm) {
    case CPML_CURVE_OFFSET_ALGORITHM_NONE:
        break;
    case CPML_CURVE_OFFSET_ALGORITHM_DEFAULT:
        class_data.offset = DEFAULT_ALGORITHM;
        break;
    case CPML_CURVE_OFFSET_ALGORITHM_GEOMETRICAL:
        class_data.offset = offset_geometrical;
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

/**
 * cpml_curve_put_offset_at_time:
 * @curve:                        the #CpmlPrimitive curve data
 * @t:                            the "time" value
 * @offset:                       the offset distance
 * @pair: (out caller-allocates): the destination pair
 *
 * Given the @curve Bézier cubic, find the coordinates at time @t
 * (where 0 is the start and 1 is the end), offset that point at
 * @offset distance and stores the result in @pair.
 *
 * The point to offset and the vector along which that point must
 * be offseted are found calling cpml_curve_put_pair_at_time() and
 * cpml_curve_put_vector_at_time() respectively.
 *
 * Since: 1.0
 **/
void
cpml_curve_put_offset_at_time(const CpmlPrimitive *curve,
                              double t, double offset,
                              CpmlPair *pair)
{
    CpmlVector vector;

    cpml_curve_put_vector_at_time(curve, t, &vector);
    cpml_vector_normal(&vector);
    cpml_vector_set_length(&vector, offset);

    cpml_curve_put_pair_at_time(curve, t, pair);
    pair->x += vector.x;
    pair->y += vector.y;
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

static int
geometrical(CpmlPrimitive *curve, double offset, const CpmlVector *v)
{
    CpmlVector v0, v3;
    CpmlPair p0, p1, p2, p3;

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

    cpml_curve_put_offset_at_time(curve, 0, offset, &p0);
    cpml_curve_put_offset_at_time(curve, 1, offset, &p3);

    p1.x = p0.x + v0.x + v->x;
    p1.y = p0.y + v0.y + v->y;
    p2.x = p3.x - v3.x + v->x;
    p2.y = p3.y - v3.y + v->y;

    /* Return the new curve in the original array */
    cpml_pair_to_cairo(&p0, curve->org);
    cpml_pair_to_cairo(&p1, &curve->data[1]);
    cpml_pair_to_cairo(&p2, &curve->data[2]);
    cpml_pair_to_cairo(&p3, &curve->data[3]);

    return 1;
}

static void
offset_geometrical(CpmlPrimitive *curve, double offset)
{
    CpmlVector v;

    cpml_curve_put_vector_at_time(curve, 0.5, &v);
    cpml_vector_normal(&v);
    cpml_vector_set_length(&v, offset * 4/3);

    geometrical(curve, offset, &v);
}

static int
handcraft(CpmlPrimitive *curve, double offset, double m)
{
    CpmlVector v0, v3;
    CpmlPair p0, p1, p2, p3, r, s;
    double mm, k0, k3;

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

    if (v0.x*v3.y == v3.x*v0.y)
        return 0;

    cpml_curve_put_offset_at_time(curve, m, offset, &r);
    cpml_curve_put_offset_at_time(curve, 0, offset, &p0);
    cpml_curve_put_offset_at_time(curve, 1, offset, &p3);

    mm = 1 - m;
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

    /* Return the new curve in the original array */
    cpml_pair_to_cairo(&p0, curve->org);
    cpml_pair_to_cairo(&p1, &curve->data[1]);
    cpml_pair_to_cairo(&p2, &curve->data[2]);
    cpml_pair_to_cairo(&p3, &curve->data[3]);

    return 1;
}

static void
offset_handcraft(CpmlPrimitive *curve, double offset)
{
    if (! handcraft(curve, offset, 0.5))
        offset_geometrical(curve, offset);
}

/* Helper macro that returns the scalar product of two vectors */
#define SP(a,b) ((a).x * (b).x + (a).y * (b).y)

static int
baioca(CpmlPrimitive *curve, double offset, const double t[], int n)
{
    int i;
    double _t, _t2, _1t, _1t2;
    CpmlPair B0, B1, B2, B3;
    CpmlPair Q0, Q1, Q2, Q3;
    CpmlPair P0, P2, Ci;
    double b0, b1, b2, b3;
    double D0, D2, E01, E02, E11, E12, E13, E22, E23;
    double A1, A2, A3, A4, A5;
    double r, s, divisor;

    /* Pick the CpmlPair from the original primitive */
    cpml_pair_from_cairo(&B0, curve->org);
    cpml_pair_from_cairo(&B1, &curve->data[1]);
    cpml_pair_from_cairo(&B2, &curve->data[2]);
    cpml_pair_from_cairo(&B3, &curve->data[3]);

    /* 3. Calculate P0 and P2 */
    P0.x = B1.x - B0.x;
    P0.y = B1.y - B0.y;
    P2.x = B3.x - B2.x;
    P2.y = B3.y - B2.y;

    D0 = D2 = E01 = E02 = E11 = E12 = E13 = E22 = E23 = 0;
    for (i = 0; i <= n; ++i) {
        _t = t[i];

        /* 2. Compute Ci */
        cpml_curve_put_offset_at_time(curve, _t, offset, &Ci);
        if (i == 0) {
            cpml_pair_copy(&Q0, &Ci);
        } else if (i == n) {
            cpml_pair_copy(&Q3, &Ci);
        } else {
            _t2 = _t * _t;
            _1t = 1 - _t;
            _1t2 = _1t * _1t;
            b0 = _1t * _1t2;
            b1 = 3 * _1t2 * _t;
            b2 = 3 * _1t * _t2;
            b3 = _t * _t2;

            /* 4. Calculate D0, D2, E01, E02, E11, E12, E13, E22, E23 */
            D0  += b1 * SP(Ci, P0);
            D2  += b2 * SP(Ci, P2);
            E01 += b0 * b1;
            E02 += b0 * b2;
            E11 += b1 * b1;
            E12 += b1 * b2;
            E13 += b1 * b3;
            E22 += b2 * b2;
            E23 += b2 * b3;
        }
    }

    /* 5. Calculate A1, A2, A3, A4 and A5 */
    A1 = D0 - SP(Q0, P0) * (E01 + E11) - SP(Q3, P0) * (E12 + E13);
    A2 = D2 - SP(Q0, P2) * (E02 + E12) - SP(Q3, P2) * (E22 + E23);
    A3 = SP(P0, P0) * E11;
    A4 = SP(P0, P2) * E12;
    A5 = SP(P2, P2) * E22;

    /* 6. Calculate r and s */
    divisor = A3 * A5 - A4 * A4;
    if (divisor == 0)
        return 0;

    r = (A1 * A5 - A4 * A2) / divisor;
    s = (A3 * A2 - A1 * A4) / divisor;

    /* 7. Get Q1 and Q2 */
    Q1.x = Q0.x + r * P0.x;
    Q1.y = Q0.y + r * P0.y;
    Q2.x = Q3.x + s * P2.x;
    Q2.y = Q3.y + s * P2.y;

    /* Store the results into the original primitive */
    cpml_pair_to_cairo(&Q0, curve->org);
    cpml_pair_to_cairo(&Q1, &curve->data[1]);
    cpml_pair_to_cairo(&Q2, &curve->data[2]);
    cpml_pair_to_cairo(&Q3, &curve->data[3]);

    return 1;
}

static void
offset_baioca(CpmlPrimitive *curve, double offset)
{
    int i, n = 4;
    double t[n+1];

    /* 1. Select t_i using the lazy method */
    for (i = 0; i <= n; ++i)
        t[i] = (double) i / n;

    if (! baioca(curve, offset, t, n))
        offset_geometrical(curve, offset);
}
