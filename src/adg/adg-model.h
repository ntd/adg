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


#ifndef __ADG_MODEL_H__
#define __ADG_MODEL_H__


G_BEGIN_DECLS

#define ADG_TYPE_MODEL             (adg_model_get_type())
#define ADG_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_MODEL, AdgModel))
#define ADG_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_MODEL, AdgModel))
#define ADG_IS_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_MODEL))
#define ADG_IS_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_MODEL))
#define ADG_MODEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_MODEL, AdgModelClass))


typedef struct _AdgModel        AdgModel;
typedef struct _AdgModelClass   AdgModelClass;
typedef void    (*AdgDependencyFunc)            (AdgModel       *model,
                                                 AdgEntity      *entity,
                                                 gpointer        user_data);
typedef void    (*AdgNamedPairFunc)             (AdgModel       *model,
                                                 const gchar    *name,
                                                 AdgPair        *pair,
                                                 gpointer        user_data);

struct _AdgModel {
    /*< private >*/
    GObject              parent;
    gpointer             data;
};

struct _AdgModelClass {
    /*< private >*/
    GObjectClass         parent_class;

    /*< public >*/
    /* Virtual table */
    const AdgPair *     (*named_pair)           (AdgModel         *model,
                                                 const gchar      *name);

    /* Signals */
    void                (*set_named_pair)       (AdgModel         *model,
                                                 const gchar      *name,
                                                 const AdgPair    *pair);
    void                (*clear)                (AdgModel         *model);
    void                (*reset)                (AdgModel         *model);
    void                (*add_dependency)       (AdgModel         *model,
                                                 AdgEntity        *entity);
    void                (*remove_dependency)    (AdgModel         *model,
                                                 AdgEntity        *entity);
    void                (*changed)              (AdgModel         *model);
};


GType           adg_model_get_type              (void) G_GNUC_CONST;

void            adg_model_add_dependency        (AdgModel         *model,
                                                 AdgEntity        *entity);
void            adg_model_remove_dependency     (AdgModel         *model,
                                                 AdgEntity        *entity);
const GSList *  adg_model_get_dependencies      (AdgModel         *model);
void            adg_model_foreach_dependency    (AdgModel         *model,
                                                 AdgDependencyFunc callback,
                                                 gpointer          user_data);
void            adg_model_set_named_pair        (AdgModel         *model,
                                                 const gchar      *name,
                                                 const AdgPair    *pair);
void            adg_model_set_named_pair_explicit
                                                (AdgModel         *model,
                                                 const gchar      *name,
                                                 gdouble           x,
                                                 gdouble           y);
const AdgPair * adg_model_get_named_pair        (AdgModel         *model,
                                                 const gchar      *name);
void            adg_model_foreach_named_pair    (AdgModel         *model,
                                                 AdgNamedPairFunc  callback,
                                                 gpointer          user_data);
void            adg_model_clear                 (AdgModel         *model);
void            adg_model_reset                 (AdgModel         *model);
void            adg_model_changed               (AdgModel         *model);

G_END_DECLS


#endif /* __ADG_MODEL_H__ */
