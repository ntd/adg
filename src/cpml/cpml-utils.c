/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:cpml-utils
 * @Section_Id:utilities
 * @title: Utilities
 * @short_description: Assorted macros and functions
 *
 * Collection of macros and functions that do not fit inside any other topic.
 *
 * Since: 1.0
 **/


#include "cpml-internal.h"
#include <math.h>


/**
 * cpml_angle:
 * @angle: an angle in radians
 *
 * Normalizes @angle, that is returns the equivalent radians value
 * between the range <constant>M_PI</constant> (inclusive)
 * and <constant>-M_PI</constant> (exclusive).
 *
 * Returns: an equivalent value in radians
 *
 * Since: 1.0
 **/
double
cpml_angle(double angle)
{
    while (angle > M_PI)
        angle -= M_PI * 2;

    while (angle <= -M_PI)
        angle += M_PI * 2;

    return angle;
}

/**
 * cpml_angle_distance:
 * @angle: first angle in radians
 * @from: second angle in radians
 *
 * Computes the distance between the two given angles. The returned
 * distance is always positive and is never greater than M_PI.
 *
 * Returns: the distance in radians
 *
 * Since: 1.0
 **/
double
cpml_angle_distance(double angle, double from)
{
    double delta = cpml_angle(from - angle);
    return fabs(delta);
}
