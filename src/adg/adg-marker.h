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


#ifndef __ADG_MARKER_H__
#define __ADG_MARKER_H__


G_BEGIN_DECLS

#define ADG_TYPE_MARKER             (adg_marker_get_type())
#define ADG_MARKER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_MARKER, AdgMarker))
#define ADG_MARKER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_MARKER, AdgMarkerClass))
#define ADG_IS_MARKER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_MARKER))
#define ADG_IS_MARKER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_MARKER))
#define ADG_MARKER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_MARKER, AdgMarkerClass))

typedef struct _AdgMarker       AdgMarker;
typedef struct _AdgMarkerClass  AdgMarkerClass;

struct _AdgMarker {
    /*< private >*/
    AdgEntity           parent;
};

struct _AdgMarkerClass {
    /*< private >*/
    AdgEntityClass      parent_class;

    /*< public >*/
    /* Virtual table */
    AdgModel *          (*create_model)         (AdgMarker      *marker);
};


GType               adg_marker_get_type         (void);

void                adg_marker_set_trail        (AdgMarker      *marker,
                                                 AdgTrail       *trail);
AdgTrail *          adg_marker_get_trail        (AdgMarker      *marker);
void                adg_marker_set_n_segment    (AdgMarker      *marker,
                                                 guint           n_segment);
guint               adg_marker_get_n_segment    (AdgMarker      *marker);
void                adg_marker_set_segment      (AdgMarker      *marker,
                                                 AdgTrail       *trail,
                                                 guint           n_segment);
const CpmlSegment * adg_marker_get_segment      (AdgMarker      *marker);
void                adg_marker_backup_segment   (AdgMarker      *marker);
const CpmlSegment * adg_marker_get_backup_segment
                                                (AdgMarker      *marker);
void                adg_marker_set_pos          (AdgMarker      *marker,
                                                 gdouble         pos);
gdouble             adg_marker_get_pos          (AdgMarker      *marker);
void                adg_marker_set_size         (AdgMarker      *marker,
                                                 gdouble         size);
gdouble             adg_marker_get_size         (AdgMarker      *marker);
void                adg_marker_set_model        (AdgMarker      *marker,
                                                 AdgModel       *model);
AdgModel *          adg_marker_get_model        (AdgMarker      *marker);
AdgModel *          adg_marker_model            (AdgMarker      *marker);

G_END_DECLS


#endif /* __ADG_MARKER_H__ */
