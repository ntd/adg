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

#include "cpml-action.h"

static int      line_line       (const CpmlPrimitive    *line1,
                                 const CpmlPrimitive    *line2,
                                 CpmlPair               *dest);
static int      line_curve      (const CpmlPrimitive    *line,
                                 const CpmlPrimitive    *curve,
                                 CpmlPair               *dest);
static int      curve_curve     (const CpmlPrimitive    *curve1,
                                 const CpmlPrimitive    *curve2,
                                 CpmlPair               *dest);

/**
 * cpml_intersection:
 * @primitive1: the first #CpmlPrimitive
 * @primitive2: the second #CpmlPrimitive
 * @dest:       the destination #CpmlPair (or a vector of #CpmlPair)
 *
 * Finds the intersection points between the given primitives and
 * returns the result in @dest. If curves are involved, @dest MUST
 * be an array of at least 4 #CpmlPair, because this can lead to
 * 4 intersection points.
 *
 * Return value: the number of intersection points found
 **/
int
cpml_intersection(const CpmlPrimitive *primitive1,
                  const CpmlPrimitive *primitive2, CpmlPair *dest)
{
    cairo_path_data_type_t type1, type2;

    type1 = primitive1->data->header.type;
    type2 = primitive1->data->header.type;

    /* Close path primitives are treated as line-to */
    if (type1 == CAIRO_PATH_CLOSE_PATH)
        type1 = CAIRO_PATH_LINE_TO;
    if (type2 == CAIRO_PATH_CLOSE_PATH)
        type2 = CAIRO_PATH_LINE_TO;

    /* Dispatcher: probably there's a smarter way to do this */
    if (type1 == CAIRO_PATH_LINE_TO && type2 == CAIRO_PATH_LINE_TO)
        return line_line(primitive1, primitive2, dest);
    else if (type1 == CAIRO_PATH_CURVE_TO && type2 == CAIRO_PATH_CURVE_TO)
        return curve_curve(primitive1, primitive2, dest);
    else if (type1 == CAIRO_PATH_LINE_TO && type2 == CAIRO_PATH_CURVE_TO)
        return line_curve(primitive1, primitive2, dest);
    else if (type1 == CAIRO_PATH_CURVE_TO && type2 == CAIRO_PATH_LINE_TO)
        return line_curve(primitive2, primitive1, dest);

    /* Primitive combination not found */
    return 0;
}

/**
 * cpml_intersection_line_line:
 * @line1: the first line
 * @to:   the second line
 * @dest: the destination #CpmlPair
 *
 * Given two lines (@line1 and @line2), gets their intersection point
 * and store the result in @dest.
 *
 * Return value: 1 on success, 0 on no intersections
 **/
static int
line_line(const CpmlPrimitive *line1, const CpmlPrimitive *line2,
          CpmlPair *dest)
{
    const cairo_path_data_t *p1a, *p2a, *p1b, *p2b;
    CpmlVector va, vb;
    double factor;

    p1a = cpml_primitive_get_point(line1, 0);
    p2a = cpml_primitive_get_point(line1, 1);
    p1b = cpml_primitive_get_point(line2, 0);
    p2b = cpml_primitive_get_point(line2, 1);

    va.x = p2a->point.x - p1a->point.x;
    va.y = p2a->point.y - p1a->point.y;
    vb.x = p2b->point.x - p1b->point.x;
    vb.y = p2b->point.y - p1b->point.y;

    factor = va.x * vb.y - va.y * vb.x;

    /* Check if the lines are parallel */
    if (factor == 0)
        return 0;

    factor = ((p1a->point.y - p1b->point.y) * vb.x -
              (p1a->point.x - p1b->point.x) * vb.y) / factor;

    dest->x = p1a->point.x + va.x * factor;
    dest->y = p1a->point.y + va.y * factor;

    return 1;
}

/**
 * line_curve:
 * @line:  a line
 * @curve: a curve
 * @dest:  a vector of at least 4 #CpmlPair
 *
 * Given a @line and a Bézier cubic @curve, gets their intersection points
 * and store the result in @dest. Because a line and a cubic curve can
 * have 4 intersections, @dest MUST be at least an array of 4 #CpmlPair.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: the number of intersections (max 4)
 **/
static int
line_curve(const CpmlPrimitive *line, const CpmlPrimitive *curve,
           CpmlPair *dest)
{
    return 0;
}

/**
 * curve_curve:
 * @curve1: the first curve
 * @curve2: the second curve
 * @dest:   a vector of at least 4 #CpmlPair
 *
 * Given two Bézier cubic curves (@curve1 and @curve2), gets their
 * intersection points and store the result in @dest. Because two
 * curves can have 4 intersections, @dest MUST be at least an array
 * of 4 #CpmlPair.
 *
 * <important>
 * <title>TODO</title>
 * <itemizedlist>
 * <listitem>To be implemented...</listitem>
 * </itemizedlist>
 * </important>
 *
 * Return value: the number of intersections (max 4)
 **/
static int
curve_curve(const CpmlPrimitive *curve1, const CpmlPrimitive *curve2,
            CpmlPair *dest)
{
    return 0;
}
