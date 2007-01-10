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


#ifndef __ADGENTITY_H__
#define __ADGENTITY_H__

#include <adg/adgpair.h>
#include <adg/adgmatrix.h>
#include <adg/adgstyle.h>
#include <gcontainer/gcontainer.h>


G_BEGIN_DECLS

/* Forward declarations */

typedef struct _AdgCanvas       AdgCanvas;


#define ADG_TYPE_ENTITY             (adg_entity_get_type ())
#define ADG_ENTITY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_ENTITY, AdgEntity))
#define ADG_ENTITY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_ENTITY, AdgEntityClass))
#define ADG_IS_ENTITY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_ENTITY))
#define ADG_IS_ENTITY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_ENTITY))
#define ADG_ENTITY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_ENTITY, AdgEntityClass))


/* AdgEntity flags management. */

typedef enum
{
  ADG_UPTODATE	       = 1 << 0
} AdgEntityFlags;

#define ADG_ENTITY_FLAGS(ent)        (ADG_ENTITY (ent)->flags)
#define ADG_ENTITY_UPTODATE(ent)     ((ADG_ENTITY_FLAGS (ent) & (ADG_UPTODATE)) != 0)

#define ADG_ENTITY_SET_FLAGS(ent,flag) \
  G_STMT_START{ (ADG_ENTITY_FLAGS (ent) |= (flag)); }G_STMT_END
#define ADG_ENTITY_UNSET_FLAGS(ent,flag) \
  G_STMT_START{ (ADG_ENTITY_FLAGS (ent) &= ~(flag)); }G_STMT_END

#define ADG_CALLBACK(f)              ((AdgCallback) (f))


typedef struct _AdgEntity       AdgEntity;
typedef struct _AdgEntityClass  AdgEntityClass;
typedef void (*AdgCallback) (AdgEntity *entity, gpointer user_data);


struct _AdgEntity
{
  GInitiallyUnowned	 child;

  /*< private >*/

  GContainerable        *parent;

  guint32                flags;
};

struct _AdgEntityClass
{
  GInitiallyUnownedClass parent_class;

  /* Signals */

  void                  (*uptodate_set)                 (AdgEntity      *entity,
                                                         gboolean        old_state);
  void                  (*ctm_changed)                  (AdgEntity      *entity,
                                                         AdgMatrix      *old_ctm);

  /* Virtual Table */

  const AdgLineStyle *  (*get_line_style)               (AdgEntity      *entity);
  void                  (*set_line_style)               (AdgEntity      *entity,
                                                         AdgLineStyle   *line_style);
  const AdgFontStyle *  (*get_font_style)               (AdgEntity      *entity);
  void                  (*set_font_style)               (AdgEntity      *entity,
                                                         AdgFontStyle   *font_style);
  const AdgArrowStyle * (*get_arrow_style)              (AdgEntity      *entity);
  void                  (*set_arrow_style)              (AdgEntity      *entity,
                                                         AdgArrowStyle  *arrow_style);
  const AdgDimStyle *   (*get_dim_style)                (AdgEntity      *entity);
  void                  (*set_dim_style)                (AdgEntity      *entity,
                                                         AdgDimStyle    *dim_style);

  const AdgMatrix *     (*get_ctm)                      (AdgEntity      *entity);

  void                  (*update)                       (AdgEntity      *entity,
                                                         gboolean        recursive);
  void                  (*outdate)                      (AdgEntity      *entity,
                                                         gboolean        recursive);
  void                  (*render)                       (AdgEntity      *entity,
                                                         cairo_t        *cr);
};


GType			adg_entity_get_type	        (void) G_GNUC_CONST;

AdgCanvas *             adg_entity_get_canvas           (AdgEntity      *entity);

void                    adg_entity_ctm_changed          (AdgEntity      *entity);
const AdgMatrix *       adg_entity_get_ctm              (AdgEntity      *entity);

const AdgLineStyle *    adg_entity_get_line_style       (AdgEntity      *entity);
void                    adg_entity_set_line_style       (AdgEntity      *entity,
                                                         AdgLineStyle   *line_style);
const AdgFontStyle *    adg_entity_get_font_style       (AdgEntity      *entity);
void                    adg_entity_set_font_style       (AdgEntity      *entity,
                                                         AdgFontStyle   *font_style);
const AdgArrowStyle *   adg_entity_get_arrow_style      (AdgEntity      *entity);
void                    adg_entity_set_arrow_style      (AdgEntity      *entity,
                                                         AdgArrowStyle  *arrow_style);
const AdgDimStyle *     adg_entity_get_dim_style        (AdgEntity      *entity);
void                    adg_entity_set_dim_style        (AdgEntity      *entity,
                                                         AdgDimStyle    *dim_style);

void                    adg_entity_update               (AdgEntity      *entity);
void                    adg_entity_update_all           (AdgEntity      *entity);
void                    adg_entity_outdate              (AdgEntity      *entity);
void                    adg_entity_outdate_all          (AdgEntity      *entity);
void                    adg_entity_render               (AdgEntity      *entity,
                                                         cairo_t        *cr);

G_END_DECLS


#endif /* __ADGENTITY_H__ */
