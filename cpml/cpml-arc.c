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
 * SECTION:arc
 * @Section_Id:CpmlArc
 * @title: CpmlArc
 * @short_description: Manipulation of circular arcs
 *
 * The following functions manipulate #CAIRO_PATH_ARC_TO #CpmlPrimitive.
 * No validation is made on the input so use the following methods
 * only when you are sure the <varname>primitive</varname> argument
 * is effectively an arc-to.
 *
 * The arc primitive is defined by 3 points: the first one is the usual
 * implicit point got from the previous primitive, the second point is
 * an arbitrary intermediate point laying on the arc and the third point
 * is the end of the arc. These points identify univocally an arc:
 * furthermore, the intermediate point also gives the side of
 * the arc.
 *
 * As a special case, when the first point is coincident with the end
 * point the primitive is considered a circle with diameter defined by
 * the segment between the first and the intermediate point.
 *
 * <important>
 * <para>
 * An arc is not a native cairo primitive and should be treated specially.
 * </para>
 * </important>
 *
 * Using these CPML APIs you are free to use #CAIRO_PATH_ARC_TO whenever
 * you want but, if you are directly accessing the struct fields, you
 * are responsible of converting arcs to curves before passing them
 * to cairo. In other words, do not directly feed #CpmlPath struct to
 * cairo (throught cairo_append_path() for example) or at least do not
 * expect it will work.
 *
 * The conversion is provided by two APIs: cpml_arc_to_cairo() and
 * cpml_arc_to_curves(). The former directly renders to a cairo context
 * and is internally used by all the ..._to_cairo() functions when an
 * arc is met. The latter provided a more powerful (and more complex)
 * approach as it allows to specify the number of curves to use and do
 * not need a cairo context.
 **/


#include "cpml-arc.h"
#include "cpml-pair.h"

#include <stdlib.h>
#include <math.h>


/* Hardcoded max angle of the arc to be approximated by a Bézier curve:
 * this influence the arc quality (the default value is got from cairo) */
#define ARC_MAX_ANGLE   M_PI_2


static cairo_bool_t     get_center      (const CpmlPair *p,
                                         CpmlPair       *dest);
static void             get_angles      (const CpmlPair *p,
                                         const CpmlPair *center,
                                         double         *start,
                                         double         *end);
static void             arc_to_curve    (CpmlPrimitive  *curve,
                                         const CpmlPair *center,
                                         double          r,
                                         double          start,
                                         double          end);


/**
 * cpml_arc_type_get_npoints:
 *
 * Returns the number of point needed to properly specify an arc primitive.
 *
 * Return value: 3
 **/
int
cpml_arc_type_get_npoints(void)
{
    return 3;
}

/**
 * cpml_arc_info:
 * @arc:    the #CpmlPrimitive arc data
 * @center: where to store the center coordinates (can be %NULL)
 * @r:      where to store the radius (can be %NULL)
 * @start:  where to store the starting angle (can be %NULL)
 * @end:    where to store the ending angle (can be %NULL)
 *
 * Given an @arc, this function calculates and returns its basic data.
 * Any pointer can be %NULL, in which case the requested info is not
 * returned. This function can fail (when the three points lay on a
 * straight line, for example) in which case 0 is returned and no
 * data can be considered valid.
 *
 * The radius @r can be 0 when the three points are coincidents: a
 * circle with radius 0 is considered a valid path.
 *
 * When the start and end angle are returned, together with their
 * values these angles implicitely gives another important information:
 * the arc direction.
 *
 * If @start < @end the arc must be rendered with increasing angle
 * value (clockwise direction using the ordinary cairo coordinate
 * system) while if @start > @end the arc must be rendered in reverse
 * order (that is counterclockwise in the cairo world). This is the
 * reason the angle values are returned in the range
 * { -M_PI < value < 3*M_PI } inclusive instead of the usual
 * { -M_PI < value < M_PI } range.
 *
 * Return value: 1 if the function worked succesfully, 0 on errors
 **/
