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
 * SECTION:curve
 * @title: Bézier curves
 * @short_description: Bézier cubic curve primitive management
 *
 * The following functions manipulate %CAIRO_PATH_CURVE_TO #CpmlPrimitive.
 * No check is made on the primitive struct, so be sure the CpmlPrimitive
 * is effectively a Bézier curve before calling these APIs.
 **/

#include "cpml-curve.h"
#include "cpml-pair.h"

/**
 * cpml_curve_get_npoints:
 *
 * Returns the number of point needed to properly specify a curve primitive.
 *
 * Return value: 4
 **/
int
cpml_curve_get_npoints(void)
{
    return 4;
}

/**
 * cpml_curve_offset:
 * @curve:  the #CpmlPrimitive curve data
 * @offset: distance for the computed parallel curve
 *
 * Given a cubic Bézier primitive in @curve, this function finds
 * the approximated Bézier curve parallel to @curve at distance
 * @offset (an offset curve). The four points needed to build the
 * new curve are returned in the @curve struct.
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
void
cpml_curve_offset(CpmlPrimitive *curve, double offset)
{
    double m, mm;
    CpmlVector v0, v3, vm, vtmp;
    CpmlPair p0, p1, p2, p3, pm;

    /* By default, interpolate the new curve by offseting the mid point.
     * TODO: use a better candidate. */
    m = 0.5;

    /* Let be, for simplicity, mm = 1-m */
    mm = 1-m;

    /* Firstly, convert the curve points from cairo format to cpml format
     * and store them (temporary) in p0..p3 */
    cpml_pair_from_cairo(&p0, curve->org);
    cpml_pair_from_cairo(&p1, &curve->data[1]);
    cpml_pair_from_cairo(&p2, &curve->data[2]);
    cpml_pair_from_cairo(&p3, &curve->data[3]);

    /* v0 = p1-p0 */
    cpml_pair_sub(cpml_pair_copy(&v0, &p1), &p0);

    /* v3 = p3-p2 */
    cpml_pair_sub(cpml_pair_copy(&v3, &p3), &p2);

    /* pm = point in C(m) offseted the requested @offset distance */
    cpml_vector_at_curve(&vm, &p0, &p1, &p2, &p3, m, offset);
    cpml_vector_normal(&vm);
    cpml_pair_at_curve(&pm, &p0, &p1, &p2, &p3, m);
    cpml_pair_add(&pm, &vm);

    /* p0 = p0 + normal of v0 of @offset magnitude (exact value) */
    cpml_vector_normal(cpml_vector_from_pair(&vtmp, &v0, offset));
    cpml_pair_add(&p0, &vtmp);

    /* p3 = p3 + normal of v3 of @offset magnitude, as done for p0 */
    cpml_vector_normal(cpml_vector_from_pair(&vtmp, &v3, offset));
    cpml_pair_add(&p3, &vtmp);

    if (v0.x*v3.y == v3.x*v0.y) {
        /* Inconsistent equations: use the alternative approach.
         * TODO: this algorithm performs bad if v0 and v3 are opposite
         *       or if are staggered! */
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
    cpml_pair_to_cairo(&p0, curve->org);
    cpml_pair_to_cairo(&p1, &curve->data[1]);
    cpml_pair_to_cairo(&p2, &curve->data[2]);
    cpml_pair_to_cairo(&p3, &curve->data[3]);
}
