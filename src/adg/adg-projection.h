/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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


#if !defined(__ADG_H__)
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_PROJECTION_H__
#define __ADG_PROJECTION_H__


G_BEGIN_DECLS

#define ADG_TYPE_PROJECTION             (adg_projection_get_type())
#define ADG_PROJECTION(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_PROJECTION, AdgProjection))
#define ADG_PROJECTION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_PROJECTION, AdgProjectionClass))
#define ADG_IS_PROJECTION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_PROJECTION))
#define ADG_IS_PROJECTION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_PROJECTION))
#define ADG_PROJECTION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_PROJECTION, AdgProjectionClass))

typedef struct _AdgProjection        AdgProjection;
typedef struct _AdgProjectionClass   AdgProjectionClass;

struct _AdgProjection {
    /*< private >*/
    AdgEntity           parent;
    gpointer            data;
};

struct _AdgProjectionClass {
    /*< private >*/
    AdgEntityClass      parent_class;
    gpointer            data_class;
};


GType           adg_projection_get_type         (void) G_GNUC_CONST;

AdgProjection * adg_projection_new              (AdgProjectionScheme scheme);

void            adg_projection_set_symbol_dress (AdgProjection  *projection,
                                                 AdgDress        dress);
AdgDress        adg_projection_get_symbol_dress (AdgProjection  *projection);
void            adg_projection_set_axis_dress   (AdgProjection  *projection,
                                                 AdgDress        dress);
AdgDress        adg_projection_get_axis_dress   (AdgProjection  *projection);
void            adg_projection_set_scheme       (AdgProjection  *projection,
                                                 AdgProjectionScheme scheme);
AdgProjectionScheme
                adg_projection_get_scheme       (AdgProjection  *projection);

G_END_DECLS


#endif /* __ADG_PROJECTION_H__ */