cairo_bool_t
cpml_arc_info(const CpmlPrimitive *arc, CpmlPair *center,
              double *r, double *start, double *end)
{
    CpmlPair p[3], l_center;

    cpml_pair_from_cairo(&p[0], arc->org);
    cpml_pair_from_cairo(&p[1], &arc->data[1]);
    cpml_pair_from_cairo(&p[2], &arc->data[2]);

    if (!get_center(p, &l_center))
        return 0;

    if (center)
        *center = l_center;

    if (r != NULL)
        *r = cpml_pair_distance(&p[0], &l_center);

    if (start != NULL || end != NULL) {
        double l_start, l_end;

        get_angles(p, &l_center, &l_start, &l_end);

        if (start != NULL)
            *start = l_start;
        if (end != NULL)
            *end = l_end;
    }

    return 1;
}

/**
 * cpml_arc_length:
 * @arc:  the #CpmlPrimitive arc data
 *
 * Given the @arc primitive, returns its length.
 *
 * Return value: the requested length or 0 on errors
 **/
double
cpml_arc_length(const CpmlPrimitive *arc)
{
    double r, start, end, delta;

    if (!cpml_arc_info(arc, NULL, &r, &start, &end) || start == end)
        return 0.;

    delta = end - start;
    if (delta < 0)
        delta += M_PI*2;

    return r*delta;
}

/**
 * cpml_arc_pair_at:
 * @arc:  the #CpmlPrimitive arc data
 * @pair: the destination #CpmlPair
 * @pos:  the position value
 *
 * Given an @arc, finds the coordinates at position @pos (where 0 is
 * the start and 1 is the end) and stores the result in @pair.
 *
 * @pos can also be outside the 0..1 limit, as interpolating on an
 * arc is quite trivial.
 **/
void
cpml_arc_pair_at(const CpmlPrimitive *arc, CpmlPair *pair, double pos)
{
    if (pos == 0.) {
        cpml_pair_from_cairo(pair, arc->org);
    } else if (pos == 1.) {
        cpml_pair_from_cairo(pair, &arc->data[2]);
    } else {
        CpmlPair center;
        double r, start, end, angle;

        if (!cpml_arc_info(arc, &center, &r, &start, &end))
            return;

        angle = (end-start)*pos + start;
        cpml_vector_from_angle(pair, angle, r);
        cpml_pair_add(pair, &center);
    }
}

/**
 * cpml_arc_vector_at:
 * @arc:    the #CpmlPrimitive arc data
 * @vector: the destination vector
 * @pos:    the position value
 *
 * Given an @arc, finds the slope at position @pos (where 0 is
 * the start and 1 is the end) and stores the result in @vector.
 *
 * @pos can also be outside the 0..1 limit, as interpolating on an
 * arc is quite trivial.
 **/
void
cpml_arc_vector_at(const CpmlPrimitive *arc, CpmlVector *vector, double pos)
{
    double start, end, angle;

    if (!cpml_arc_info(arc, NULL, NULL, &start, &end))
        return;

    angle = (end-start)*pos + start;
    cpml_vector_from_angle(vector, angle, 1.);
    cpml_vector_normal(vector);
}

/**
 * cpml_arc_near_pos:
 * @arc: the #CpmlPrimitive arc data
 * @pair: the coordinates of the subject point
 *
 * Returns the pos value of the point on @arc nearest to @pair.
 * The returned value is always between 0 and 1.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: the pos value, always between 0 and 1
 **/
double
cpml_arc_near_pos(const CpmlPrimitive *arc, const CpmlPair *pair)
{
    /* TODO */

    return 0;
}

/**
 * cpml_arc_intersection:
 * @arc:  the first arc
 * @arc2: the second arc
 * @dest: a vector of #CpmlPair
 * @max:  maximum number of intersections to return
 *        (that is, the size of @dest)
 *
 * Given two arcs (@arc and @arc2), gets their intersection points
 * and store the result in @dest. Keep in mind two arcs can have
 * up to 2 intersections.
 *
 * If @max is 0, the function returns 0 immediately without any
 * further processing. If @arc and @arc2 are cohincident (same
 * center and same radius), their intersections are not considered.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: the number of intersections found (max 2)
 *               or 0 if the primitives do not intersect
 **/
int
cpml_arc_intersection(const CpmlPrimitive *arc, const CpmlPrimitive *arc2,
                      CpmlPair *dest, int max)
{
    return 0;
}

