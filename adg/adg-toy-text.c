/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
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
 * SECTION:toytext
 * @title: AdgToyText
 * @short_description: Simple text entity that use the cairo "toy" text API
 *
 * The #AdgToyText class is a basic class to show simple text. It internally
 * uses the so called cairo "toy" API and it shares the same limitations.
 **/

#include "adg-toy-text.h"
#include "adg-toy-text-private.h"
#include "adg-positionable.h"
#include "adg-font-style.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgEntityClass *) adg_toy_text_parent_class)


enum {
    PROP_0,
    PROP_ORG,
    PROP_LABEL
};

static void     positionable_init       (AdgPositionableIface *iface);
static AdgPoint*org                     (AdgPositionable*positionable);

static void     finalize                (GObject        *object);
static void     get_property            (GObject        *object,
                                         guint           param_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           param_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     model_matrix_changed    (AdgEntity      *entity,
                                         AdgMatrix      *parent_matrix);
static void     invalidate              (AdgEntity      *entity);
static void     render                  (AdgEntity      *entity,
                                         cairo_t        *cr);
static gboolean update_org_cache        (AdgToyText     *toy_text);
static gboolean update_label_cache      (AdgToyText     *toy_text,
                                         cairo_t        *cr);
static void     clear_label_cache       (AdgToyText     *toy_text);
static void     move_label_cache        (AdgToyText     *toy_text,
                                         const AdgPair  *to);


G_DEFINE_TYPE_WITH_CODE(AdgToyText, adg_toy_text, ADG_TYPE_ENTITY,
                        G_IMPLEMENT_INTERFACE(ADG_TYPE_POSITIONABLE,
                                              positionable_init))


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
 
    entity_class->model_matrix_changed = model_matrix_changed;
    entity_class->invalidate = invalidate;
    entity_class->render = render;

    g_object_class_override_property(gobject_class, PROP_ORG, "org");

    param = g_param_spec_string("label",
                                P_("Label"),
                                P_("The label to display"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LABEL, param);
}

static void
adg_toy_text_init(AdgToyText *toy_text)
{
    AdgToyTextPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(toy_text,
                                                          ADG_TYPE_TOY_TEXT,
                                                          AdgToyTextPrivate);

    priv->label = NULL;
    adg_point_unset(&priv->org);
    priv->org_pair.x = priv->org_pair.y = 0.;
    priv->num_glyphs = 0;
    priv->glyphs = NULL;

    toy_text->priv = priv;
}

static void
positionable_init(AdgPositionableIface *iface)
{
    iface->org = org;
}

static AdgPoint *
org(AdgPositionable *positionable)
{
    return & ((AdgToyText *) positionable)->priv->org;
}

static void
finalize(GObject *object)
{
    AdgToyText *toy_text = (AdgToyText *) object;

    g_free(toy_text->priv->label);
    clear_label_cache(toy_text);

    ((GObjectClass *) PARENT_CLASS)->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgToyText *toy_text = (AdgToyText *) toy_text;

    switch (prop_id) {
    case PROP_ORG:
        g_value_set_boxed(value, &toy_text->priv->org);
        break;
    case PROP_LABEL:
        g_value_set_string(value, toy_text->priv->label);
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
    AdgToyTextPrivate *priv = toy_text->priv;

    switch (prop_id) {
    case PROP_ORG:
        adg_point_copy(&priv->org, (AdgPoint *) g_value_get_boxed(value));
        update_org_cache(toy_text);
        break;
    case PROP_LABEL:
        g_free(priv->label);
        priv->label = g_value_dup_string(value);
        clear_label_cache(toy_text);
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
 * Return value: the new entity
 */
AdgEntity *
adg_toy_text_new(const gchar *label)
{
    return (AdgEntity *) g_object_new(ADG_TYPE_TOY_TEXT, "label", label, NULL);
}

/**
 * adg_toy_text_get_label:
 * @toy_text: an #AdgToyText
 *
 * Gets the label text. The string is internally owned and
 * must not be freed or modified.
 *
 * Return value: the label text
 **/
const gchar *
adg_toy_text_get_label(AdgToyText *toy_text)
{
    g_return_val_if_fail(ADG_IS_TOY_TEXT(toy_text), NULL);

    return toy_text->priv->label;
}

/**
 * adg_toy_text_set_label:
 * @toy_text: an #AdgToyText
 * @label: the label text
 *
 * Explicitely sets the text to use as label.
 **/
void
adg_toy_text_set_label(AdgToyText *toy_text, const gchar *label)
{
    g_return_if_fail(ADG_IS_TOY_TEXT(label));

    g_free(toy_text->priv->label);
    toy_text->priv->label = g_strdup(label);
    g_object_notify((GObject *) toy_text, "label");

    clear_label_cache(toy_text);
}


static void
render(AdgEntity *entity, cairo_t *cr)
{
    AdgToyText *toy_text = (AdgToyText *) entity;
    AdgToyTextPrivate *priv = toy_text->priv;

    if (priv->label) {
        AdgStyle *font_style = adg_entity_get_style(entity, ADG_SLOT_FONT_STYLE);
        cairo_set_matrix(cr, adg_entity_get_paper_matrix(entity));
        adg_style_apply(font_style, cr);

        if (!priv->glyphs) {
            update_label_cache(toy_text, cr);
            update_org_cache(toy_text);
        }

        cairo_show_glyphs(cr, priv->glyphs, priv->num_glyphs);
    }

    PARENT_CLASS->render(entity, cr);
}

static void
model_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    update_org_cache((AdgToyText *) entity);
    PARENT_CLASS->model_matrix_changed(entity, parent_matrix);
}

static void
invalidate(AdgEntity *entity)
{
    clear_label_cache((AdgToyText *) entity);
    PARENT_CLASS->invalidate(entity);
}

static gboolean
update_org_cache(AdgToyText *toy_text)
{
    AdgToyTextPrivate *priv = toy_text->priv;
    AdgPoint *org = &toy_text->priv->org;
    AdgPair *org_pair = &toy_text->priv->org_pair;
    cairo_glyph_t *glyph;
    double x, y;
    int cnt;

    org = &priv->org;
    org_pair = &priv->org_pair;
    glyph = priv->glyphs;
    cnt = priv->num_glyphs;

    if (glyph == NULL || cnt <= 0) {
        /* No label cache to update: return */
        return TRUE;
    }

    adg_entity_point_to_paper_pair((AdgEntity *) toy_text, org, org_pair);
    if (org_pair->x == glyph->x && org_pair->y == glyph->y) {
        /* The label is yet properly positioned */
        return TRUE;
    }

    x = org_pair->x - glyph->x;
    y = org_pair->y - glyph->y;

    while (cnt --) {
        glyph->x += x;
        glyph->y += y;
        ++ glyph;
    }
}

static gboolean
update_label_cache(AdgToyText *toy_text, cairo_t *cr)
{
    AdgToyTextPrivate *priv = toy_text->priv;
    cairo_status_t status;

    status = cairo_scaled_font_text_to_glyphs(cairo_get_scaled_font(cr),
                                              0., 0., priv->label, -1,
                                              &priv->glyphs,
                                              &priv->num_glyphs,
                                              NULL, NULL, NULL);

    if (status != CAIRO_STATUS_SUCCESS) {
        g_error("Unable to build glyphs (cairo message: %s)",
                cairo_status_to_string(status));
        return FALSE;
    }

    cairo_glyph_extents(cr, priv->glyphs, priv->num_glyphs, &priv->extents);

    return TRUE;
}

static void
clear_label_cache(AdgToyText *toy_text)
{
    AdgToyTextPrivate *priv = toy_text->priv;

    if (priv->glyphs) {
        cairo_glyph_free(priv->glyphs);
        priv->glyphs = NULL;
    }
    priv->num_glyphs = 0;
    memset(&priv->extents, 0, sizeof(priv->extents));
}
