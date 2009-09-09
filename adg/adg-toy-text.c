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


/**
 * SECTION:adg-toy-text
 * @short_description: Simple text entity that use the cairo "toy" text API
 *
 * The #AdgToyText class is a basic class to show simple text. It internally
 * uses the so called cairo "toy" API and it shares the same limitations.
 *
 * The toy text entity is not subject to the local matrix, only its origin is.
 **/

/**
 * AdgToyText:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-toy-text.h"
#include "adg-toy-text-private.h"
#include "adg-font-style.h"
#include "adg-type-builtins.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_toy_text_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_toy_text_parent_class)


enum {
    PROP_0,
    PROP_LABEL,
    PROP_FONT_STYLE
};


static void     finalize                (GObject        *object);
static void     get_property            (GObject        *object,
                                         guint           param_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           param_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     get_local_matrix        (AdgEntity      *entity,
                                         AdgMatrix      *matrix);
static gboolean invalidate              (AdgEntity      *entity);
static gboolean render                  (AdgEntity      *entity,
                                         cairo_t        *cr);
static gboolean set_label               (AdgToyText     *toy_text,
                                         const gchar    *label);
static gboolean set_font_style          (AdgToyText     *toy_text,
                                         AdgFontStyleId  font_style);
static void     update_label_cache      (AdgToyText     *toy_text,
                                         cairo_t        *cr);
static void     clear_label_cache       (AdgToyText     *toy_text);


G_DEFINE_TYPE(AdgToyText, adg_toy_text, ADG_TYPE_ENTITY);


static void
adg_toy_text_class_init(AdgToyTextClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgToyTextPrivate));

    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->get_local_matrix = get_local_matrix;
    entity_class->invalidate = invalidate;
    entity_class->render = render;

    param = g_param_spec_string("label",
                                P_("Label"),
                                P_("The label to display"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LABEL, param);

    param = g_param_spec_enum("font-style",
                              P_("Font Style"),
                              P_("The identifier of the font to use while rendering the label"),
                              ADG_TYPE_FONT_STYLE_ID,
                              ADG_FONT_STYLE_TEXT,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FONT_STYLE, param);
}

static void
adg_toy_text_init(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(toy_text,
                                                          ADG_TYPE_TOY_TEXT,
                                                          AdgToyTextPrivate);

    data->label = NULL;
    data->font_style = ADG_FONT_STYLE_TEXT;
    data->glyphs = NULL;

    toy_text->data = data;
}

static void
finalize(GObject *object)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;

    toy_text = (AdgToyText *) object;
    data = toy_text->data;

    g_free(data->label);
    clear_label_cache(toy_text);

    if (PARENT_OBJECT_CLASS->finalize != NULL)
        PARENT_OBJECT_CLASS->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgToyTextPrivate *data = ((AdgToyText *) object)->data;

    switch (prop_id) {
    case PROP_LABEL:
        g_value_set_string(value, data->label);
        break;
    case PROP_FONT_STYLE:
        g_value_set_enum(value, data->font_style);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AdgToyText *toy_text = (AdgToyText *) object;

    switch (prop_id) {
    case PROP_LABEL:
        set_label(toy_text, g_value_get_string(value));
        break;
    case PROP_FONT_STYLE:
        set_font_style(toy_text, g_value_get_enum(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_toy_text_new:
 * @label: the label text
 *
 * Creates a new toy text entity using @label as its text
 *
 * Returns: the newly created toy text entity
 **/
AdgToyText *
adg_toy_text_new(const gchar *label)
{
    return g_object_new(ADG_TYPE_TOY_TEXT, "label", label, NULL);
}

/**
 * adg_toy_text_get_label:
 * @toy_text: an #AdgToyText
 *
 * Gets the label text. The string is internally owned and
 * must not be freed or modified.
 *
 * Returns: the label text
 **/
const gchar *
adg_toy_text_get_label(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data;

    g_return_val_if_fail(ADG_IS_TOY_TEXT(toy_text), NULL);

    data = toy_text->data;

    return data->label;
}

/**
 * adg_toy_text_set_label:
 * @toy_text: an #AdgToyText
 * @label: the label text
 *
 * Sets a new label for @toy_text. @label can be also %NULL,
 * in which case will be treated as an empty string.
 **/
void
adg_toy_text_set_label(AdgToyText *toy_text, const gchar *label)
{
    g_return_if_fail(ADG_IS_TOY_TEXT(toy_text));

    if (set_label(toy_text, label))
        g_object_notify((GObject *) toy_text, "label");
}