/**
 * cpml_arc_intersection_with_line:
 * @arc:  an arc
 * @line: a line
 * @dest: a vector of #CpmlPair
 * @max:  maximum number of intersections to return
 *        (that is, the size of @dest)
 *
 * Given an @arc and a @line, gets their intersection points
 * and store the result in @dest. Keep in mind an arc and a
 * line can have up to 2 intersections.
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
 * Return value: the number of intersections found (max 2)
 *               or 0 if the primitives do not intersect
 **/
int
cpml_arc_intersection_with_line(const CpmlPrimitive *arc,
                                const CpmlPrimitive *line,
                                CpmlPair *dest, int max)
{
    return 0;
}

/**
 * cpml_arc_offset:
 * @arc:    the #CpmlPrimitive arc data
 * @offset: distance for the computed parallel arc
 *
 * Given an @arc, this function computes the parallel arc at
 * distance @offset. The three points needed to build the
 * new arc are returned in the @arc data (substituting the
 * previous ones.
 **/
void
cpml_arc_offset(CpmlPrimitive *arc, double offset)
{
    CpmlPair p[3], center;
    double r;

    cpml_pair_from_cairo(&p[0], arc->org);
    cpml_pair_from_cairo(&p[1], &arc->data[1]);
    cpml_pair_from_cairo(&p[2], &arc->data[2]);

    if (!get_center(p, &center))
        return;

    r = cpml_pair_distance(&p[0], &center) + offset;

    /* Offset the three points by calculating their vector from the center,
     * setting the new radius as length and readding the center */
    cpml_pair_sub(&p[0], &center);
    cpml_pair_sub(&p[1], &center);
    cpml_pair_sub(&p[2], &center);

    cpml_vector_set_length(&p[0], r);
    cpml_vector_set_length(&p[1], r);
    cpml_vector_set_length(&p[2], r);

    cpml_pair_add(&p[0], &center);
    cpml_pair_add(&p[1], &center);
    cpml_pair_add(&p[2], &center);

    cpml_pair_to_cairo(&p[0], arc->org);
    cpml_pair_to_cairo(&p[1], &arc->data[1]);
    cpml_pair_to_cairo(&p[2], &arc->data[2]);
}

/**
 * cpml_arc_to_cairo:
 * @arc: the #CpmlPrimitive arc data
 * @cr:  the destination cairo context
 *
 * Renders @arc to the @cr cairo context. As cairo does not support
 * arcs natively, it is approximated using one or more Bézier curves.
 *
 * The number of curves used is dependent from the angle of the arc.
 * Anyway, this function uses internally the hardcoded %M_PI_2 value
 * as threshold value. This means the maximum arc approximated by a
 * single curve will be a quarter of a circle and, consequently, a
 * whole circle will be approximated by 4 Bézier curves.
 **/
void
cpml_arc_to_cairo(const CpmlPrimitive *arc, cairo_t *cr)
{
    CpmlPair center;
    double r, start, end;
    int n_curves;
    double step, angle;
    CpmlPrimitive curve;
    cairo_path_data_t data[4];

    if (!cpml_arc_info(arc, &center, &r, &start, &end))
        return;

    n_curves = ceil(fabs(end-start) / ARC_MAX_ANGLE);
    step = (end-start) / (double) n_curves;
    curve.data = data;

    for (angle = start; n_curves--; angle += step) {
        arc_to_curve(&curve, &center, r, angle, angle+step);
        cairo_curve_to(cr,
                       curve.data[1].point.x, curve.data[1].point.y,
                       curve.data[2].point.x, curve.data[2].point.y,
                       curve.data[3].point.x, curve.data[3].point.y);
    }
}

/**
 * cpml_arc_to_curves:
 * @arc:      the #CpmlPrimitive arc data
 * @segment:  the destination #CpmlSegment
 * @n_curves: number of Bézier to use
 *
 * Converts @arc to a serie of @n_curves Bézier curves and puts them
 * inside @segment. Obviously, @segment must have enough space to
 * contain at least @n_curves curves.
 *
 * This function works in a similar way as cpml_arc_to_cairo() but
 * has two important differences: it does not need a cairo context
 * and the number of curves to be generated is explicitely defined.
 * The latter difference allows a more specific error control from
 * the application: in the file src/cairo-arc.c, found in the cairo
 * tarball (at least in cairo-1.9.1), there is a table showing the
 * magnitude of error of this curve approximation algorithm.
 **/
