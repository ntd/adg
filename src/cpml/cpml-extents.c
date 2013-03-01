/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:cpml-extents
 * @Section_Id:CpmlExtents
 * @title: CpmlExtents
 * @short_description: A rectangular area representing a bounding box
 *
 * The #CpmlExtents struct groups two pairs representing the rectangular
 * area of a bounding box.
 *
 * Since: 1.0
 **/

/**
 * CpmlExtents:
 * @is_defined: set to %0 when these extents are undefined
 * @org: the lowest x,y coordinates
 * @size: the width (x) and height (y) of the extents
 *
 * A structure defining a bounding box area. These APIs expect the
 * size of the extents to be always positives, so be careful while
 * directly accessing the @size field.
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"
#include "cpml-extents.h"
#include <string.h>
#include <math.h>


/**
 * cpml_extents_copy:
 * @extents: (out): the destination #CpmlExtents
 * @src:     (in):  the source #CpmlExtents
 *
 * Copies @src in @extents.
 *
 * Since: 1.0
 **/
void
cpml_extents_copy(CpmlExtents *extents, const CpmlExtents *src)
{
    memcpy(extents, src, sizeof(CpmlExtents));
}

/**
 * cpml_extents_from_cairo_text:
 * @extents:                      (out): the destination #CpmlExtents
 * @cairo_extents: (in) (type gpointer): the source #cairo_text_extents_t
 *
 * Converts @cairo_extents in a #CpmlExtents format and stores the
 * result in @extents.
 *
 * Since: 1.0
 **/
void
cpml_extents_from_cairo_text(CpmlExtents *extents,
                             const cairo_text_extents_t *cairo_extents)
{
    extents->is_defined = 1;
    extents->org.x = cairo_extents->x_bearing;
    extents->org.y = cairo_extents->y_bearing;
    extents->size.x = cairo_extents->width;
    extents->size.y = cairo_extents->height;
}

/**
 * cpml_extents_equal:
 * @extents: the first extents to compare
 * @src:     the second extents to compare
 *
 * Compares @extents to @src and returns 1 if the extents are equals.
 * Two %NULL or two undefined extents are considered equal, athough
 * %NULL extents are not equal to undefined extents.
 *
 * Returns: (type gboolean): %1 if @extents is equal to @src,
 *                           %0 otherwise
 *
 * Since: 1.0
 **/
int
cpml_extents_equal(const CpmlExtents *extents, const CpmlExtents *src)
{
    if (extents == NULL && src == NULL)
        return 1;

    if (extents == NULL || src == NULL)
        return 0;

    return (!extents->is_defined && !src->is_defined) ||
           (extents->is_defined && src->is_defined &&
               cpml_pair_equal(&extents->org,  &src->org) &&
               cpml_pair_equal(&extents->size, &src->size));
}

/**
 * cpml_extents_add:
 * @extents: (inout): the destination #CpmlExtents
 * @src:     (in):    the extents to add
 *
 * Merges @extents and @src and store the result in @extents.
 *
 * Since: 1.0
 **/
void
cpml_extents_add(CpmlExtents *extents, const CpmlExtents *src)
{
    CpmlPair pair;

    if (src->is_defined == 0)
        return;

    pair.x = src->org.x + src->size.x;
    pair.y = src->org.y + src->size.y;

    cpml_extents_pair_add(extents, &src->org);
    cpml_extents_pair_add(extents, &pair);
}

/**
 * cpml_extents_pair_add:
 * @extents: (inout): the source and destination #CpmlExtents
 * @src:     (in):    the #CpmlPair to add
 *
 * Extends @extents, if required, to include @src. If @extents is
 * undefined, the origin of @extents is set to @src and its size
 * will be (0,0).
 *
 * Since: 1.0
 **/
