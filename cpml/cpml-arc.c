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
 * @title: Circular arcs
 * @short_description: Functions for manipulating circular arcs
 *
 * The following functions manipulate %CAIRO_PATH_ARC_TO #CpmlPrimitive.
 * No check is made on the primitive struct, so be sure
 * <structname>CpmlPrimitive</structname> is effectively an arc
 * before calling these APIs.
 *
 * An arc is not a native cairo primitive and should be treated specially.
 *
 * The arc primitive is defined by 3 points: the first one is the usual
 * implicit point got from the previous primitive, the second point is
 * an arbitrary intermediate point laying on the arc and the third point
 * is the end of the arc. These points identify univocally an arc:
 * furthermore, the intermediate point also gives the "direction" of
 * the arc.
 *
 * As a special case, when the first point is coincident with the end
 * point, the primitive is considered a circle with diameter defined
 * as the segment between the first point and the intermediate point.
 **/

#include "cpml-arc.h"
#include "cpml-pair.h"

#include <stdlib.h>
#include <math.h>


/* Hardcoded max angle of the arc to be approximated by a Bézier curve:
 * this influence the arc quality (the default value is got from cairo) */
#define ARC_MAX_ANGLE   (M_PI / 2.)


static cairo_bool_t     get_center      (const CpmlPair *p,
                                         CpmlPair       *dest);
static void             get_angles      (const CpmlPair *p,
                                         const CpmlPair *center,
                                         CpmlPair       *angles);


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
        CpmlPair angles;

        get_angles(p, &l_center, &angles);

        if (start != NULL)
            *start = angles.x;
        if (end != NULL)
            *end = angles.y;
    }

    return 1;
}

/**
 * cpml_arc_to_cairo:
 * @arc:  the #CpmlPrimitive arc data
 * @cr: the destination cairo context
 *
 * Appends the path of @primitive to @cr using cairo_append_path().
 * As a special case, if the primitive is a #CAIRO_PATH_CLOSE_PATH,
 * an equivalent line is rendered, because a close path left alone
 * is not renderable.
 * @pair: the destination #CpmlPair
 *
 * Given an @arc, this function renders it to the @cr cairo context
 * approximating it using only Bézier curves.
 **/
void
cpml_arc_to_cairo(const CpmlPrimitive *arc, cairo_t *cr)
{
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
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 **/
void
cpml_arc_pair_at(const CpmlPrimitive *arc, CpmlPair *pair, double pos)
{
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
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 **/
void
cpml_arc_vector_at(const CpmlPrimitive *arc, CpmlVector *vector, double pos)
{
}

/**
 * cpml_arc_intersection:
 * @arc:  the first arc
 * @arc2: the second arc
 * @dest: a vector of at least 2 #CpmlPair
 *
 * Given two arcs (@arc and @arc2), gets their intersection points
 * and store the result in @dest. Because two arcs can have
 * 2 intersections, @dest MUST be at least an array of 2 #CpmlPair.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: the number of intersections (max 2)
 **/
int
cpml_arc_intersection(const CpmlPrimitive *arc,
                      const CpmlPrimitive *arc2, CpmlPair *dest)
{
    return 0;
}

/**
 * cpml_arc_intersection_with_line:
 * @arc:  an arc
 * @line: a line
 * @dest: a vector of at least 2 #CpmlPair
 *
 * Given an @arc and a @line, gets their intersection points
 * and store the result in @dest. Because an arc and a line
 * can have up to 2 intersections, @dest MUST be at least an
 * array of 2 #CpmlPair.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: the number of intersections (max 2)
 **/
int
cpml_arc_intersection_with_line(const CpmlPrimitive *arc,
                                const CpmlPrimitive *line, CpmlPair *dest)
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


static cairo_bool_t
get_center(const CpmlPair *p, CpmlPair *dest)
{
    CpmlPair div;
    double p2[3];

    /* When p[0] == p[2], p[0]..p[1] is considered the diameter of a circle */
    if (p[0].x == p[2].x && p[0].y == p[2].y) {
        dest->x = (p[0].x + p[1].x) / 2;
        dest->y = (p[0].y + p[1].y) / 2;
        return 1;
    }

    div.x = (p[0].x*(p[2].y-p[1].y) +
             p[1].x*(p[0].y-p[2].y) +
             p[1].x*(p[1].y-p[0].y)) * 2;
    div.y = (p[0].y*(p[2].x-p[1].x) +
             p[1].y*(p[0].x-p[2].x) +
             p[2].y*(p[1].x-p[0].x)) * 2;

    /* Check for division by 0, that is the case where the 3 given points
     * are laying on a straight line) */
    if (div.x == 0. || div.y == 0.)
        return 0;

    p2[0] = p[0].x*p[0].x + p[0].y*p[0].y;
    p2[1] = p[1].x*p[1].x + p[1].y*p[1].y;
    p2[2] = p[2].x*p[2].x + p[2].y*p[2].y;

    /* Compute the center of the arc */
    dest->x = (p2[0] * (p[2].y - p[1].y) +
               p2[1] * (p[0].y - p[2].y) +
               p2[2] * (p[1].y - p[0].y)) / div.x;
    dest->y = (p2[0] * (p[2].x - p[1].x) +
               p2[1] * (p[0].x - p[2].x) +
               p2[2] * (p[1].x - p[0].x)) / div.y;

    return 1;
}

static void
get_angles(const CpmlPair *p, const CpmlPair *center, CpmlPair *angles)
{
    CpmlVector vector;
    double start, mid, end;

    /* Calculate the starting angle */
    cpml_pair_sub(cpml_pair_copy(&vector, &p[0]), center);
    start = cpml_vector_angle(&vector);

    if (p[0].x == p[2].x && p[0].y == p[2].y) {
        /* When p[0] and p[2] are cohincidents, p[0]..p[1] is the diameter
         * of a circle: return by convention start=start end=start+2PI */
        end = start + M_PI*2;
    } else {
        /* Calculate the mid and end angle */
        cpml_pair_sub(cpml_pair_copy(&vector, &p[1]), center);
        mid = cpml_vector_angle(&vector);
        cpml_pair_sub(cpml_pair_copy(&vector, &p[2]), center);
        end = cpml_vector_angle(&vector);

        if (end > start) {
            if (mid > end || mid < start)
                start += M_PI*2;
        } else {
            if (mid < end || mid > start)
                end += M_PI*2;
        }
    }

    angles->x = start;
    angles->y = end;
}
