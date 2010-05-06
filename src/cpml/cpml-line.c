/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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

#include "cpml-internal.h"
#include "cpml-extents.h"
#include "cpml-segment.h"
#include "cpml-primitive.h"
#include "cpml-primitive-private.h"


static double   get_length              (const CpmlPrimitive    *line);
static void     put_extents             (const CpmlPrimitive    *line,
                                         CpmlExtents            *extents);
static void     put_pair_at             (const CpmlPrimitive    *line,
                                         double                  pos,
                                         CpmlPair               *pair);
static void     put_vector_at           (const CpmlPrimitive    *line,
                                         double                  pos,
                                         CpmlVector             *vector);
static double   get_closest_pos         (const CpmlPrimitive    *line,
                                         const CpmlPair         *pair);
static size_t   put_intersections       (const CpmlPrimitive    *line,
                                         const CpmlPrimitive    *primitive,
                                         size_t                  n_dest,
                                         CpmlPair               *dest);
static void     offset                  (CpmlPrimitive          *line,
                                         double                  offset);
static cairo_bool_t
                intersection            (const CpmlPair         *p1_4,
                                         CpmlPair               *dest,
                                         double                 *get_factor);


const _CpmlPrimitiveClass *
_cpml_line_get_class(void)
{
    static _CpmlPrimitiveClass *p_class = NULL;

    if (p_class == NULL) {
        static _CpmlPrimitiveClass class_data = {
            "line to", 2,
            get_length,
            put_extents,
            put_pair_at,
            put_vector_at,
            get_closest_pos,
            put_intersections,
            offset,
            NULL
        };
        p_class = &class_data;
    }

    return p_class;
}

const _CpmlPrimitiveClass *
_cpml_close_get_class(void)
{
    static _CpmlPrimitiveClass *p_class = NULL;

    if (p_class == NULL) {
        static _CpmlPrimitiveClass class_data = {
            "close", 2,
            get_length,
            put_extents,
            put_pair_at,
            put_vector_at,
            get_closest_pos,
            put_intersections,
            offset,
            NULL
        };
        p_class = &class_data;
    }

    return p_class;
}


static double
get_length(const CpmlPrimitive *line)
{
    CpmlPair p1, p2;

    cpml_pair_from_cairo(&p1, cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p2, cpml_primitive_get_point(line, -1));

    return cpml_pair_distance(&p1, &p2);
}

static void
put_extents(const CpmlPrimitive *line, CpmlExtents *extents)
{
    CpmlPair p1, p2;

    extents->is_defined = 0;

    cpml_pair_from_cairo(&p1, cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p2, cpml_primitive_get_point(line, -1));

    cpml_extents_pair_add(extents, &p1);
    cpml_extents_pair_add(extents, &p2);
}

static void
put_pair_at(const CpmlPrimitive *line, double pos, CpmlPair *pair)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, -1);

    pair->x = p1->point.x + (p2->point.x - p1->point.x) * pos;
    pair->y = p1->point.y + (p2->point.y - p1->point.y) * pos;
}

static void
put_vector_at(const CpmlPrimitive *line, double pos, CpmlVector *vector)
{
    cairo_path_data_t *p1, *p2;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, -1);

    vector->x = p2->point.x - p1->point.x;
    vector->y = p2->point.y - p1->point.y;
}

static double
get_closest_pos(const CpmlPrimitive *line, const CpmlPair *pair)
{
    CpmlPair p1_4[4];
    CpmlVector normal;
    double pos;

    cpml_pair_from_cairo(&p1_4[0], cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p1_4[1], cpml_primitive_get_point(line, -1));

    normal.x = p1_4[1].x - p1_4[2].x;
    normal.y = p1_4[1].y - p1_4[2].y;
    cpml_vector_normal(&normal);

    cpml_pair_copy(&p1_4[2], pair);

    p1_4[3].x = pair->x + normal.x;
    p1_4[3].y = pair->y + normal.y;

    /* Ensure to return 0 if intersection() fails */
    pos = 0;
    intersection(p1_4, NULL, &pos);

    /* Clamp the result to 0..1 */
    if (pos < 0)
        pos = 0;
    else if (pos > 1.)
        pos = 1.;

    return pos;
}

static size_t
put_intersections(const CpmlPrimitive *line, const CpmlPrimitive *primitive,
                  size_t n_dest, CpmlPair *dest)
{
    CpmlPair p1_4[4];

    if (n_dest == 0)
        return 0;

    cpml_pair_from_cairo(&p1_4[0], cpml_primitive_get_point(line, 0));
    cpml_pair_from_cairo(&p1_4[1], cpml_primitive_get_point(line, -1));
    cpml_pair_from_cairo(&p1_4[2], cpml_primitive_get_point(primitive, 0));
    cpml_pair_from_cairo(&p1_4[3], cpml_primitive_get_point(primitive, -1));

    return intersection(p1_4, dest, NULL) ? 1 : 0;
}

static void
offset(CpmlPrimitive *line, double offset)
{
    cairo_path_data_t *p1, *p2;
    CpmlVector normal;

    p1 = cpml_primitive_get_point(line, 0);
    p2 = cpml_primitive_get_point(line, -1);

    put_vector_at(line, 0, &normal);
    cpml_vector_normal(&normal);
    cpml_vector_set_length(&normal, offset);

    p1->point.x += normal.x;
    p1->point.y += normal.y;
    p2->point.x += normal.x;
    p2->point.y += normal.y;
}

static cairo_bool_t
intersection(const CpmlPair *p1_4, CpmlPair *dest, double *get_factor)
{
    CpmlVector v[2];
    double factor;

    v[0].x = p1_4[1].x - p1_4[0].x;
    v[0].y = p1_4[1].y - p1_4[0].y;
    v[1].x = p1_4[3].x - p1_4[2].x;
    v[1].y = p1_4[3].y - p1_4[2].y;
    factor = v[0].x * v[1].y - v[0].y * v[1].x;

    /* Check for equal slopes (the lines are parallel) */
    if (factor == 0)
        return 0;

    factor = ((p1_4[0].y - p1_4[2].y) * v[1].x -
              (p1_4[0].x - p1_4[2].x) * v[1].y) / factor;

    if (dest != NULL) {
        dest->x = p1_4[0].x + v[0].x * factor;
        dest->y = p1_4[0].y + v[0].y * factor;
    }

    if (get_factor != NULL)
        *get_factor = factor;

    return 1;
}