/**
 * adg_toy_text_get_font_style:
 * @toy_text: an #AdgToyText
 *
 * Gets the font style id of this label.
 *
 * Returns: the font id
 **/
const AdgFontStyleId
adg_toy_text_get_font_style(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data;

    g_return_val_if_fail(ADG_IS_TOY_TEXT(toy_text), ADG_FONT_STYLE_TEXT);

    data = toy_text->data;

    return data->font_style;
}

/**
 * adg_toy_text_set_font_style:
 * @toy_text: an #AdgToyText
 * @font_style: the new font style id to use
 *
 * Sets a new font style for @toy_text.
 **/
void
adg_toy_text_set_font_style(AdgToyText *toy_text, AdgFontStyleId font_style)
{
    g_return_if_fail(ADG_IS_TOY_TEXT(toy_text));

    if (set_font_style(toy_text, font_style))
        g_object_notify((GObject *) toy_text, "font-style");
}

/**
 * adg_toy_text_get_extents:
 * @toy_text: an #AdgToyText
 * @cr: a cairo context
 * @extents: where to store the extents
 *
 * Computes the extents of @toy_text and returns the result in @extents.
 * The cairo context is required for font computation.
 **/
void
adg_toy_text_get_extents(AdgToyText *toy_text, cairo_t *cr,
                         cairo_text_extents_t *extents)
{
    AdgToyTextPrivate *data;

    g_return_if_fail(ADG_IS_TOY_TEXT(toy_text));
    g_return_if_fail(extents != NULL);

    update_label_cache(toy_text, cr);

    data = toy_text->data;
    *extents = data->extents;
}


static void
get_local_matrix(AdgEntity *entity, AdgMatrix *matrix)
{
    PARENT_ENTITY_CLASS->get_local_matrix(entity, matrix);
    cairo_matrix_init_translate(matrix, matrix->x0, matrix->y0);
}

static gboolean
invalidate(AdgEntity *entity)
{
    clear_label_cache((AdgToyText *) entity);

    if (PARENT_ENTITY_CLASS->invalidate != NULL)
        return PARENT_ENTITY_CLASS->invalidate(entity);

    return TRUE;
}

static gboolean
render(AdgEntity *entity, cairo_t *cr)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;

    toy_text = (AdgToyText *) entity;
    data = toy_text->data;

    if (data->label != NULL && data->label[0] != '\0') {
        update_label_cache(toy_text, cr);

        cairo_save(cr);
        adg_entity_apply_local_matrix(entity, cr);
        cairo_show_glyphs(cr, data->glyphs, data->num_glyphs);
        cairo_restore(cr);
    }

    return TRUE;
}


static gboolean
set_label(AdgToyText *toy_text, const gchar *label)
{
    AdgToyTextPrivate *data = toy_text->data;

    /* Check if the new label differs from the current label */
    if (adg_strcmp(label, data->label) == 0)
        return FALSE;

    g_free(data->label);
    data->label = g_strdup(label);

    clear_label_cache(toy_text);
    return TRUE;
}

static gboolean
set_font_style(AdgToyText *toy_text, AdgFontStyleId font_style)
{
    AdgToyTextPrivate *data = toy_text->data;

    /* Check if the new style differs from the current one */
    if (font_style == data->font_style)
        return FALSE;

    data->font_style = font_style;

    /* Changing the font invalidate the cache */
    clear_label_cache(toy_text);
    return TRUE;
}

static void
update_label_cache(AdgToyText *toy_text, cairo_t *cr)
{
    AdgToyTextPrivate *data;
    cairo_status_t status;

    data = toy_text->data;

    adg_entity_apply_font((AdgEntity *) toy_text, data->font_style, cr);

    if (data->glyphs != NULL)
        return;

    status = cairo_scaled_font_text_to_glyphs(cairo_get_scaled_font(cr),
                                              0, 0, data->label, -1,
                                              &data->glyphs,
                                              &data->num_glyphs,
                                              NULL, NULL, NULL);

    if (status != CAIRO_STATUS_SUCCESS) {
        clear_label_cache(toy_text);
        g_error("Unable to build glyphs (cairo message: %s)",
                cairo_status_to_string(status));
        return;
    }

    cairo_glyph_extents(cr, data->glyphs, data->num_glyphs, &data->extents);
}

static void
clear_label_cache(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = toy_text->data;

    if (data->glyphs != NULL) {
        cairo_glyph_free(data->glyphs);
        data->glyphs = NULL;
    }

    data->num_glyphs = 0;
    memset(&data->extents, 0, sizeof(data->extents));
}
