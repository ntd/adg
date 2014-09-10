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


/**
 * SECTION:adg-text
 * @short_description: A pango based text entity
 *
 * The #AdgText class is the main class for showing text. It works
 * the same way as #AdgToyText but uses pango instead of the so
 * called cairo "toy" API.
 *
 * The text entity is not subject to the local matrix, only its origin is.
 *
 * <note><para>
 * By default, the #AdgEntity:local-mix property is set to
 * #ADG_MIX_ANCESTORS_NORMALIZED on #AdgText entities.
 * </para></note>
 *
 * Since: 1.0
 **/

/**
 * AdgText:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <pango/pangocairo.h>

#include "adg-dress.h"
#include "adg-style.h"
#include "adg-font-style.h"
#include "adg-pango-style.h"
#include "adg-textual.h"
#include "adg-entity-private.h"

#include "adg-text.h"
#include "adg-text-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_text_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_text_parent_class)


static void             _adg_iface_init         (AdgTextualIface *iface);


G_DEFINE_TYPE_WITH_CODE(AdgText, adg_text, ADG_TYPE_ENTITY,
                        G_IMPLEMENT_INTERFACE(ADG_TYPE_TEXTUAL, _adg_iface_init))

enum {
    PROP_0,
    PROP_FONT_DRESS,
    PROP_TEXT
};


static void             _adg_dispose            (GObject        *object);
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
static void             _adg_refresh_extents    (AdgText        *text);
static void             _adg_clear_layout       (AdgText        *text);


static void
adg_text_class_init(AdgTextClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTextPrivate));

    gobject_class->dispose = _adg_dispose;
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
adg_text_init(AdgText *text)
{
    AdgTextPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(text, ADG_TYPE_TEXT,
                                                       AdgTextPrivate);
    AdgEntityPrivate *entity_data = ((AdgEntity *) text)->data;

    data->font_dress = ADG_DRESS_FONT_TEXT;
    data->text = NULL;
    data->layout = NULL;

    text->data = data;

    /* Initialize to custom default some AdgEntity field by directly
     * accessing the private struct to avoid notify signal emissions
     */
    entity_data->local_mix = ADG_MIX_ANCESTORS_NORMALIZED;
}

