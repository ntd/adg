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
 * SECTION:segment
 * @title: CpmlSegment
 * @short_description: Contiguous lines
 *
 * A segment is a single contiguous line got from a cairo path.
 *
 * Every #cairo_path_t struct can contain more than one segment:
 * the CPML library provides iteration APIs to browse these segments.
 * The CpmlSegment struct internally keeps a reference to the source
 * cairo path so it can be used both for getting segment data and
 * browsing the segments (that is, it is used also as an iterator).
 *
 * Use cpml_segment_reset() to reset the iterator at the start of the cairo
 * path (at the first segment) and cpml_segment_next() to get the next
 * segment. Getting the previous segment is not provided, as the underlying
 * struct is not accessible in reverse order.
 **/

/**
 * CpmlSegment:
 * @source: the source #cairo_path_t struct
 * @data: the segment data
 * @num_data: size of @data
 *
 * This is an unobtrusive struct to identify a segment inside a
 * cairo path. Unobtrusive means that the real coordinates are
 * still stored in @source: CpmlSegment only provides a way to
 * access the underlying cairo path.
 **/

#include "cpml-segment.h"
#include "cpml-pair.h"
#include "cpml-macros.h"
#include "cpml-alloca.h"

#include <stdio.h>
#include <string.h>

static cairo_bool_t     segment_normalize       (CpmlSegment       *segment);
static void             line_offset             (CpmlPair          *p,
                                                 CpmlVector        *vector,
                                                 double             offset);
static void             curve_offset            (CpmlPair          *p,
                                                 CpmlVector        *vector,
                                                 double             offset);
static void             join_primitives         (cairo_path_data_t *last_data,
                                                 const CpmlVector  *last_vector,
                                                 const CpmlPair    *new_point,
                                                 const CpmlVector  *new_vector);


/**
 * cpml_segment_from_cairo:
 * @segment: a #CpmlSegment
 * @cairo_path: the source cairo_path_t
 *
 * Builds a CpmlSegment from a cairo_path_t structure. This operation
 * involves stripping the leading %CAIRO_PATH_MOVE_TO primitives and
 * setting the internal segment structure accordling. A pointer to the
 * source cairo path is kept.
 *
 * This function will fail if @cairo_path is null, empty or if its
 * %status member is not %CAIRO_STATUS_SUCCESS. Also, the first
 * primitive must be a CAIRO_PATH_MOVE_TO, so no dependency on the
 * cairo context is needed.
 *
 * Return value: 1 on success, 0 on errors
 **/
cairo_bool_t
cpml_segment_from_cairo(CpmlSegment *segment, cairo_path_t *cairo_path)
{
    /* The cairo path should be defined and in perfect state */
    if (cairo_path == NULL || cairo_path->num_data == 0 ||
        cairo_path->status != CAIRO_STATUS_SUCCESS)
        return 0;

    segment->source = cairo_path;
    segment->data = cairo_path->data;
    segment->num_data = cairo_path->num_data;

    return segment_normalize(segment);
}

/**
 * cpml_segment_copy:
 * @segment: a #CpmlSegment
 * @src: the source segment to copy
 *
 * Makes a shallow copy of @src into @segment.
 *
 * Return value: @segment or %NULL on errors
 **/
CpmlSegment *
cpml_segment_copy(CpmlSegment *segment, const CpmlSegment *src)
{
    if (segment == NULL || src == NULL)
        return NULL;

    return memcpy(segment, src, sizeof(CpmlSegment));
}

/**
 * cpml_segment_dump:
 * @segment: a #CpmlSegment
 *
 * Dumps the specified @segment to stdout. Useful for debugging purposes.
 **/
