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


#ifndef ADG_PATH_PRIVATE_H
#define ADG_PATH_PRIVATE_H

#include "adgpath.h"

#define ADG_TOLERANCE   0.1


G_BEGIN_DECLS

typedef enum _AdgDirection  AdgDirection;

enum _AdgDirection
{
  ADG_DIRECTION_FORWARD,
  ADG_DIRECTION_REVERSE
};


void    _adg_path_arc                   (AdgPath *path,
                                         double	  xc,
                                         double	  yc,
                                         double	  radius,
                                         double	  angle1,
                                         double	  angle2);

void    _adg_path_arc_negative          (AdgPath *path,
                                         double   xc,
                                         double   yc,
                                         double   radius,
                                         double   angle1,
                                         double   angle2);

G_END_DECLS

#endif /* ADG_PATH_PRIVATE_H */
