/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-enums
 * @Section_Id:enumerations
 * @title: Standard enumerations
 * @short_description: Public enumerated types used throughout ADG
 *
 * All the enumerations are available in GType notation, and so are
 * the standard type macros.
 **/


/**
 * AdgThreeState:
 * @ADG_THREE_STATE_OFF: disabled, switched off, disactive
 * @ADG_THREE_STATE_ON: enabled, switched on, active
 * @ADG_THREE_STATE_UNKNOWN: unknown / indefinite / automatic state
 *
 * A generic three state flags to be used whenever needed. Could be
 * user when a boolean is requested but also an indefinite / default
 * state should be caught.
 **/

/**
 * AdgTransformMode:
 * @ADG_TRANSFORM_NONE: do not apply any transformation
 * @ADG_TRANSFORM_BEFORE: apply the transformation before the matrix using
 *                        cairo_matrix_multiply(matrix, matrix, transformation)
 * @ADG_TRANSFORM_AFTER: apply the transformation after the matrix using
 *                       cairo_matrix_multiply(matrix, transformation, matrix)
 * @ADG_TRANSFORM_BEFORE_NORMALIZED: same as %ADG_TRANSFORM_BEFORE but
 *                                   normalizing the transformation with
 *                                   adg_matrix_normalize() before applying it
 * @ADG_TRANSFORM_AFTER_NORMALIZED: same as %ADG_TRANSFORM_AFTER but
 *                                  normalizing the transformation with
 *                                  adg_matrix_normalize() before applying it
 *
 * Specifies the mode a generic transformation should be applied on
 * a subject matrix. Although used in different places, the function
 * performing the dirty work is always adg_matrix_transform().
 **/
