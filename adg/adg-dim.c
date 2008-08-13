/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */

/**
 * SECTION:dim
 * @title: AdgDim
 * @short_description: Root abstract class for all dimension entities
 *
 * The #AdgDim class is the base stub of all the dimension entities.
 */

#include "adg-dim.h"
#include "adg-dim-private.h"
#include "adg-util.h"
#include "adg-intl.h"
#include <string.h>

#define PARENT_CLASS ((AdgEntityClass *) adg_dim_parent_class)


enum {
    PROP_0,
    PROP_REF1,
    PROP_REF2,
    PROP_POS1,
    PROP_POS2,
    PROP_LEVEL,
    PROP_QUOTE,
    PROP_TOLERANCE_UP,
    PROP_TOLERANCE_DOWN,
    PROP_NOTE
};


static void     finalize                        (GObject        *object);
static void     get_property                    (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void     set_property                    (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     model_matrix_changed            (AdgEntity      *entity,
                                                 AdgMatrix      *parent_matrix);
static void     invalidate                      (AdgEntity      *entity);
static gchar *  default_quote                   (AdgDim         *dim);
static void     quote_layout                    (AdgDim         *dim,
                                                 cairo_t        *cr);
static gboolean text_cache_update               (AdgTextCache   *text_cache,
                                                 const gchar    *text,
                                                 cairo_t        *cr,
                                                 AdgStyle       *style);
static void     text_cache_invalidate           (AdgTextCache   *text_cache);
static void     text_cache_move_to              (AdgTextCache   *text_cache,
                                                 const CpmlPair *to);
static void     text_cache_render               (AdgTextCache   *text_cache,
                                                 cairo_t        *cr);


G_DEFINE_ABSTRACT_TYPE(AdgDim, adg_dim, ADG_TYPE_ENTITY);


static void
adg_dim_class_init(AdgDimClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgDimPrivate));

    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->model_matrix_changed = model_matrix_changed;
    entity_class->invalidate = invalidate;

    klass->default_quote = default_quote;
    klass->quote_layout = quote_layout;

    param = g_param_spec_boxed("ref1",
                               P_("Reference 1"),
                               P_("First reference point of the dimension"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_REF1, param);

    param = g_param_spec_boxed("ref2",
                               P_("Reference 2"),
                               P_("Second reference point of the dimension"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_REF2, param);

    param = g_param_spec_boxed("pos1",
                               P_("Position 1"),
                               P_("First position point: it will be computed with the level property to get the real dimension position"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_POS1, param);

    param = g_param_spec_boxed("pos2",
                               P_("Position 2"),
                               P_("Second position point: it will be computed with the level property to get the real dimension position"),
                               ADG_TYPE_PAIR, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_POS2, param);

    param = g_param_spec_double("level",
                                P_("Level"),
                                P_("The dimension level, that is the factor to multiply dim_style->baseline_spacing to get the offset (in device units) from pos1..pos2 where render the dimension baseline"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 1.0,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_LEVEL, param);

    param = g_param_spec_string("quote",
                                P_("Quote"),
                                P_("The quote to display: set to NULL to get the default quote"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_QUOTE, param);

    param = g_param_spec_string("tolerance-up",
                                P_("Up Tolerance"),
                                P_("The up tolerance of the quote: set to NULL to suppress"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_UP, param);

    param = g_param_spec_string("tolerance-down",
                                P_("Down Tolerance"),
                                P_("The down tolerance of the quote: set to NULL to suppress"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOLERANCE_DOWN, param);

    param = g_param_spec_string("note",
                                P_("Note"),
                                P_("A custom note appended to the dimension quote"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NOTE, param);
}

static void
adg_dim_init(AdgDim *dim)
{
    AdgDimPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(dim, ADG_TYPE_DIM,
                                                      AdgDimPrivate);

    priv->ref1.x = priv->ref1.y = 0.;
    priv->ref2.x = priv->ref2.y = 0.;
    priv->pos1.x = priv->pos1.y = 0.;
    priv->pos2.x = priv->pos2.y = 0.;
    priv->level = 1.;
    priv->quote = NULL;
    priv->tolerance_up = NULL;
    priv->tolerance_down = NULL;
    priv->note = NULL;

    priv->quote_org.x = priv->quote_org.y = 0.;
    priv->quote_angle = 0.;
    priv->quote_cache.num_glyphs = 0;
    priv->quote_cache.glyphs = NULL;
    priv->tolerance_up_cache.num_glyphs = 0;
    priv->tolerance_up_cache.glyphs = NULL;
    priv->tolerance_down_cache.num_glyphs = 0;
    priv->tolerance_down_cache.glyphs = NULL;
    priv->note_cache.num_glyphs = 0;
    priv->note_cache.glyphs = NULL;

    dim->priv = priv;
}

static void
finalize(GObject *object)
{
    AdgDim *dim = (AdgDim *) object;

    g_free(dim->priv->quote);
    g_free(dim->priv->tolerance_up);
    g_free(dim->priv->tolerance_down);

    invalidate((AdgEntity *) dim);

    ((GObjectClass *) PARENT_CLASS)->finalize(object);
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgDim *dim = (AdgDim *) object;

    switch (prop_id) {
    case PROP_REF1:
        g_value_set_boxed(value, &dim->priv->ref1);
        break;
    case PROP_REF2:
        g_value_set_boxed(value, &dim->priv->ref2);
        break;
    case PROP_POS1:
        g_value_set_boxed(value, &dim->priv->pos1);
        break;
    case PROP_POS2:
        g_value_set_boxed(value, &dim->priv->pos1);
        break;
    case PROP_LEVEL:
        g_value_set_double(value, dim->priv->level);
        break;
    case PROP_QUOTE:
        g_value_set_string(value, dim->priv->quote);
        break;
    case PROP_TOLERANCE_UP:
        g_value_set_string(value, dim->priv->tolerance_up);
        break;
    case PROP_TOLERANCE_DOWN:
        g_value_set_string(value, dim->priv->tolerance_down);
        break;
    case PROP_NOTE:
        g_value_set_string(value, dim->priv->note);
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
    AdgEntity *entity;
    AdgDim    *dim;

    entity = (AdgEntity *) object;
    dim = (AdgDim *) object;

    switch (prop_id) {
    case PROP_REF1:
        cpml_pair_copy(&dim->priv->ref1, (AdgPair *) g_value_get_boxed(value));
        invalidate(entity);
        break;
    case PROP_REF2:
        cpml_pair_copy(&dim->priv->ref2, (AdgPair *) g_value_get_boxed(value));
        invalidate(entity);
        break;
    case PROP_POS1:
        cpml_pair_copy(&dim->priv->pos1, (AdgPair *) g_value_get_boxed(value));
        invalidate(entity);
        break;
    case PROP_POS2:
        cpml_pair_copy(&dim->priv->pos2, (AdgPair *) g_value_get_boxed(value));
        invalidate(entity);
        break;
    case PROP_LEVEL:
        dim->priv->level = g_value_get_double(value);
        invalidate(entity);
        break;
    case PROP_QUOTE:
        g_free(dim->priv->quote);
        dim->priv->quote = g_value_dup_string(value);
        text_cache_invalidate(&dim->priv->quote_cache);
        break;
    case PROP_TOLERANCE_UP:
        g_free(dim->priv->tolerance_up);
        dim->priv->tolerance_up = g_value_dup_string(value);
        text_cache_invalidate(&dim->priv->tolerance_up_cache);
        break;
    case PROP_TOLERANCE_DOWN:
        g_free(dim->priv->tolerance_down);
        dim->priv->tolerance_down = g_value_dup_string(value);
        text_cache_invalidate(&dim->priv->tolerance_down_cache);
        break;
    case PROP_NOTE:
        g_free(dim->priv->note);
        dim->priv->note = g_value_dup_string(value);
        text_cache_invalidate(&dim->priv->note_cache);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


static void
model_matrix_changed(AdgEntity *entity, AdgMatrix *parent_matrix)
{
    //invalidate(entity);
    PARENT_CLASS->model_matrix_changed(entity, parent_matrix);
}

static void
invalidate(AdgEntity *entity)
{
    AdgDim *dim = (AdgDim *) entity;

    dim->priv->quote_angle = 0.;
    dim->priv->quote_org.x = 0.;
    dim->priv->quote_org.y = 0.;

    text_cache_invalidate(&dim->priv->quote_cache);
    text_cache_invalidate(&dim->priv->tolerance_up_cache);
    text_cache_invalidate(&dim->priv->tolerance_down_cache);
    text_cache_invalidate(&dim->priv->note_cache);
}


static gchar *
default_quote(AdgDim *dim)
{
    g_warning("AdgDim::default_quote not implemented for `%s'",
              g_type_name(G_TYPE_FROM_INSTANCE(dim)));
    return g_strdup("undef");
}

static void
quote_layout(AdgDim *dim, cairo_t *cr)
{
    AdgDimPrivate *priv;
    AdgDimStyle *dim_style;
    AdgPair shift;
    CpmlPair cp, org;
    CpmlPair quote_org, tolerance_up_org, tolerance_down_org, note_org;
    cairo_text_extents_t extents;

    priv = dim->priv;
    dim_style = (AdgDimStyle *) adg_entity_get_style((AdgEntity *) dim,
                                                     ADG_SLOT_DIM_STYLE);

    /* Compute the quote */
    quote_org.x = 0.;
    quote_org.y = 0.;
    if (text_cache_update(&priv->quote_cache, priv->quote, cr,
                          adg_dim_style_get_quote_style(dim_style))) {
        cp.x = priv->quote_cache.extents.width;
        cp.y = priv->quote_cache.extents.height / -2.;
    } else {
        cp.x = 0.;
        cp.y = 0.;
    }

    /* Compute the tolerances */
    if (priv->tolerance_up != NULL || priv->tolerance_down != NULL) {
        double width;
        double midspacing;

        adg_style_apply(adg_dim_style_get_tolerance_style(dim_style), cr);

        width = 0.;
        midspacing = adg_dim_style_get_tolerance_spacing(dim_style) / 2.;
        cpml_pair_copy(&shift, adg_dim_style_get_tolerance_shift(dim_style));
        cp.x += shift.x;

        if (text_cache_update(&priv->tolerance_up_cache,
                              priv->tolerance_up, cr, NULL)) {
            tolerance_up_org.x = cp.x;
            tolerance_up_org.y = cp.y + shift.y - midspacing;

            width = priv->tolerance_up_cache.extents.width;
        }

        if (text_cache_update(&priv->tolerance_down_cache,
                              priv->tolerance_down, cr, NULL)) {
            tolerance_down_org.x = cp.x;
            tolerance_down_org.y = cp.y + shift.y + midspacing +
                priv->tolerance_down_cache.extents.height;

            if (extents.width > width)
                width = priv->tolerance_down_cache.extents.width;
        }

        cp.x += width;
    }

    /* Compute the note */
    if (text_cache_update(&priv->note_cache, priv->note, cr,
                          adg_dim_style_get_note_style(dim_style))) {
        cpml_pair_copy(&shift, adg_dim_style_get_note_shift(dim_style));
        cp.x += shift.x;

        note_org.x = cp.x;
        note_org.y = cp.y + shift.y + priv->note_cache.extents.height / 2.;

        cp.x += priv->note_cache.extents.width;
    }

    /* Centering and shifting the whole group */
    cpml_pair_copy(&shift, adg_dim_style_get_quote_shift(dim_style));
    shift.x -= cp.x / 2.;

    if (priv->quote_cache.glyphs) {
        quote_org.x += shift.x;
        quote_org.y += shift.y;
        text_cache_move_to(&priv->quote_cache, &quote_org);
    }

    if (priv->tolerance_up_cache.glyphs) {
        tolerance_up_org.x += shift.x;
        tolerance_up_org.y += shift.y;
        text_cache_move_to(&priv->tolerance_up_cache, &tolerance_up_org);
    }

    if (priv->tolerance_down_cache.glyphs) {
        tolerance_down_org.x += shift.x;
        tolerance_down_org.y += shift.y;
        text_cache_move_to(&priv->tolerance_down_cache, &tolerance_down_org);
    }

    if (priv->note_cache.glyphs) {
        note_org.x += shift.x;
        note_org.y += shift.y;
        text_cache_move_to(&priv->note_cache, &note_org);
    }
}


const AdgPair *
adg_dim_get_ref1(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return &dim->priv->ref1;
}

const AdgPair *
adg_dim_get_ref2(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return &dim->priv->ref2;
}

void
adg_dim_set_ref(AdgDim *dim, const AdgPair *ref1, const AdgPair *ref2)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (ref1 != NULL || ref2 != NULL) {
        GObject *object = (GObject *) dim;

        g_object_freeze_notify(object);

        if (ref1 != NULL) {
            dim->priv->ref1 = *ref1;
            g_object_notify(object, "ref1");
        }

        if (ref2 != NULL) {
            dim->priv->ref2 = *ref2;
            g_object_notify(object, "ref2");
        }

        g_object_thaw_notify(object);
        invalidate((AdgEntity *) dim);
    }
}

void
adg_dim_set_ref_explicit(AdgDim *dim, double ref1_x, double ref1_y,
                         double ref2_x, double ref2_y)
{
    AdgPair ref1;
    AdgPair ref2;

    ref1.x = ref1_x;
    ref1.y = ref1_y;
    ref2.x = ref2_x;
    ref2.y = ref2_y;

    adg_dim_set_ref(dim, &ref1, &ref2);
}

const AdgPair *
adg_dim_get_pos1(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return &dim->priv->pos1;
}

const AdgPair *
adg_dim_get_pos2(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return &dim->priv->pos2;
}

void
adg_dim_set_pos(AdgDim *dim, AdgPair *pos1, AdgPair *pos2)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    if (pos1 != NULL || pos2 != NULL) {
        GObject *object = (GObject *) dim;

        g_object_freeze_notify(object);

        if (pos1 != NULL) {
            dim->priv->pos1 = *pos1;
            g_object_notify(object, "pos1");
        }
        if (pos2 != NULL) {
            dim->priv->pos2 = *pos2;
            g_object_notify(object, "pos2");
        }

        g_object_thaw_notify(object);
        invalidate((AdgEntity *) dim);
    }
}

void
adg_dim_set_pos_explicit(AdgDim *dim, double pos1_x, double pos1_y,
                         double pos2_x, double pos2_y)
{
    AdgPair pos1;
    AdgPair pos2;

    pos1.x = pos1_x;
    pos1.y = pos1_y;
    pos2.x = pos2_x;
    pos2.y = pos2_y;

    adg_dim_set_pos(dim, &pos1, &pos2);
}

double
adg_dim_get_level(AdgDim  *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), 0.0);

    return dim->priv->level;
}

void
adg_dim_set_level(AdgDim *dim, double level)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    dim->priv->level = level;
    g_object_notify((GObject *) dim, "level");
}

const gchar *
adg_dim_get_quote(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return dim->priv->quote;
}

void
adg_dim_set_quote(AdgDim *dim, const gchar *quote)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_free(dim->priv->quote);
    dim->priv->quote = g_strdup(quote);
    g_object_notify((GObject *) dim, "quote");

    text_cache_invalidate(&dim->priv->quote_cache);
}

const gchar *
adg_dim_get_tolerance_up(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return dim->priv->tolerance_up;
}

void
adg_dim_set_tolerance_up(AdgDim *dim, const gchar *tolerance_up)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_free(dim->priv->tolerance_up);
    dim->priv->tolerance_up = g_strdup(tolerance_up);
    g_object_notify((GObject *) dim, "tolerance-up");

    text_cache_invalidate(&dim->priv->tolerance_up_cache);
}

const gchar *
adg_dim_get_tolerance_down(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return dim->priv->tolerance_down;
}

void
adg_dim_set_tolerance_down(AdgDim *dim, const gchar *tolerance_down)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_free(dim->priv->tolerance_down);
    dim->priv->tolerance_down = g_strdup(tolerance_down);
    g_object_notify((GObject *) dim, "tolerance-down");

    text_cache_invalidate(&dim->priv->tolerance_down_cache);
}

void
adg_dim_set_tolerances(AdgDim *dim,
                       const gchar *tolerance_up, const gchar *tolerance_down)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_object_freeze_notify((GObject *) dim);
    adg_dim_set_tolerance_up(dim, tolerance_up);
    adg_dim_set_tolerance_down(dim, tolerance_down);
    g_object_thaw_notify((GObject *) dim);
}

const gchar *
adg_dim_get_note(AdgDim *dim)
{
    g_return_val_if_fail(ADG_IS_DIM(dim), NULL);

    return dim->priv->note;
}

void
adg_dim_set_note(AdgDim *dim, const gchar *note)
{
    g_return_if_fail(ADG_IS_DIM(dim));

    g_free(dim->priv->note);
    dim->priv->note = g_strdup(note);
    g_object_notify((GObject *) dim, "note");

    text_cache_invalidate(&dim->priv->note_cache);
}


/**
 * adg_dim_render_quote:
 * @dim: an #AdgDim object
 * @cr: a #cairo_t drawing context
 *
 * Renders the quote of @dim at the @org position. This function
 * is only useful in new dimension implementations.
 */
void
adg_dim_render_quote(AdgDim *dim, cairo_t *cr)
{
    AdgDimPrivate *priv;
    AdgDimStyle *dim_style;

    g_return_if_fail(ADG_IS_DIM(dim));

    priv = dim->priv;
    dim_style = (AdgDimStyle *) adg_entity_get_style((AdgEntity *) dim,
                                                     ADG_SLOT_DIM_STYLE);

    if (priv->quote == NULL)
        adg_dim_set_quote(dim, ADG_DIM_GET_CLASS(dim)->default_quote(dim));

    cairo_save(cr);

    cairo_set_matrix(cr, adg_entity_get_paper_matrix((AdgEntity *) dim));
    ADG_DIM_GET_CLASS(dim)->quote_layout(dim, cr);
    cairo_set_matrix(cr, adg_entity_get_model_matrix((AdgEntity *) dim));

    cairo_translate(cr, priv->quote_org.x, priv->quote_org.y);
    adg_entity_scale_to_paper((AdgEntity *) dim, cr);
    cairo_rotate(cr, priv->quote_angle);

    /* Rendering quote */
    adg_style_apply(adg_dim_style_get_quote_style(dim_style), cr);
    text_cache_render(&priv->quote_cache, cr);

    /* Rendering tolerances */
    if (priv->tolerance_up != NULL || priv->tolerance_down != NULL) {
        adg_style_apply(adg_dim_style_get_tolerance_style(dim_style), cr);

        if (priv->tolerance_up != NULL)
            text_cache_render(&priv->tolerance_up_cache, cr);

        if (priv->tolerance_down != NULL)
            text_cache_render(&priv->tolerance_down_cache, cr);
    }

    /* Rendering the note */
    if (priv->note != NULL) {
        adg_style_apply(adg_dim_style_get_note_style(dim_style), cr);
        text_cache_render(&priv->note_cache, cr);
    }

    cairo_restore(cr);
}


static gboolean
text_cache_update(AdgTextCache *text_cache, const gchar *text,
                  cairo_t *cr, AdgStyle *style)
{
    if (!text)
        return FALSE;

    if (style)
        adg_style_apply(style, cr);

    if (!text_cache->glyphs) {
        cairo_scaled_font_text_to_glyphs(cairo_get_scaled_font(cr),
                                         0., 0., text, -1,
                                         &text_cache->glyphs,
                                         &text_cache->num_glyphs,
                                         NULL, NULL, NULL);
        cairo_glyph_extents(cr, text_cache->glyphs, text_cache->num_glyphs,
                            &text_cache->extents);
    }

    return TRUE;
}

static void
text_cache_invalidate(AdgTextCache *text_cache)
{
    if (text_cache->glyphs) {
        cairo_glyph_free(text_cache->glyphs);
        text_cache->glyphs = NULL;
    }
    text_cache->num_glyphs = 0;
    memset(&text_cache->extents, 0, sizeof(text_cache->extents));
}

static void
text_cache_move_to(AdgTextCache *text_cache, const CpmlPair *to)
{
    cairo_glyph_t *glyph;
    int cnt;
    double x, y;

    glyph = text_cache->glyphs;
    cnt = text_cache->num_glyphs;
    x = to->x - glyph->x;
    y = to->y - glyph->y;

    while (cnt --) {
        glyph->x += x;
        glyph->y += y;
        ++ glyph;
    }
}

static void
text_cache_render(AdgTextCache *text_cache, cairo_t *cr)
{
    cairo_show_glyphs(cr, text_cache->glyphs, text_cache->num_glyphs);
}