void
cpml_segment_dump(const CpmlSegment *segment)
{
    cairo_path_data_t *data;
    int n, n_point;

    for (n = 0; n < segment->num_data; ++n) {
	data = segment->data + n;

	switch (data->header.type) {
	case CAIRO_PATH_MOVE_TO:
	    printf("Move to ");
	    break;
	case CAIRO_PATH_LINE_TO:
	    printf("Line to ");
	    break;
	case CAIRO_PATH_CURVE_TO:
	    printf("Curve to ");
	    break;
	case CAIRO_PATH_CLOSE_PATH:
	    printf("Path close");
	    break;
	default:
	    printf("Unknown entity (%d)", data->header.type);
	    break;
	}

	for (n_point = 1; n_point < data->header.length; ++n_point)
	    printf("(%lf, %lf) ", data[n_point].point.x,
                   data[n_point].point.y);

	n += n_point - 1;
	printf("\n");
    }
}

/**
 * cpml_segment_reset:
 * @segment: a #CpmlSegment
 *
 * Modifies @segment to point to the first segment of the source path.
 **/
void
cpml_segment_reset(CpmlSegment *segment)
{
    segment->data = segment->source->data;
    segment->num_data = segment->source->num_data;
    segment_normalize(segment);
}

/**
 * cpml_segment_next:
 * @segment: a #CpmlSegment
 *
 * Modifies @segment to point to the next segment of the source path.
 *
 * Return value: 1 on success, 0 if no next segment found or errors
 **/
cairo_bool_t
cpml_segment_next(CpmlSegment *segment)
{
    int rest = segment->source->num_data - segment->num_data +
        segment->source->data - segment->data;

    if (rest <= 0)
        return 0;

    segment->data += segment->num_data;
    segment->num_data = rest;

    return segment_normalize(segment);
}

/**
 * cpml_segment_reverse:
 * @segment: a #CpmlSegment
 *
 * Reverses @segment in-place. The resulting rendering will be the same,
 * but with the primitives generated in reverse order.
 **/
void
cpml_segment_reverse(CpmlSegment *segment)
{
    cairo_path_data_t *data, *dst_data;
    size_t data_size;
    double end_x, end_y;
    int n, num_points, n_point;
    const cairo_path_data_t *src_data;

    data_size = sizeof(cairo_path_data_t) * segment->num_data;
    data = cpml_alloca(data_size);
    end_x = segment->data[1].point.x;
    end_y = segment->data[1].point.y;

    for (n = 2; n < segment->num_data; ++n) {
        src_data = segment->data + n;
	num_points = src_data->header.length;

        dst_data = data + segment->num_data - n - num_points + 2;
        dst_data->header.type = src_data->header.type;
        dst_data->header.length = num_points;

	for (n_point = 1; n_point < num_points; ++n_point) {
            dst_data[num_points - n_point].point.x = end_x;
            dst_data[num_points - n_point].point.y = end_y;
            end_x = src_data[n_point].point.x;
            end_y = src_data[n_point].point.y;
        }

	n += n_point - 1;
    }

    data[0].header.type = CAIRO_PATH_MOVE_TO;
    data[0].header.length = 2;
    data[1].point.x = end_x;
    data[1].point.y = end_y;
    memcpy(segment->data, data, data_size);
}

/**
 * cpml_segment_transform:
 * @segment: a #CpmlSegment
 * @matrix: the matrix to be applied
 *
 * Applies @matrix on all the points of @segment.
 **/
void
cpml_segment_transform(CpmlSegment *segment, const cairo_matrix_t *matrix)
{
    cairo_path_data_t *data;
    int n, n_point, num_points;

    data = segment->data;

    for (n = 0; n < segment->num_data; n += num_points) {
        num_points = data->header.length;
        ++data;
        for (n_point = 1; n_point < num_points; ++n_point) {
            cairo_matrix_transform_point(matrix, &data->point.x, &data->point.y);
            ++data;
        }
    }
}

/**
 * cpml_segment_offset:
 * @segment: a #CpmlSegment
 * @offset: the offset distance
 *
 * Offsets a segment of the specified amount, that is builds a "parallel"
 * segment at the @offset distance from the original one and returns the
 * result by replacing the original @segment.
 *
 * Return value: 1 on success, 0 on errors
 *
 * @todo: closed path are not yet managed; the solution is not so obvious.
 **/
