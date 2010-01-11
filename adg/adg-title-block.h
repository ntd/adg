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

#include <adg/adg-table.h>


G_BEGIN_DECLS

#define ADG_TYPE_TITLE_BLOCK             (adg_title_block_get_type())
#define ADG_TITLE_BLOCK(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TITLE_BLOCK, AdgTitleBlock))
#define ADG_TITLE_BLOCK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_TITLE_BLOCK, AdgTitleBlockClass))
#define ADG_IS_TITLE_BLOCK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TITLE_BLOCK))
#define ADG_IS_TITLE_BLOCK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_TITLE_BLOCK))
#define ADG_TITLE_BLOCK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_TITLE_BLOCK, AdgTitleBlockClass))

typedef struct _AdgTitleBlock        AdgTitleBlock;
typedef struct _AdgTitleBlockClass   AdgTitleBlockClass;

struct _AdgTitleBlock {
    /*< private >*/
    AdgTable             parent;
    gpointer             data;
};

struct _AdgTitleBlockClass {
    /*< private >*/
    AdgTableClass        parent_class;
};


GType           adg_title_block_get_type        (void) G_GNUC_CONST;

AdgTitleBlock * adg_title_block_new             (void);
void            adg_title_block_set_logo        (AdgTitleBlock  *title_block,
                                                 AdgEntity      *logo);
AdgEntity *     adg_title_block_logo            (AdgTitleBlock  *title_block);
void            adg_title_block_set_title       (AdgTitleBlock  *title_block,
                                                 const gchar    *title);
const gchar *   adg_title_block_get_title       (AdgTitleBlock  *title_block);
void            adg_title_block_set_drawing     (AdgTitleBlock  *title_block,
                                                 const gchar    *drawing);
const gchar *   adg_title_block_get_drawing     (AdgTitleBlock  *title_block);
void            adg_title_block_set_size        (AdgTitleBlock  *title_block,
                                                 const gchar    *size);
const gchar *   adg_title_block_get_size        (AdgTitleBlock  *title_block);
void            adg_title_block_set_scale       (AdgTitleBlock  *title_block,
                                                 const gchar    *scale);
const gchar *   adg_title_block_get_scale       (AdgTitleBlock  *title_block);
void            adg_title_block_set_author      (AdgTitleBlock  *title_block,
                                                 const gchar    *author);
const gchar *   adg_title_block_get_author      (AdgTitleBlock  *title_block);
void            adg_title_block_set_date        (AdgTitleBlock  *title_block,
                                                 const gchar    *date);
const gchar *   adg_title_block_get_date        (AdgTitleBlock  *title_block);
void            adg_title_block_set_projection  (AdgTitleBlock  *title_block,
                                                 AdgEntity      *projection);
AdgEntity *     adg_title_block_projection      (AdgTitleBlock  *title_block);

G_END_DECLS


#endif /* __ADG_TITLE_BLOCK_H__ */