void
cpml_arc_to_curves(const CpmlPrimitive *arc, CpmlSegment *segment,
                   int n_curves)
{
    CpmlPair center;
    double r, start, end;
    double step, angle;
    CpmlPrimitive curve;

    if (!cpml_arc_info(arc, &center, &r, &start, &end))
        return;

    step = (end-start) / (double) n_curves;
    segment->num_data = n_curves*4;
    curve.segment = segment;
    curve.data = segment->data;

    for (angle = start; n_curves--; angle += step) {
        arc_to_curve(&curve, &center, r, angle, angle+step);
        curve.data += 4;
    }
}


static cairo_bool_t
get_center(const CpmlPair *p, CpmlPair *dest)
{
    CpmlPair b, c;
    double d, b2, c2;

    /* When p[0] == p[2], p[0]..p[1] is considered the diameter of a circle */
    if (p[0].x == p[2].x && p[0].y == p[2].y) {
        dest->x = (p[0].x + p[1].x) / 2;
        dest->y = (p[0].y + p[1].y) / 2;
        return 1;
    }

    /* Translate the 3 points of -p0, to simplify the formula */
    cpml_pair_sub(cpml_pair_copy(&b, &p[1]), &p[0]);
    cpml_pair_sub(cpml_pair_copy(&c, &p[2]), &p[0]);

    /* Check for division by 0, that is the case where the 3 given points
     * are laying on a straight line and there is no fitting circle */
    d = (b.x*c.y - b.y*c.x) * 2;
    if (d == 0.)
        return 0;

    b2 = b.x*b.x + b.y*b.y;
    c2 = c.x*c.x + c.y*c.y;

    dest->x = (c.y*b2 - b.y*c2) / d + p[0].x;
    dest->y = (b.x*c2 - c.x*b2) / d + p[0].y;

    return 1;
}

static void
get_angles(const CpmlPair *p, const CpmlPair *center,
           double *start, double *end)
{
    CpmlVector vector;
    double mid;

    /* Calculate the starting angle */
    cpml_pair_sub(cpml_pair_copy(&vector, &p[0]), center);
    *start = cpml_vector_angle(&vector);

    if (p[0].x == p[2].x && p[0].y == p[2].y) {
        /* When p[0] and p[2] are cohincidents, p[0]..p[1] is the diameter
         * of a circle: return by convention start=start end=start+2PI */
        *end = *start + M_PI*2;
    } else {
        /* Calculate the mid and end angle */
        cpml_pair_sub(cpml_pair_copy(&vector, &p[1]), center);
        mid = cpml_vector_angle(&vector);
        cpml_pair_sub(cpml_pair_copy(&vector, &p[2]), center);
        *end = cpml_vector_angle(&vector);

        if (*end > *start) {
            if (mid > *end || mid < *start)
                *start += M_PI*2;
        } else {
            if (mid < *end || mid > *start)
                *end += M_PI*2;
        }
    }
}

static void
arc_to_curve(CpmlPrimitive *curve, const CpmlPair *center,
             double r, double start, double end)
{
    double r_sin1, r_cos1;
    double r_sin2, r_cos2;
    double h;

    r_sin1 = r*sin(start);
    r_cos1 = r*cos(start);
    r_sin2 = r*sin(end);
    r_cos2 = r*cos(end);

    h = 4./3. * tan((end-start) / 4.);

    curve->data[0].header.type = CAIRO_PATH_CURVE_TO;
    curve->data[0].header.length = 4;
    curve->data[1].point.x = center->x + r_cos1 - h*r_sin1;
    curve->data[1].point.y = center->y + r_sin1 + h*r_cos1;
    curve->data[2].point.x = center->x + r_cos2 + h*r_sin2;
    curve->data[2].point.y = center->y + r_sin2 - h*r_cos2;
    curve->data[3].point.x = center->x + r_cos2;
    curve->data[3].point.y = center->y + r_sin2;
}
