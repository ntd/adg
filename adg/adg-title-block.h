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


#ifndef __ADG_TITLE_BLOCK_H__
#define __ADG_TITLE_BLOCK_H__

#include <adg/adg-entity.h>


G_BEGIN_DECLS

#define ADG_TYPE_TITLE_BLOCK             (adg_title_block_get_type ())
#define ADG_TITLE_BLOCK(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_TITLE_BLOCK, AdgTitleBlock))
#define ADG_TITLE_BLOCK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_TITLE_BLOCK, AdgTitleBlock))
#define ADG_IS_TITLE_BLOCK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_TITLE_BLOCK))
#define ADG_IS_TITLE_BLOCK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_TITLE_BLOCK))
#define ADG_TITLE_BLOCK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_TITLE_BLOCK, AdgTitleBlock))

typedef struct _AdgTitleBlock        AdgTitleBlock;
typedef struct _AdgTitleBlockClass   AdgTitleBlockClass;
typedef struct _AdgTitleBlockPrivate AdgTitleBlockPrivate;

struct _AdgTitleBlock {
    /*< private >*/
    AdgEntity             entity;
    AdgTitleBlockPrivate *priv;
};

struct _AdgTitleBlockClass {
    AdgEntityClass        parent_class;
};


GType           adg_title_block_get_type        (void) G_GNUC_CONST;

gchar *         adg_title_block_get_name        (AdgTitleBlock  *title_block);
void            adg_title_block_set_name        (AdgTitleBlock  *title_block,
                                                 const gchar    *name);
gchar *         adg_title_block_get_material    (AdgTitleBlock  *title_block);
void            adg_title_block_set_material    (AdgTitleBlock  *title_block,
                                                 const gchar    *material);
gchar *         adg_title_block_get_treatment   (AdgTitleBlock  *title_block);
void            adg_title_block_set_treatment   (AdgTitleBlock  *title_block,
                                                 const gchar    *treatment);

G_END_DECLS


#endif /* __ADG_TITLE_BLOCK_H__ */
