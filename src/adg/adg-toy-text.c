/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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
 * @short_description: Simple text entity that uses the cairo "toy" text API
 *
 * The #AdgToyText class is a basic class to show simple text. It internally
 * uses the so called cairo "toy" API and it shares the same limitations.
 *
 * The toy text entity is not subject to the local matrix, only its origin is.
 *
 * <note><para>
 * By default, the #AdgEntity:local-mix property is set to
 * #ADG_MIX_ANCESTORS_NORMALIZED on #AdgToyText entities.
 * </para></note>
 *
 * Since: 1.0
 **/

/**
 * AdgToyText:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-dress.h"
#include "adg-style.h"
#include "adg-font-style.h"
#include "adg-textual.h"
#include "adg-entity-private.h"

#include "adg-toy-text.h"
#include "adg-toy-text-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_toy_text_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_toy_text_parent_class)


static void             _adg_iface_init         (AdgTextualIface *iface);


G_DEFINE_TYPE_WITH_CODE(AdgToyText, adg_toy_text, ADG_TYPE_ENTITY,
                        G_IMPLEMENT_INTERFACE(ADG_TYPE_TEXTUAL, _adg_iface_init))

enum {
    PROP_0,
    PROP_FONT_DRESS,
    PROP_TEXT
};


static void             _adg_finalize           (GObject        *object);
static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_global_changed     (AdgEntity      *entity);
static void             _adg_local_changed      (AdgEntity      *entity);
static void             _adg_invalidate         (AdgEntity      *entity);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static void             _adg_set_font_dress     (AdgTextual     *textual,
                                                 AdgDress        dress);
static AdgDress         _adg_get_font_dress     (AdgTextual     *textual);
static void             _adg_set_text           (AdgTextual     *textual,
                                                 const gchar    *text);
static gchar *          _adg_dup_text           (AdgTextual     *textual);
static void             _adg_clear_font         (AdgToyText     *toy_text);
static void             _adg_clear_glyphs       (AdgToyText     *toy_text);


static void
adg_toy_text_class_init(AdgToyTextClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgToyTextPrivate));

    gobject_class->finalize = _adg_finalize;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;
    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    g_object_class_override_property(gobject_class, PROP_FONT_DRESS, "font-dress");
    g_object_class_override_property(gobject_class, PROP_TEXT, "text");
}

static void
_adg_iface_init(AdgTextualIface *iface)
{
    iface->set_font_dress = _adg_set_font_dress;
    iface->get_font_dress = _adg_get_font_dress;
    iface->set_text = _adg_set_text;
    iface->dup_text = _adg_dup_text;
    iface->text_changed = NULL;
}

static void
adg_toy_text_init(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(toy_text,
                                                          ADG_TYPE_TOY_TEXT,
                                                          AdgToyTextPrivate);
    AdgEntityPrivate *entity_data = ((AdgEntity *) toy_text)->data;

    data->font_dress = ADG_DRESS_FONT_TEXT;
    data->text = NULL;
    data->glyphs = NULL;

    toy_text->data = data;

    /* Initialize to custom default some AdgEntity field by directly
     * accessing the private struct to avoid notify signal emissions
     */
    entity_data->local_mix = ADG_MIX_ANCESTORS_NORMALIZED;
}

static void
_adg_finalize(GObject *object)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;

    toy_text = (AdgToyText *) object;
    data = toy_text->data;

    g_free(data->text);
    _adg_clear_font(toy_text);
    _adg_clear_glyphs(toy_text);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgToyTextPrivate *data = ((AdgToyText *) object)->data;

    switch (prop_id) {
    case PROP_FONT_DRESS:
        g_value_set_enum(value, data->font_dress);
        break;
    case PROP_TEXT:
        g_value_set_string(value, data->text);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;

    toy_text = (AdgToyText *) object;
    data = toy_text->data;

    switch (prop_id) {
    case PROP_FONT_DRESS:
        data->font_dress = g_value_get_enum(value);
        _adg_clear_font(toy_text);
        break;
    case PROP_TEXT:
        g_free(data->text);
        data->text = g_value_dup_string(value);
        _adg_clear_glyphs(toy_text);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_toy_text_new:
 * @text: the text
 *
 * Creates a new toy text entity using @text as its content.
 *
 * Returns: the newly created toy text entity
 *
 * Since: 1.0
 **/
AdgToyText *
adg_toy_text_new(const gchar *text)
{
    return g_object_new(ADG_TYPE_TOY_TEXT,
                        "text", text, NULL);
}


static void
_adg_global_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->global_changed)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    adg_entity_invalidate(entity);
}

