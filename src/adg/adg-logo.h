/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_LOGO_H__
#define __ADG_LOGO_H__


G_BEGIN_DECLS

#define ADG_TYPE_LOGO             (adg_logo_get_type())
#define ADG_LOGO(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_LOGO, AdgLogo))
#define ADG_LOGO_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_LOGO, AdgLogoClass))
#define ADG_IS_LOGO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_LOGO))
#define ADG_IS_LOGO_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_LOGO))
#define ADG_LOGO_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_LOGO, AdgLogoClass))

typedef struct _AdgLogo        AdgLogo;
typedef struct _AdgLogoClass   AdgLogoClass;

struct _AdgLogo {
    /*< private >*/
    AdgEntity           parent;
    gpointer            data;
};

struct _AdgLogoClass {
    /*< private >*/
    AdgEntityClass      parent_class;
    gpointer            data_class;
};


GType           adg_logo_get_type             (void);

AdgLogo *       adg_logo_new                  (void);

void            adg_logo_set_symbol_dress       (AdgLogo        *logo,
                                                 AdgDress        dress);
AdgDress        adg_logo_get_symbol_dress       (AdgLogo        *logo);
void            adg_logo_set_screen_dress       (AdgLogo        *logo,
                                                 AdgDress        dress);
AdgDress        adg_logo_get_screen_dress       (AdgLogo        *logo);
void            adg_logo_set_frame_dress        (AdgLogo        *logo,
                                                 AdgDress        dress);
AdgDress        adg_logo_get_frame_dress        (AdgLogo        *logo);

G_END_DECLS


#endif /* __ADG_LOGO_H__ */
