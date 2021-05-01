/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_EDGES_H__
#define __ADG_EDGES_H__


G_BEGIN_DECLS

#define ADG_TYPE_EDGES             (adg_edges_get_type())
#define ADG_EDGES(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_EDGES, AdgEdges))
#define ADG_EDGES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_EDGES, AdgEdgesClass))
#define ADG_IS_EDGES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_EDGES))
#define ADG_IS_EDGES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_EDGES))
#define ADG_EDGES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_EDGES, AdgEdgesClass))


typedef struct _AdgEdges        AdgEdges;
typedef struct _AdgEdgesClass   AdgEdgesClass;

struct _AdgEdges {
    /*< private >*/
    AdgTrail        parent;
};

struct _AdgEdgesClass {
    /*< private >*/
    AdgTrailClass   parent_class;
};


GType           adg_edges_get_type              (void);
AdgEdges *      adg_edges_new                   (void);
AdgEdges *      adg_edges_new_with_source       (AdgTrail       *source);

void            adg_edges_set_source            (AdgEdges       *edges,
                                                 AdgTrail       *source);
AdgTrail *      adg_edges_get_source            (AdgEdges       *edges);
void            adg_edges_set_axis_angle        (AdgEdges       *edges,
                                                 gdouble         angle);
gdouble         adg_edges_get_axis_angle        (AdgEdges       *edges);
void            adg_edges_set_critical_angle    (AdgEdges       *edges,
                                                 gdouble         angle);
gdouble         adg_edges_get_critical_angle    (AdgEdges       *edges);

G_END_DECLS


#endif /* __ADG_EDGES_H__ */