static void
_adg_local_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);

    adg_entity_invalidate(entity);
}

static void
_adg_invalidate(AdgEntity *entity)
{
    _adg_clear_font((AdgToyText *) entity);
    _adg_clear_glyphs((AdgToyText *) entity);

    if (_ADG_OLD_ENTITY_CLASS->invalidate)
        _ADG_OLD_ENTITY_CLASS->invalidate(entity);
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;
    CpmlExtents extents;

    toy_text = (AdgToyText *) entity;
    data = toy_text->data;

    if (data->font == NULL) {
        AdgDress dress;
        AdgFontStyle *font_style;
        cairo_matrix_t ctm;

        dress = data->font_dress;
        font_style = (AdgFontStyle *) adg_entity_style(entity, dress);

        adg_matrix_copy(&ctm, adg_entity_get_global_matrix(entity));
        adg_matrix_transform(&ctm, adg_entity_get_local_matrix(entity),
                             ADG_TRANSFORM_BEFORE);

        data->font = adg_font_style_get_scaled_font(font_style, &ctm);
    }

    if (adg_is_string_empty(data->text)) {
        /* Undefined text */
        extents.is_defined = FALSE;
    } else if (data->glyphs != NULL) {
        /* Cached result */
        return;
    } else {
        cairo_status_t status;
        cairo_text_extents_t cairo_extents;

        status = cairo_scaled_font_text_to_glyphs(data->font, 0, 0,
                                                  data->text, -1,
                                                  &data->glyphs,
                                                  &data->num_glyphs,
                                                  NULL, NULL, NULL);

        if (status != CAIRO_STATUS_SUCCESS) {
            _adg_clear_glyphs(toy_text);
            g_error(_("Unable to build glyphs (cairo message: %s)"),
                    cairo_status_to_string(status));
            return;
        }

        cairo_scaled_font_glyph_extents(data->font, data->glyphs,
                                        data->num_glyphs, &cairo_extents);
        cpml_extents_from_cairo_text(&extents, &cairo_extents);
        cpml_extents_transform(&extents, adg_entity_get_local_matrix(entity));
        cpml_extents_transform(&extents, adg_entity_get_global_matrix(entity));

    }

    adg_entity_set_extents(entity, &extents);
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgToyText *toy_text;
    AdgToyTextPrivate *data;

    toy_text = (AdgToyText *) entity;
    data = toy_text->data;

    if (data->glyphs != NULL) {
        adg_entity_apply_dress(entity, data->font_dress, cr);
        cairo_transform(cr, adg_entity_get_global_matrix(entity));
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_show_glyphs(cr, data->glyphs, data->num_glyphs);
    }
}

static void
_adg_set_font_dress(AdgTextual *textual, AdgDress dress)
{
    g_object_set(textual, "font-dress", dress, NULL);
}

static AdgDress
_adg_get_font_dress(AdgTextual *textual)
{
    AdgToyTextPrivate *data = ((AdgToyText *) textual)->data;
    return data->font_dress;
}

static void
_adg_set_text(AdgTextual *textual, const gchar *text)
{
    g_object_set(textual, "text", text, NULL);
}

static gchar *
_adg_dup_text(AdgTextual *textual)
{
    AdgToyTextPrivate *data = ((AdgToyText *) textual)->data;
    return g_strdup(data->text);
}

static void
_adg_clear_font(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = toy_text->data;

    data->font = NULL;
}

static void
_adg_clear_glyphs(AdgToyText *toy_text)
{
    AdgToyTextPrivate *data = toy_text->data;

    if (data->glyphs != NULL) {
        cairo_glyph_free(data->glyphs);
        data->glyphs = NULL;
    }

    data->num_glyphs = 0;
}
