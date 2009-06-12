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
 **/

#include "cpml-arc.h"
#include "cpml-pair.h"

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
 * cpml_arc_center:
 * @arc:  the #CpmlPrimitive arc data
 * @pair: the destination #CpmlPair
 *
 * Given an @arc, this function computes its center and
 * returns the coordinates in the @pair struct.
 **/
void
cpml_arc_center(const CpmlPrimitive *arc, CpmlPair *pair)
{
    double p0_2, p1_2, p2_2;
    CpmlPair p0, p1, p2;
    CpmlPair div;

    cpml_pair_from_cairo(&p0, arc->org);
    cpml_pair_from_cairo(&p1, &arc->data[1]);
    cpml_pair_from_cairo(&p2, &arc->data[2]);

    div.x = 2. * (p0.x*(p2.y-p1.y) + p1.x*(p0.y-p2.y) + p1.x*(p1.y-p0.y));
    if (div.x == 0)
        return;

    div.y = 2. * (p0.y*(p2.x-p1.x) + p1.y*(p0.x-p2.x) + p2.y*(p1.x-p0.x));
    if (div.y == 0)
        return;

    p0_2 = p0.x*p0.x + p0.y*p0.y;
    p1_2 = p1.x*p1.x + p1.y*p1.y;
    p2_2 = p2.x*p2.x + p2.y*p2.y;

    /* Compute the center of the arc */
    pair->x = (p0_2*(p2.y-p1.y) + p1_2*(p0.y-p2.y) + p2_2*(p1.y-p0.y)) / div.x;
    pair->y = (p0_2*(p2.x-p1.x) + p1_2*(p0.x-p2.x) + p2_2*(p1.x-p0.x)) / div.y;
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
    CpmlPair p0, p1, p2, center;
    double r;

    cpml_pair_from_cairo(&p0, arc->org);
    cpml_pair_from_cairo(&p1, &arc->data[1]);
    cpml_pair_from_cairo(&p2, &arc->data[2]);
    cpml_arc_center(arc, &center);

    r = cpml_pair_distance(&p0, &center);
    r += offset;

    /* Offset the points by calculating their vector referred to the center,
     * setting the new radius as length and readding the center */
    cpml_pair_sub(&p0, &center);
    cpml_pair_sub(&p1, &center);
    cpml_pair_sub(&p2, &center);

    cpml_vector_set_length(&p0, r);
    cpml_vector_set_length(&p1, r);
    cpml_vector_set_length(&p2, r);

    cpml_pair_add(&p0, &center);
    cpml_pair_add(&p1, &center);
    cpml_pair_add(&p2, &center);

    cpml_pair_to_cairo(&p0, arc->org);
    cpml_pair_to_cairo(&p1, &arc->data[1]);
    cpml_pair_to_cairo(&p2, &arc->data[2]);
}
