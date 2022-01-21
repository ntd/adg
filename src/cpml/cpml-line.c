/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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
static int      intersection            (const CpmlPair         *p1_4,
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

    cpml_primitive_put_point(line, 0, &p1);
    cpml_primitive_put_point(line, -1, &p2);

    return cpml_pair_distance(&p1, &p2);
}

static void
put_extents(const CpmlPrimitive *line, CpmlExtents *extents)
{
    CpmlPair p1, p2;

    extents->is_defined = 0;

    cpml_primitive_put_point(line, 0, &p1);
    cpml_primitive_put_point(line, -1, &p2);

    cpml_extents_pair_add(extents, &p1);
    cpml_extents_pair_add(extents, &p2);
}

static void
put_pair_at(const CpmlPrimitive *line, double pos, CpmlPair *pair)
{
    CpmlPair p1, p2;

    cpml_primitive_put_point(line, 0, &p1);
    cpml_primitive_put_point(line, -1, &p2);

    pair->x = p1.x + (p2.x - p1.x) * pos;
    pair->y = p1.y + (p2.y - p1.y) * pos;
}

static void
put_vector_at(const CpmlPrimitive *line, double pos, CpmlVector *vector)
{
    CpmlPair p1, p2;

    cpml_primitive_put_point(line, 0, &p1);
    cpml_primitive_put_point(line, -1, &p2);

    vector->x = p2.x - p1.x;
    vector->y = p2.y - p1.y;
}

static double
get_closest_pos(const CpmlPrimitive *line, const CpmlPair *pair)
{
    CpmlPair p1_4[4];
    CpmlVector normal;
    CpmlPair dummy;
    double pos;

    cpml_primitive_put_point(line, 0, &p1_4[0]);
    cpml_primitive_put_point(line, -1, &p1_4[1]);

    normal.x = p1_4[1].x - p1_4[0].x;
    normal.y = p1_4[1].y - p1_4[0].y;
    cpml_vector_normal(&normal);

    cpml_pair_copy(&p1_4[2], pair);

    p1_4[3].x = pair->x + normal.x;
    p1_4[3].y = pair->y + normal.y;

    /* Ensure to return 0 if intersection() fails */
    pos = 0;
    /* The destination pair cannot be NULL to avoid crashes */
    intersection(p1_4, &dummy, &pos);

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

    cpml_primitive_put_point(line, 0, &p1_4[0]);
    cpml_primitive_put_point(line, -1, &p1_4[1]);
    cpml_primitive_put_point(primitive, 0, &p1_4[2]);
    cpml_primitive_put_point(primitive, -1, &p1_4[3]);

    return intersection(p1_4, dest, NULL) ? 1 : 0;
}

static void
offset(CpmlPrimitive *line, double offset)
{
    CpmlPair p1, p2;
    CpmlVector normal;

    cpml_primitive_put_point(line, 0, &p1);
    cpml_primitive_put_point(line, -1, &p2);

    put_vector_at(line, 0, &normal);
    cpml_vector_normal(&normal);
    cpml_vector_set_length(&normal, offset);

    p1.x += normal.x;
    p1.y += normal.y;
    p2.x += normal.x;
    p2.y += normal.y;

    cpml_primitive_set_point(line, 0, &p1);
    cpml_primitive_set_point(line, -1, &p2);
}

static int
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

    dest->x = p1_4[0].x + v[0].x * factor;
    dest->y = p1_4[0].y + v[0].y * factor;

    if (get_factor != NULL)
        *get_factor = factor;

    return 1;
}
