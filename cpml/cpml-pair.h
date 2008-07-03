/* CPML - Cairo Path Manipulation Library
 * Copyright (C) 2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */

#ifndef __CPML_PAIR_H__
#define __CPML_PAIR_H__

#include <cairo.h>


CAIRO_BEGIN_DECLS


typedef struct _CpmlPair	CpmlPair;

struct _CpmlPair {
	double			x, y;
};


cairo_bool_t
cpml_pair_copy			(CpmlPair		*pair,
				 const CpmlPair		*src);
cairo_bool_t
cpml_pair_distance		(const CpmlPair		*from,
				 const CpmlPair		*to,
				 double			*distance);
cairo_bool_t
cpml_pair_square_distance	(const CpmlPair		*from,
				 const CpmlPair		*to,
				 double			*distance);


CAIRO_END_DECLS


#endif /* __CPML_PAIR_H__ */