cairo_bool_t
cpml_segment_offset(CpmlSegment *segment, double offset)
{
    int n, num_points, n_point;
    cairo_path_data_t *data;
    cairo_path_data_t *last_data;
    CpmlVector v_old, v_new;
    CpmlPair p_old;
    CpmlPair p[4];

    last_data = NULL;
    p_old.x = 0;
    p_old.y = 0;

    for (n = 0; n < segment->num_data; n += data->header.length) {
        data = segment->data + n;
        num_points = data->header.length - 1;

        /* Fill the p[] vector */
        cpml_pair_copy(&p[0], &p_old);
        for (n_point = 1; n_point <= num_points; ++ n_point) {
            p[n_point].x = data[n_point].point.x;
            p[n_point].y = data[n_point].point.y;
        }

        /* Save the last direction vector in v_old */
        cpml_pair_copy(&v_old, &v_new);

        switch (data->header.type) {

        case CAIRO_PATH_MOVE_TO:
            v_new.x = 0;
            v_new.y = 0;
            break;

        case CAIRO_PATH_LINE_TO:
            line_offset(p, &v_new, offset);
            break;

        case CAIRO_PATH_CURVE_TO:
            curve_offset(p, &v_new, offset);
            break;

        case CAIRO_PATH_CLOSE_PATH:
            return 1;
        }

        join_primitives(last_data, &v_old, &p[0], &v_new);

        /* Save the end point of the original primitive in p_old */
        last_data = data + num_points;
        p_old.x = last_data->point.x;
        p_old.y = last_data->point.y;

        /* Store the results got from the p[] vector in the cairo path */
        for (n_point = 1; n_point <= num_points; ++ n_point) {
            data[n_point].point.x = p[n_point].x;
            data[n_point].point.y = p[n_point].y;
        }
    }

    return 1;
}

/**
 * segment_normalize:
 * @segment: a #CpmlSegment
 *
 * Strips the leading CAIRO_PATH_MOVE_TO primitives, updating the CpmlSegment
 * structure accordling. One, and only once, MOVE_TO primitive is left.
 *
 * Return value: 1 on success, 0 on no leading MOVE_TOs or on errors
 **/
static cairo_bool_t
segment_normalize(CpmlSegment *segment)
{
    cairo_path_data_t *data;

    if (segment->source->data->header.type != CAIRO_PATH_MOVE_TO) {
        return 0;
    }

    data = segment->data;

    while (segment->num_data >= 0) {
        data += 2;
        if (data->header.type != CAIRO_PATH_MOVE_TO)
            return 1;

        segment->data = data;
        segment->num_data -= 2;
    }

    return 0;
}

/**
 * line_offset:
 * @p: an array of two #CpmlPair structs
 * @vector: the ending direction vector of the resulting primitive
 * @offset: distance for the computed parallel line
 *
 * Given a line segment starting from @p[0] and ending in @p[1],
 * computes the parallel line distant @offset from the original one
 * and returns the result in the @p array.
 *
 * The direction vector of the new line segment is saved in @vector
 * with a somewhat arbitrary magnitude.
 **/
static void
line_offset(CpmlPair *p, CpmlVector *vector, double offset)
{
    CpmlVector normal;

    cpml_pair_sub(cpml_pair_copy(vector, &p[1]), &p[0]);

    cpml_vector_from_pair(&normal, vector, offset);
    cpml_vector_normal(&normal);

    cpml_pair_add(&p[0], &normal);
    cpml_pair_add(&p[1], &normal);
}

