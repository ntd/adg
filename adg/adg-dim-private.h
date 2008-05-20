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


#ifndef __ADGDIMPRIVATE_H__
#define __ADGDIMPRIVATE_H__

#include <adg/adg-style.h>
#include <adg/adg-pair.h>


G_BEGIN_DECLS


struct _AdgDimPrivate
{
  AdgDimStyle		*dim_style;
  AdgPair		 ref1;
  AdgPair		 ref2;
  AdgPair		 pos1;
  AdgPair		 pos2;
  double		 level;
  gchar			*label;
  gchar			*tolerance_up;
  gchar			*tolerance_down;
  gchar			*note;

  /* Cached data */
  AdgPair		 quote_org;
  gdouble		 quote_angle;

  /* Cached data (quote related) */
  AdgPair		 quote_offset;
  AdgPair		 tolerance_up_offset;
  AdgPair		 tolerance_down_offset;
  AdgPair 		 note_offset;
};


void		_adg_dim_render_quote		(AdgDim		*dim,
						 cairo_t	*cr);

G_END_DECLS


#endif /* __ADGDIMPRIVATE_H__ */
