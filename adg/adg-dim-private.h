/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADGDIM_PRIVATE_H__
#define __ADGDIM_PRIVATE_H__

#include <adg/adg-dim.h>
#include <adg/adg-ldim.h>


G_BEGIN_DECLS

#define _ADG_DIM_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ADG_TYPE_DIM, _AdgDimPrivate))
#define _ADG_LDIM_GET_PRIVATE(obj)   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ADG_TYPE_LDIM, _AdgLDimPrivate))
#define _ADG_ADIM_GET_PRIVATE(obj)   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ADG_TYPE_ADIM, _AdgADimPrivate))


typedef struct __AdgDimPrivate   _AdgDimPrivate;
typedef struct __AdgLDimPrivate  _AdgLDimPrivate;
typedef struct __AdgADimPrivate  _AdgADimPrivate;


struct __AdgDimPrivate
{
  /* Device and user matrix dependent */
  AdgPair                quote_org;

  /* Only device matrix dependent */
  double                 quote_angle;
  AdgPair                quote_offset;
  AdgPair                tolerance_up_offset;
  AdgPair                tolerance_down_offset;
  AdgPair                note_offset;
};

struct __AdgLDimPrivate
{
  cairo_path_t           extension1;
  cairo_path_t           extension2;
  cairo_path_t           arrow_path;
  cairo_path_t           baseline;
};

struct __AdgADimPrivate
{
  cairo_path_t           extension1;
  cairo_path_t           extension2;
  cairo_path_t           arrow_path;
  cairo_path_t           baseline;
};


void            _adg_dim_render_quote           (AdgDim         *dim,
                                                 cairo_t        *cr);

G_END_DECLS


#endif /* __ADGDIM_PRIVATE_H__ */
