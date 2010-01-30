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


#if !defined (__ADG_H__)
#error "Only <adg/adg.h> can be included directly."
#endif


#ifndef __ADG_TOY_TEXT_H__
#define __ADG_TOY_TEXT_H__

#include <adg/adg-entity.h>
#include <adg/adg-pair.h>


G_BEGIN_DECLS

#define ADG_TYPE_TOY_TEXT             (adg_toy_text_get_type())
#define ADG_TOY_TEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TOY_TEXT, AdgToyText))
#define ADG_TOY_TEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_TOY_TEXT, AdgToyTextClass))
#define ADG_IS_TOY_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TOY_TEXT))
#define ADG_IS_TOY_TEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_TOY_TEXT))
#define ADG_TOY_TEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_TOY_TEXT, AdgToyTextClass))

typedef struct _AdgToyText        AdgToyText;
typedef struct _AdgToyTextClass   AdgToyTextClass;

struct _AdgToyText {
    /*< private >*/
    AdgEntity            parent;
    gpointer             data;
};

struct _AdgToyTextClass {
    /*< private >*/
    AdgEntityClass       parent_class;
};


GType           adg_toy_text_get_type           (void) G_GNUC_CONST;
AdgToyText *    adg_toy_text_new                (const gchar    *label);
void            adg_toy_text_set_font_dress     (AdgToyText     *toy_text,
                                                 AdgDress        dress);
AdgDress        adg_toy_text_get_font_dress     (AdgToyText     *toy_text);
void            adg_toy_text_set_label          (AdgToyText     *toy_text,
                                                 const gchar    *label);
const gchar *   adg_toy_text_get_label          (AdgToyText     *toy_text);

G_END_DECLS


#endif /* __ADG_TOY_TEXT_H__ */