/**
 * curve_offset:
 * @p:      (inout): an array of four #CpmlPair structs
 * @vector: (out):   the ending direction vector of the resulting primitive
 * @offset: (in):    distance for the computed parallel line
 *
 * Given a cubic Bézier segment starting from @p[0] and ending
 * in p[3], with control points in @p[1] and @p[2], this function
 * finds the approximated Bézier curve parallel to the given one
 * at distance @offset (an offset curve).
 *
 * The four points needed to build the new curve are returned
 * in the @p vector. The angular coefficient of the new curve
 * in @p[3] is returned as @vector with @offset magnitude.
 *
 * To solve the offset problem, a custom algorithm is used. First, the
 * resulting curve MUST have the same slope at the start and end point.
 * These constraints are not sufficient to resolve the system, so I let
 * the curve pass thought a given point (pm, known and got from the
 * original curve) at a given time (m, now hardcoded to 0.5).
 *
 * Firstly, I define some useful variables:
 *
 * v0 = unitvector(p[1]-p[0]) * offset;
 * v3 = unitvector(p[3]-p[2]) * offset;
 * p0 = p[0] + normal v0;
 * p3 = p[3] + normal v3.
 *
 * Now I want the curve to have the specified slopes at the start
 * and end point. Forcing the same slope at the start point means:
 *
 * p1 = p0 + k0 v0.
 *
 * where k0 is an arbitrary factor. Decomposing for x and y components:
 *
 * p1.x = p0.x + k0 v0.x;
 * p1.y = p0.y + k0 v0.y.
 *
 * Doing the same for the end point gives:
 *
 * p2.x = p3.x + k3 v3.x;
 * p2.y = p3.y + k3 v3.y.
 *
 * Now I interpolate the curve by forcing it to pass throught pm
 * when "time" is m, where 0 < m < 1. The cubic Bézier function is:
 *
 * C(t) = (1-t)³p0 + 3t(1-t)²p1 + 3t²(1-t)p2 + t³p3.
 *
 * and forcing t=m and C(t) = pm:
 *
 * pm = (1-m)³p0 + 3m(1-m)²p1 + 3m²(1-m)p2 + m³p3.
 *
 * (1-m) p1 + m p2 = (pm - (1-m)³p0 - m³p3) / (3m (1-m)).
 *
 * So the final system is:
 *
 * p1.x = p0.x + k0 v0.x;
 * p1.y = p0.y + k0 v0.y;
 * p2.x = p3.x + k3 v3.x;
 * p2.y = p3.y + k3 v3.y;
 * (1-m) p1.x + m p2.x = (pm.x - (1-m)³p0.x - m³p3.x) / (3m (1-m));
 * (1-m) p1.y + m p2.y = (pm.y - (1-m)³p0.y - m³p3.y) / (3m (1-m)).
 *
 * Substituting and resolving for k0 and k3:
 *
 * (1-m) k0 v0.x + m k3 v3.x =
 *     (pm.x - (1-m)³p0.x - m³p3.x) / (3m (1-m)) - (1-m) p0.x - m p3.x;
 * (1-m) k0 v0.y + m k3 v3.y =
 *     (pm.y - (1-m)³p0.y - m³p3.y) / (3m (1-m)) - (1-m) p0.y - m p3.y.
 *
 * (1-m) k0 v0.x + m k3 v3.x =
 *     (pm.x - (1-m)²(1+2m) p0.x - m²(3-2m) p3.x) / (3m (1-m));
 * (1-m) k0 v0.y + m k3 v3.y =
 *     (pm.y - (1-m)²(1+2m) p0.y - m²(3-2m) p3.y) / (3m (1-m)).
 *
 * Let:
 *
 * pk = (pm - (1-m)²(1+2m) p0 - m²(3-2m) p3) / (3m (1-m)).
 *
 * gives the following system:
 *     
 * (1-m) k0 v0.x + m k3 v3.x = pk.x;
 * (1-m) k0 v0.y + m k3 v3.y = pk.y.
 *
 * Now I should avoid division by 0 troubles. If either v0.x and v3.x
 * are 0, the first equation will be inconsistent. More in general the
 * v0.x*v3.y = v3.x*v3.y condition should be avoided. This is the first
 * case to check, in which case an alternative approach is used. In the
 * other cases the above system can be used.
 *
 * If v0.x != 0 I can resolve for k0 and then find k3:
 *
 * k0 = (pk.x - m k3 v3.x) / ((1-m) v0.x);
 * (pk.x - m k3 v3.x) v0.y / v0.x + m k3 v3.y = pk.y.
 *
 * k0 = (pk.x - m k3 v3.x) / ((1-m) v0.x);
 * k3 m (v3.y - v3.x v0.y / v0.x) = pk.y - pk.x v0.y / v0.x.
 *
 * k3 = (pk.y - pk.x v0.y / v0.x) / (m (v3.y - v3.x v0.y / v0.x));
 * k0 = (pk.x - m k3 v3.x) / ((1-m) v0.x).
 *
 * If v3.x != 0 I can resolve for k3 and then find k0:
 *
 * k3 = (pk.x - (1-m) k0 v0.x) / (m v3.x);
 * (1-m) k0 v0.y + (pk.x - (1-m) k0 v0.x) v3.y / v3.x = pk.y.
 *
 * k3 = (pk.x - (1-m) k0 v0.x) / (m v3.x);
 * k0 (1-m) (v0.y - k0 v0.x v3.y / v3.x) = pk.y - pk.x v3.y / v3.x.
 *
 * k0 = (pk.y - pk.x v3.y / v3.x) / ((1-m) (v0.y - v0.x v3.y / v3.x));
 * k3 = (pk.x - (1-m) k0 v0.x) / (m v3.x).
 **/
