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


#ifndef __ADG_TEXTUAL_H__
#define __ADG_TEXTUAL_H__


G_BEGIN_DECLS

#define ADG_TYPE_TEXTUAL            (adg_textual_get_type())
#define ADG_TEXTUAL(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_TEXTUAL, AdgTextual))
#define ADG_IS_TEXTUAL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_TEXTUAL))
#define ADG_TEXTUAL_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE((obj), ADG_TYPE_TEXTUAL, AdgTextualIface))


typedef struct _AdgTextual       AdgTextual;
typedef struct _AdgTextualIface  AdgTextualIface;

struct _AdgTextualIface {
    /*< private >*/
    GTypeInterface base_iface;

    /*< public >*/
    /* Virtual table */
    void                (*set_font_dress)       (AdgTextual     *textual,
                                                 AdgDress        dress);
    AdgDress            (*get_font_dress)       (AdgTextual     *textual);
    void                (*set_text)             (AdgTextual     *textual,
                                                 const gchar    *text);
    gchar *             (*dup_text)             (AdgTextual     *textual);

    /* Signals */
    void                (*text_changed)         (AdgTextual     *textual,
                                                 const gchar    *old_text);
};


GType           adg_textual_get_type            (void);
void            adg_textual_set_font_dress      (AdgTextual     *textual,
                                                 AdgDress        dress);
AdgDress        adg_textual_get_font_dress      (AdgTextual     *textual);
void            adg_textual_set_text            (AdgTextual     *textual,
                                                 const gchar    *text);
gchar *         adg_textual_dup_text            (AdgTextual     *textual);
void            adg_textual_text_changed        (AdgTextual     *textual,
                                                 const gchar    *old_text);

G_END_DECLS


#endif /* __ADG_TEXTUAL_H__ */
