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


#ifndef __ADG_PATH_H__
#define __ADG_PATH_H__


G_BEGIN_DECLS

#define ADG_TYPE_PATH             (adg_path_get_type())
#define ADG_PATH(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_PATH, AdgPath))
#define ADG_PATH_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_PATH, AdgPathClass))
#define ADG_IS_PATH(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_PATH))
#define ADG_IS_PATH_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_PATH))
#define ADG_PATH_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_PATH, AdgPathClass))

typedef struct _AdgPath        AdgPath;
typedef struct _AdgPathClass   AdgPathClass;

struct _AdgPath {
    /*< private >*/
    AdgTrail             parent;
    gpointer             data;
};

struct _AdgPathClass {
    /*< private >*/
    AdgTrailClass        parent_class;
};


GType           adg_path_get_type               (void) G_GNUC_CONST;
AdgPath *       adg_path_new                    (void);

const AdgPair * adg_path_get_current_point      (AdgPath        *path);
gboolean        adg_path_has_current_point      (AdgPath        *path);
const AdgPrimitive *
                adg_path_last_primitive         (AdgPath        *path);
const AdgPrimitive *
                adg_path_over_primitive         (AdgPath        *path);

void            adg_path_append                 (AdgPath        *path,
                                                 CpmlPrimitiveType type,
                                                 ...);
void            adg_path_append_valist          (AdgPath        *path,
                                                 CpmlPrimitiveType type,
                                                 va_list         var_args);
void            adg_path_append_array           (AdgPath        *path,
                                                 CpmlPrimitiveType type,
                                                 const AdgPair **pairs);
void            adg_path_append_primitive       (AdgPath        *path,
                                                 const AdgPrimitive
                                                                *primitive);
void            adg_path_append_segment         (AdgPath        *path,
                                                 const AdgSegment *segment);
void            adg_path_append_cpml_path       (AdgPath        *path,
                                                 const CpmlPath *cpml_path);
void            adg_path_move_to                (AdgPath        *path,
                                                 const AdgPair  *pair);
void            adg_path_move_to_explicit       (AdgPath        *path,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_path_line_to                (AdgPath        *path,
                                                 const AdgPair  *pair);
void            adg_path_line_to_explicit       (AdgPath        *path,
                                                 gdouble         x,
                                                 gdouble         y);
void            adg_path_arc_to                 (AdgPath        *path,
                                                 const AdgPair  *throught,
                                                 const AdgPair  *pair);
void            adg_path_arc_to_explicit        (AdgPath        *path,
                                                 gdouble         x1,
                                                 gdouble         y1,
                                                 gdouble         x2,
                                                 gdouble         y2);
void            adg_path_curve_to               (AdgPath        *path,
                                                 const AdgPair  *control1,
                                                 const AdgPair  *control2,
                                                 const AdgPair  *pair);
void            adg_path_curve_to_explicit      (AdgPath        *path,
                                                 gdouble         x1,
                                                 gdouble         y1,
                                                 gdouble         x2,
                                                 gdouble         y2,
                                                 gdouble         x3,
                                                 gdouble         y3);
void            adg_path_close                  (AdgPath        *path);

void            adg_path_arc                    (AdgPath        *path,
                                                 const AdgPair  *center,
                                                 gdouble         r,
                                                 gdouble         start,
                                                 gdouble         end);
void            adg_path_arc_explicit           (AdgPath        *path,
                                                 gdouble         xc,
                                                 gdouble         yc,
                                                 gdouble         r,
                                                 gdouble         start,
                                                 gdouble         end);
void            adg_path_chamfer                (AdgPath        *path,
                                                 gdouble         delta1,
                                                 gdouble         delta2);
void            adg_path_fillet                 (AdgPath        *path,
                                                 gdouble         radius);
void            adg_path_reflect                (AdgPath        *path,
                                                 const CpmlVector *vector);
void            adg_path_reflect_explicit       (AdgPath        *path,
                                                 gdouble         x,
                                                 gdouble         y);

G_END_DECLS


#endif /* __ADG_PATH_H__ */
