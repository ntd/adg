/* ADG - Automatic Drawing Generation.
 * Copyright (C) 2007 - Fontana Nicola <ntd@users.sourceforge.net>
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


#ifndef __ADGADIM_H__
#define __ADGADIM_H__

#include <adg/adgdim.h>


G_BEGIN_DECLS

#define ADG_TYPE_ADIM             (adg_adim_get_type ())
#define ADG_ADIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_ADIM, AdgADim))
#define ADG_ADIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_ADIM, AdgADimClass))
#define ADG_IS_ADIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_ADIM))
#define ADG_IS_ADIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_ADIM))
#define ADG_ADIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_ADIM, AdgADimClass))


typedef struct _AdgADim       AdgADim;
typedef struct _AdgADimClass  AdgADimClass;

struct _AdgADim
{
  AdgDim                 dim;

  /*< private >*/
};

struct _AdgADimClass
{
  AdgDimClass            parent_class;
};


GType           adg_adim_get_type               (void) G_GNUC_CONST;

AdgEntity *     adg_adim_new                    (void);

G_END_DECLS


#endif /* __ADGADIM_H__ */
