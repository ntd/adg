/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_TEXT_H__
#define __ADG_TEXT_H__


G_BEGIN_DECLS

#define ADG_TYPE_TEXT             (adg_text_get_type())
#define ADG_TEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TEXT, AdgText))
#define ADG_TEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_TEXT, AdgTextClass))
#define ADG_IS_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TEXT))
#define ADG_IS_TEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_TEXT))
#define ADG_TEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_TEXT, AdgTextClass))

typedef struct _AdgText        AdgText;
typedef struct _AdgTextClass   AdgTextClass;

struct _AdgText {
    /*< private >*/
    AdgEntity           parent;
};

struct _AdgTextClass {
    /*< private >*/
    AdgEntityClass      parent_class;
};


GType           adg_text_get_type               (void);
AdgText *       adg_text_new                    (const gchar    *text);

G_END_DECLS


#endif /* __ADG_TEXT_H__ */
