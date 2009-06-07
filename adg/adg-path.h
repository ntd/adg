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


#ifndef __ADG_PATH_H__
#define __ADG_PATH_H__

#include <adg/adg-model.h>
#include <cairo.h>


G_BEGIN_DECLS

#define ADG_TYPE_PATH             (adg_path_get_type ())
#define ADG_PATH(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_PATH, AdgPath))
#define ADG_PATH_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_PATH, AdgPathClass))
#define ADG_IS_PATH(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_PATH))
#define ADG_IS_PATH_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_PATH))
#define ADG_PATH_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_PATH, AdgPathClass))

typedef struct _AdgPath        AdgPath;
typedef struct _AdgPathClass   AdgPathClass;
typedef struct _AdgPathPrivate AdgPathPrivate;

struct _AdgPath {
    /*< private >*/
    AdgModel		 model;
    AdgPathPrivate	*priv;
};

struct _AdgPathClass {
    AdgModelClass	 parent_class;

    /* Virtual table */
    void		(*clear)		(AdgPath        *path);
};


GType           adg_path_get_type               (void) G_GNUC_CONST;
AdgModel *      adg_path_new                    (void);

const cairo_path_t *
                adg_path_get_cairo_path         (AdgPath        *path);
void            adg_path_dump                   (AdgPath        *path);

G_END_DECLS


#endif /* __ADG_PATH_H__ */