static void
curve_offset(CpmlPair *p, CpmlVector *vector, double offset)
{
    double m, mm;
    CpmlVector v0, v3, vm;
    CpmlPair p0, p1, p2, p3, pm;

    /* v0 = p[1]-p[0] */
    cpml_pair_sub(cpml_pair_copy(&v0, &p[1]), &p[0]);

    /* v3 = p[3]-p[2] */
    cpml_pair_sub(cpml_pair_copy(&v3, &p[3]), &p[2]);

    /* p0 = p[0] + normal of v0 of @offset magnitude (the final value of p0) */
    cpml_vector_normal(cpml_vector_from_pair(&p0, &v0, offset));
    cpml_pair_add(&p0, &p[0]);

    /* p3 = p[3] + normal of v3 of @offset magnitude (the final value of p3) */
    cpml_vector_normal(cpml_vector_from_pair(&p3, &v3, offset));
    cpml_pair_add(&p3, &p[3]);

    /* By default, interpolate the new curve by offseting the mid point.
     * @todo: use a better candidate. */
    m = 0.5;

    /* Let be, for simplicity, mm = 1-m */
    mm = 1-m;

    /* pm = point in C(m) offseted the requested @offset distance */
    cpml_vector_at_curve(&vm, &p[0], &p[1], &p[2], &p[3], m, offset);
    cpml_vector_normal(&vm);
    cpml_pair_at_curve(&pm, &p[0], &p[1], &p[2], &p[3], m);
    cpml_pair_add(&pm, &vm);

    if (v0.x*v3.y == v3.x*v0.y) {
        /* Inconsistent equations: use the alternative approach.
         * @todo: this algorithm performs bad if v0 and v3 are opposite
         *        or if are staggered! */
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
    cpml_pair_copy(&p[0], &p0);
    cpml_pair_copy(&p[1], &p1);
    cpml_pair_copy(&p[2], &p2);
    cpml_pair_copy(&p[3], &p3);

    /* Return the ending vector */
    cpml_pair_copy(vector, &v3);
}

/**
 * join_primitives:
 * @last_data: the previous primitive end data point (if any)
 * @last_vector: @last_data direction vector
 * @new_point: the new primitive starting point
 * @new_vector: @new_point direction vector
 *
 * Joins two primitive modifying the end point of the first one (stored
 * as cairo path data in @last_data).
 *
 * @todo: this approach is quite naive when curves are involved.
 **/
static void
join_primitives(cairo_path_data_t *last_data, const CpmlVector *last_vector,
                const CpmlPair *new_point, const CpmlVector *new_vector)
{
    CpmlPair last_point;

    if (last_data == NULL)
        return;

    last_point.x = last_data->point.x;
    last_point.y = last_data->point.y;

    if (cpml_pair_intersection_pv_pv(&last_point,
                                     &last_point, last_vector,
                                     new_point, new_vector)) {
        last_data->point.x = last_point.x;
        last_data->point.y = last_point.y;
    } else {
        last_data->point.x = new_point->x;
        last_data->point.y = new_point->y;
    }
}