static void
_adg_dispose(GObject *object)
{
    AdgText *text = (AdgText *) object;

    _adg_clear_layout(text);

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_finalize(GObject *object)
{
    AdgText *text;
    AdgTextPrivate *data;

    text = (AdgText *) object;
    data = text->data;

    g_free(data->text);

    if (_ADG_OLD_OBJECT_CLASS->finalize)
        _ADG_OLD_OBJECT_CLASS->finalize(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgTextPrivate *data = ((AdgText *) object)->data;

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
    AdgText *text;
    AdgTextPrivate *data;

    text = (AdgText *) object;
    data = text->data;

    switch (prop_id) {
    case PROP_FONT_DRESS:
        data->font_dress = g_value_get_enum(value);
        _adg_clear_layout(text);
        break;
    case PROP_TEXT:
        g_free(data->text);
        data->text = g_value_dup_string(value);
        _adg_clear_layout(text);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_text_new:
 * @text: the text
 *
 * Creates a new text entity using @text as its content.
 *
 * Returns: the newly created text entity
 *
 * Since: 1.0
 **/
AdgText *
adg_text_new(const gchar *text)
{
    return g_object_new(ADG_TYPE_TEXT,
                        "text", text, NULL);
}


static void
_adg_global_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->global_changed != NULL)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    _adg_refresh_extents((AdgText *) entity);
}

static void
_adg_local_changed(AdgEntity *entity)
{
    if (_ADG_OLD_ENTITY_CLASS->local_changed != NULL)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);

    _adg_refresh_extents((AdgText *) entity);
}

static void
_adg_invalidate(AdgEntity *entity)
{
    _adg_clear_layout((AdgText *) entity);

    if (_ADG_OLD_ENTITY_CLASS->invalidate)
        _ADG_OLD_ENTITY_CLASS->invalidate(entity);
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgText *text;
    AdgTextPrivate *data;
    PangoRectangle size;

    text = (AdgText *) entity;
    data = text->data;

    if (adg_is_string_empty(data->text)) {
        /* Undefined text */
        CpmlExtents new_extents;
        new_extents.is_defined = FALSE;
        adg_entity_set_extents(entity, &new_extents);
        _adg_clear_layout(text);
        return;
    } else if (data->layout != NULL) {
        /* Cached result */
        return;
    }

    if (data->layout == NULL) {
        static PangoFontMap *font_map = NULL;
        AdgDress dress;
        AdgPangoStyle *pango_style;
        PangoFontDescription *font_description;
        cairo_font_options_t *options;
        PangoContext *context;

        /* Keep around a font_map object. The rationale is here:
         * https://bugzilla.gnome.org/show_bug.cgi?id=143542
         *
         * Basically, PangoFontMap is an heavy object and
         * creating/destroying it is not the right way.
         *
         * In reality, the blocking issue for me was the following
         * line make the adg-demo program crash on MinGW32:
         * g_object_unref(font_map);
         */
        if (font_map == NULL)
            font_map = pango_cairo_font_map_new();

        dress = data->font_dress;
        pango_style = (AdgPangoStyle *) adg_entity_style(entity, dress);
        font_description = adg_pango_style_get_description(pango_style);

        context = pango_context_new();
        pango_context_set_font_map(context, font_map);
        pango_cairo_context_set_resolution(context, 72);

        options = adg_font_style_new_options((AdgFontStyle *) pango_style);
        pango_cairo_context_set_font_options(context, options);
        cairo_font_options_destroy(options);

        data->layout = pango_layout_new(context);
        g_object_unref(context);

        pango_layout_set_text(data->layout, data->text, -1);
        pango_layout_set_font_description(data->layout, font_description);
    }

    pango_layout_get_extents(data->layout, NULL, &size);

    data->raw_extents.org.x = pango_units_to_double(size.x);
    data->raw_extents.org.y = pango_units_to_double(size.y);
    data->raw_extents.size.x = pango_units_to_double(size.width);
    data->raw_extents.size.y = pango_units_to_double(size.height);
    data->raw_extents.is_defined = TRUE;

    _adg_refresh_extents((AdgText *) entity);
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgText *text;
    AdgTextPrivate *data;

    text = (AdgText *) entity;
    data = text->data;

    if (data->layout != NULL) {
        adg_entity_apply_dress(entity, data->font_dress, cr);
        cairo_transform(cr, adg_entity_get_global_matrix(entity));
        cairo_transform(cr, adg_entity_get_local_matrix(entity));

        /* Realign the text to follow the cairo toy text convention:
         * use bottom/left corner as reference (pango uses top/left). */
        cairo_translate(cr, 0, -data->raw_extents.size.y);

        pango_cairo_update_layout(cr, data->layout);
        pango_cairo_show_layout(cr, data->layout);
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
    AdgTextPrivate *data = ((AdgText *) textual)->data;
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
    AdgTextPrivate *data = ((AdgText *) textual)->data;
    return g_strdup(data->text);
}

static void
_adg_refresh_extents(AdgText *text)
{
    AdgTextPrivate *data;
    AdgEntity *entity;
    cairo_matrix_t ctm;
    CpmlExtents new_extents;

    data = text->data;

    if (! data->raw_extents.is_defined)
        return;

    entity = (AdgEntity *) text;

    adg_matrix_copy(&ctm, adg_entity_get_global_matrix(entity));
    adg_matrix_transform(&ctm, adg_entity_get_local_matrix(entity),
                         ADG_TRANSFORM_AFTER);
    cpml_extents_copy(&new_extents, &data->raw_extents);

    /* Realign the text to follow the cairo toy text convention:
     * use bottom/left corner as reference (pango uses top/left). */
    new_extents.org.y -= new_extents.size.y;

    cpml_extents_transform(&new_extents, &ctm);
    adg_entity_set_extents(entity, &new_extents);
}

static void
_adg_clear_layout(AdgText *text)
{
    AdgTextPrivate *data = text->data;

    if (data->layout != NULL) {
        g_object_unref(data->layout);
        data->layout = NULL;
    }
}
