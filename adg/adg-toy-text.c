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
#include "adg-dress-builtins.h"
#include "adg-font-style.h"
#include "adg-intl.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_toy_text_parent_class)
#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_toy_text_parent_class)


enum {
    PROP_0,
    PROP_DRESS,
    PROP_LABEL
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
static void     global_changed          (AdgEntity      *entity);
static void     invalidate              (AdgEntity      *entity);
static void     arrange                 (AdgEntity      *entity);
static void     render                  (AdgEntity      *entity,
                                         cairo_t        *cr);
static gboolean set_dress               (AdgToyText     *toy_text,
                                         AdgDress        dress);
static gboolean set_label               (AdgToyText     *toy_text,
                                         const gchar    *label);
static void     unset_font              (AdgToyText     *toy_text);
static void     unset_glyphs            (AdgToyText     *toy_text);


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

    entity_class->global_changed = global_changed;
    entity_class->invalidate = invalidate;
    entity_class->arrange = arrange;
    entity_class->render = render;

    param = adg_param_spec_dress("dress",
                                 P_("Font Dress"),
                                 P_("The font dress to use for rendering this text"),
                                 ADG_DRESS_TEXT,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DRESS, param);

    param = g_param_spec_string("label",
                                P_("Label"),
                                P_("The label to display"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LABEL, param);
}

static void
adg_toy_text_init(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(toy_text,
                                                          ADG_TYPE_TOY_TEXT,
                                                          AdgToyTextPrivate);

    data->dress = ADG_DRESS_TEXT;
    data->label = NULL;
    data->glyphs = NULL;

    adg_entity_set_local_mode((AdgEntity *) toy_text,
                              ADG_TRANSFORM_BEFORE_NORMALIZED);

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
    unset_font(toy_text);
    unset_glyphs(toy_text);

    if (PARENT_OBJECT_CLASS->finalize != NULL)
        PARENT_OBJECT_CLASS->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgToyTextPrivate *data = ((AdgToyText *) object)->data;

    switch (prop_id) {
    case PROP_DRESS:
        g_value_set_int(value, data->dress);
        break;
    case PROP_LABEL:
        g_value_set_string(value, data->label);
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
    case PROP_DRESS:
        set_dress(toy_text, g_value_get_int(value));
        break;
    case PROP_LABEL:
        set_label(toy_text, g_value_get_string(value));
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
 * adg_toy_text_get_dress:
 * @toy_text: an #AdgToyText
 *
 * Gets the font dress to be used in rendering @toy_text.
 *
 * Returns: the current font dress
 **/
AdgDress
adg_toy_text_get_dress(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data;

    g_return_val_if_fail(ADG_IS_TOY_TEXT(toy_text), ADG_DRESS_UNDEFINED);

    data = toy_text->data;

    return data->dress;
}

/**
 * adg_toy_text_set_dress:
 * @toy_text: an #AdgToyText
 * @dress: the new #AdgDress to use
 *
 * Sets a new font dress for rendering @toy_text. The new dress
 * must be related to the original dress for this property:
 * you cannot set a dress used for line styles to a dress
 * managing fonts.
 *
 * The check is done by calling adg_dress_are_related() with
 * @dress and the previous dress as arguments. Check out its
 * documentation for details on what is a related dress.
 **/
void
adg_toy_text_set_dress(AdgToyText *toy_text, AdgDress dress)
{
    g_return_if_fail(ADG_IS_TOY_TEXT(toy_text));

    if (set_dress(toy_text, dress))
        g_object_notify((GObject *) toy_text, "dress");
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


static void
global_changed(AdgEntity *entity)
{
    PARENT_ENTITY_CLASS->global_changed(entity);
    unset_font((AdgToyText *) entity);
}

static void
invalidate(AdgEntity *entity)
{
    unset_font((AdgToyText *) entity);
    unset_glyphs((AdgToyText *) entity);

    if (PARENT_ENTITY_CLASS->invalidate != NULL)
        return PARENT_ENTITY_CLASS->invalidate(entity);
}

static void
arrange(AdgEntity *entity)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;
    AdgMatrix local;
    CpmlExtents extents;

    toy_text = (AdgToyText *) entity;
    data = toy_text->data;

    if (data->font == NULL) {
        AdgFontStyle *font_style;
        AdgMatrix ctm;

        font_style = (AdgFontStyle *) adg_entity_style(entity, data->dress);
        adg_entity_get_ctm(entity, &ctm);

        data->font = adg_font_style_font(font_style, &ctm);
    }

    if (data->label == NULL || data->label[0] == '\0') {
        /* Undefined label */
        extents.is_defined = FALSE;
        adg_entity_set_extents(entity, &extents);
        return;
    }

    if (data->glyphs == NULL) {
        cairo_status_t status;
        cairo_text_extents_t cairo_extents;

        status = cairo_scaled_font_text_to_glyphs(data->font, 0, 0,
                                                  data->label, -1,
                                                  &data->glyphs,
                                                  &data->num_glyphs,
                                                  NULL, NULL, NULL);

        if (status != CAIRO_STATUS_SUCCESS) {
            unset_glyphs(toy_text);
            g_error("Unable to build glyphs (cairo message: %s)",
                    cairo_status_to_string(status));
            return;
        }

        cairo_scaled_font_glyph_extents(data->font, data->glyphs,
                                        data->num_glyphs, &cairo_extents);
        cpml_extents_from_cairo_text(&extents, &cairo_extents);
        adg_entity_get_local_map(entity, &local);
        cairo_matrix_transform_point(&local, &extents.org.x, &extents.org.y);
        cairo_matrix_transform_distance(&local, &extents.size.x, &extents.size.y);

        adg_entity_set_extents(entity, &extents);
    }
}

static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;

    toy_text = (AdgToyText *) entity;
    data = toy_text->data;

    if (data->glyphs != NULL) {
        AdgMatrix ctm;

        adg_entity_get_ctm(entity, &ctm);

        cairo_set_scaled_font(cr, data->font);
        cairo_set_matrix(cr, &ctm);
        cairo_show_glyphs(cr, data->glyphs, data->num_glyphs);
    }
}

static gboolean
set_dress(AdgToyText *toy_text, AdgDress dress)
{
    AdgToyTextPrivate *data = toy_text->data;

    if (adg_dress_set(&data->dress, dress)) {
        unset_font(toy_text);
        return TRUE;
    }

    return FALSE;
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

    unset_glyphs(toy_text);
    return TRUE;
}

static void
unset_font(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = toy_text->data;

    data->font = NULL;
}

static void
unset_glyphs(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = toy_text->data;

    if (data->glyphs != NULL) {
        cairo_glyph_free(data->glyphs);
        data->glyphs = NULL;
    }

    data->num_glyphs = 0;
}