void
cpml_extents_pair_add(CpmlExtents *extents, const CpmlPair *src)
{
    if (extents->is_defined == 0) {
        extents->is_defined = 1;
        cpml_pair_copy(&extents->org, src);
        extents->size.x = 0;
        extents->size.y = 0;
        return;
    }

    if (src->x < extents->org.x) {
        extents->size.x += extents->org.x - src->x;
        extents->org.x = src->x;
    } else if (src->x > extents->org.x + extents->size.x) {
        extents->size.x = src->x - extents->org.x;
    }

    if (src->y < extents->org.y) {
        extents->size.y += extents->org.y - src->y;
        extents->org.y = src->y;
    } else if (src->y > extents->org.y + extents->size.y) {
        extents->size.y = src->y - extents->org.y;
    }
}

/**
 * cpml_extents_is_inside:
 * @extents: the container #CpmlExtents
 * @src:     the subject #CpmlExtents
 *
 * Checks wheter @src is enterely contained by @extents. If @extents
 * is undefined, %0 will be returned. If @src is undefined, %1 will
 * be returned. The border of @extents is considered inside.
 *
 * Returns: (type gboolean): %1 if @src is totally inside @extents,
 *                           %0 otherwise
 *
 * Since: 1.0
 **/
int
cpml_extents_is_inside(const CpmlExtents *extents, const CpmlExtents *src)
{
    CpmlPair pe, ps;

    if (extents->is_defined == 0)
        return 0;

    if (src->is_defined == 0)
        return 1;

    cpml_pair_copy(&pe, &extents->org);
    cpml_pair_copy(&ps, &src->org);

    if (ps.x < pe.x || ps.y < pe.y)
        return 0;

    pe.x += extents->size.x;
    pe.y += extents->size.y;
    ps.x += extents->size.x;
    ps.y += extents->size.y;

    return ps.x <= pe.x && ps.y <= pe.y;
}

/**
 * cpml_extents_pair_is_inside:
 * @extents: the container #CpmlExtents
 * @src:     the subject #CpmlPair
 *
 * Checks wheter @src is inside @extents. If @extents is undefined,
 * %0 will be returned. The border of @extents is considered inside.
 *
 * Returns: (type gboolean): %1 if @src is inside @extents,
 *                           %0 otherwise
 *
 * Since: 1.0
 **/
int
cpml_extents_pair_is_inside(const CpmlExtents *extents, const CpmlPair *src)
{
    if (extents->is_defined == 0 ||
        src->x < extents->org.x || src->y < extents->org.y ||
        src->x > extents->org.x + extents->size.x ||
        src->y > extents->org.y + extents->size.y)
        return 0;

    return 1;
}

/**
 * cpml_extents_transform:
 * @extents: (inout): the container #CpmlExtents
 * @matrix:  (in):    the transformation matrix
 *
 * Transforms the four corners of @extents with @matrix and
 * recomputes @extents. This will logically equivalent to transform
 * an extents box and gets the extents of the resulting shape.
 *
 * Since: 1.0
 **/
void
cpml_extents_transform(CpmlExtents *extents, const cairo_matrix_t *matrix)
{
    CpmlPair p[4];

    if (extents->is_defined == 0)
        return;

    p[0] = extents->org;
    p[1].x = extents->org.x + extents->size.x;
    p[1].y = extents->org.y;
    p[2].x = extents->org.x + extents->size.x;
    p[2].y = extents->org.y + extents->size.y;
    p[3].x = extents->org.x;
    p[3].y = extents->org.y + extents->size.y;

    cairo_matrix_transform_point(matrix, &p[0].x, &p[0].y);
    cairo_matrix_transform_point(matrix, &p[1].x, &p[1].y);
    cairo_matrix_transform_point(matrix, &p[2].x, &p[2].y);
    cairo_matrix_transform_point(matrix, &p[3].x, &p[3].y);

    extents->is_defined = 0;
    cpml_extents_pair_add(extents, &p[0]);
    cpml_extents_pair_add(extents, &p[1]);
    cpml_extents_pair_add(extents, &p[2]);
    cpml_extents_pair_add(extents, &p[3]);
}
