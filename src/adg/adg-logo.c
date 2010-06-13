/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-logo
 * @short_description: The ADG default logo
 *
 * The #AdgLogo is an entity representing the default ADG logo.
 **/

/**
 * AdgLogo:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-logo.h"
#include "adg-logo-private.h"
#include "adg-line-style.h"
#include "adg-dress-builtins.h"


G_DEFINE_TYPE(AdgLogo, adg_logo, ADG_TYPE_ENTITY);

enum {
    PROP_0,
    PROP_SYMBOL_DRESS,
    PROP_SCREEN_DRESS,
    PROP_FRAME_DRESS
};


static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           param_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_arrange            (AdgEntity      *entity);
static void             _adg_render             (AdgEntity      *entity,
                                                 cairo_t        *cr);
static void             _adg_arrange_class      (AdgLogoClass   *logo_class);


static void
adg_logo_class_init(AdgLogoClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;
    AdgLogoClassPrivate *data_class;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgLogoPrivate));

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    param = adg_param_spec_dress("symbol-dress",
                                 P_("Symbol Dress"),
                                 P_("The line dress to use for rendering the symbol of the logo"),
                                 ADG_DRESS_LINE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SYMBOL_DRESS, param);

    param = adg_param_spec_dress("screen-dress",
                                 P_("Screen Dress"),
                                 P_("The line dress to use for rendering the screen shape around the logo"),
                                 ADG_DRESS_LINE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SCREEN_DRESS, param);

    param = adg_param_spec_dress("frame-dress",
                                 P_("Frame Dress"),
                                 P_("The line dress to use for rendering the frame"),
                                 ADG_DRESS_LINE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FRAME_DRESS, param);

    /* Initialize the private class data: the allocated struct is
     * never freed as this type is registered statically, hence
     * never destroyed. A better approach would be to use the old
     * type initialization (no G_TYPE_DEFINE and friends) that
     * allows to specify a custom class finalization method */
    data_class = g_new(AdgLogoClassPrivate, 1);

    data_class->symbol = NULL;
    data_class->screen = NULL;
    data_class->frame = NULL;
    data_class->extents.is_defined = FALSE;

    klass->data_class = data_class;
}

static void
adg_logo_init(AdgLogo *logo)
{
    AdgLogoPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(logo, ADG_TYPE_LOGO,
                                                       AdgLogoPrivate);

    data->symbol_dress = ADG_DRESS_LINE;
    data->screen_dress = ADG_DRESS_LINE;
    data->frame_dress = ADG_DRESS_LINE;

    logo->data = data;
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgLogoPrivate *data = ((AdgLogo *) object)->data;

    switch (prop_id) {
    case PROP_SYMBOL_DRESS:
        g_value_set_int(value, data->symbol_dress);
        break;
    case PROP_SCREEN_DRESS:
        g_value_set_int(value, data->screen_dress);
        break;
    case PROP_FRAME_DRESS:
        g_value_set_int(value, data->frame_dress);
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
    AdgLogo *logo;
    AdgLogoPrivate *data;

    logo = (AdgLogo *) object;
    data = logo->data;

    switch (prop_id) {
    case PROP_SYMBOL_DRESS:
        data->symbol_dress = g_value_get_int(value);
        break;
    case PROP_SCREEN_DRESS:
        data->screen_dress = g_value_get_int(value);
        break;
    case PROP_FRAME_DRESS:
        data->frame_dress = g_value_get_int(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_logo_new:
 *
 * Creates a new logo entity.
 *
 * Returns: the newly created logo entity
 **/
AdgLogo *
adg_logo_new(void)
{
    return g_object_new(ADG_TYPE_LOGO, NULL);
}

/**
 * adg_logo_set_symbol_dress:
 * @logo: an #AdgLogo
 * @dress: the new #AdgDress to use
 *
 * Sets a new line dress for rendering the symbol of @logo. The
 * new dress must be a line dress: the check is done by calling
 * adg_dress_are_related() with @dress and the old dress as
 * arguments. Check out its documentation for further details.
 *
 * The default dress is a transparent line dress: the rendering
 * callback will stroke the symbol using the default color with
 * a predefined thickness.
 **/
void
adg_logo_set_symbol_dress(AdgLogo *logo, AdgDress dress)
{
    g_return_if_fail(ADG_IS_LOGO(logo));
    g_object_set((GObject *) logo, "symbol-dress", dress, NULL);
}

/**
 * adg_logo_get_symbol_dress:
 * @logo: an #AdgLogo
 *
 * Gets the line dress to be used in stroking the symbol of @logo.
 *
 * Returns: the requested line dress
 **/
AdgDress
adg_logo_get_symbol_dress(AdgLogo *logo)
{
    AdgLogoPrivate *data;

    g_return_val_if_fail(ADG_IS_LOGO(logo), ADG_DRESS_UNDEFINED);

    data = logo->data;

    return data->symbol_dress;
}

/**
 * adg_logo_set_screen_dress:
 * @logo: an #AdgLogo
 * @dress: the new #AdgDress to use
 *
 * Sets a new line dress for rendering the screen of @logo.
 * The new dress must be a line dress: the check is done by
 * calling adg_dress_are_related() with @dress and the old
 * dress as arguments. Check out its documentation for
 * further details.
 *
 * The default dress is a transparent line dress: the rendering
 * callback will stroke the screen using the default color with
 * a predefined thickness.
 **/
void
adg_logo_set_screen_dress(AdgLogo *logo, AdgDress dress)
{
    g_return_if_fail(ADG_IS_LOGO(logo));
    g_object_set((GObject *) logo, "screen-dress", dress, NULL);
}

/**
 * adg_logo_get_screen_dress:
 * @logo: an #AdgLogo
 *
 * Gets the line dress to be used in stroking the screen shape of @logo.
 *
 * Returns: the requested line dress
 **/
AdgDress
adg_logo_get_screen_dress(AdgLogo *logo)
{
    AdgLogoPrivate *data;

    g_return_val_if_fail(ADG_IS_LOGO(logo), ADG_DRESS_UNDEFINED);

    data = logo->data;

    return data->screen_dress;
}

/**
 * adg_logo_set_frame_dress:
 * @logo: an #AdgLogo
 * @dress: the new #AdgDress to use
 *
 * Sets a new line dress for rendering the frame of @logo.
 * The new dress must be a line dress: the check is done by
 * calling adg_dress_are_related() with @dress and the old
 * dress as arguments. Check out its documentation for
 * further details.
 *
 * The default dress is a transparent line dress: the rendering
 * callback will stroke the frame using the default color with
 * a predefined thickness.
 **/
void
adg_logo_set_frame_dress(AdgLogo *logo, AdgDress dress)
{
    g_return_if_fail(ADG_IS_LOGO(logo));
    g_object_set((GObject *) logo, "frame-dress", dress, NULL);
}

/**
 * adg_logo_get_frame_dress:
 * @logo: an #AdgLogo
 *
 * Gets the line dress to be used in stroking the frame of @logo.
 *
 * Returns: the requested line dress
 **/
AdgDress
adg_logo_get_frame_dress(AdgLogo *logo)
{
    AdgLogoPrivate *data;

    g_return_val_if_fail(ADG_IS_LOGO(logo), ADG_DRESS_UNDEFINED);

    data = logo->data;

    return data->frame_dress;
}


static void
_adg_arrange(AdgEntity *entity)
{
    AdgLogoClass *logo_class;
    AdgLogoClassPrivate *data_class;
    CpmlExtents extents;

    logo_class = ADG_LOGO_GET_CLASS(entity);
    data_class = logo_class->data_class;

    _adg_arrange_class(logo_class);
    cpml_extents_copy(&extents, &data_class->extents);

    cpml_extents_transform(&extents, adg_entity_get_local_matrix(entity));
    cpml_extents_transform(&extents, adg_entity_get_global_matrix(entity));
    adg_entity_set_extents(entity, &extents);
}

static void
_adg_arrange_class(AdgLogoClass *logo_class)
{
    AdgLogoClassPrivate *data_class;
    CpmlExtents *extents;

    data_class = logo_class->data_class;
    extents = &data_class->extents;

    if (data_class->symbol == NULL) {
        AdgPath *path = adg_path_new();

        adg_path_move_to_explicit(path, 8, 26);
        adg_path_line_to_explicit(path, 24, 10);
        adg_path_arc_to_explicit(path, 32, 18, 24, 26);
        adg_path_line_to_explicit(path, 24, 11);

        adg_path_move_to_explicit(path, 40, 10);
        adg_path_arc_to_explicit(path, 32, 18, 40, 26);
        adg_path_line_to_explicit(path, 40, 22);
        adg_path_line_to_explicit(path, 38, 22);

        adg_path_move_to_explicit(path, 23, 22);
        adg_path_line_to_explicit(path, 16, 22);

        data_class->symbol = path;
        extents->is_defined = FALSE;
    }

    if (data_class->screen == NULL) {
        AdgPath *path = adg_path_new();

        adg_path_move_to_explicit(path, 3, 3);
        adg_path_line_to_explicit(path, 47, 3);
        adg_path_fillet(path, 10);
        adg_path_line_to_explicit(path, 47, 33);
        adg_path_fillet(path, 10);
        adg_path_line_to_explicit(path, 3, 33);
        adg_path_fillet(path, 10);
        adg_path_close(path);
        adg_path_fillet(path, 10);

        data_class->screen = path;
        extents->is_defined = FALSE;
    }

    if (data_class->frame == NULL) {
        AdgPath *path = adg_path_new();

        adg_path_move_to_explicit(path, 0, 0);
        adg_path_line_to_explicit(path, 50, 0);
        adg_path_line_to_explicit(path, 50, 36);
        adg_path_line_to_explicit(path, 0, 36);
        adg_path_close(path);

        data_class->frame = path;
        extents->is_defined = FALSE;
    }

    if (!data_class->extents.is_defined) {
        cpml_extents_add(extents,
                         adg_trail_get_extents((AdgTrail *) data_class->symbol));
        cpml_extents_add(extents,
                         adg_trail_get_extents((AdgTrail *) data_class->screen));
        cpml_extents_add(extents,
                         adg_trail_get_extents((AdgTrail *) data_class->frame));
    }
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgLogoClassPrivate *data_class;
    AdgLogoPrivate *data;
    const cairo_path_t *cairo_path;

    data_class = ADG_LOGO_GET_CLASS(entity)->data_class;
    data = ((AdgLogo *) entity)->data;

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    cairo_path = adg_trail_get_cairo_path((AdgTrail *) data_class->symbol);
    if (cairo_path != NULL) {
        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        cairo_set_line_width(cr, 3);
        adg_entity_apply_dress(entity, data->symbol_dress, cr);

        cairo_stroke(cr);
    }

    cairo_path = adg_trail_get_cairo_path((AdgTrail *) data_class->screen);
    if (cairo_path != NULL) {
        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        cairo_set_line_width(cr, 2);
        adg_entity_apply_dress(entity, data->screen_dress, cr);

        cairo_stroke(cr);
    }

    cairo_path = adg_trail_get_cairo_path((AdgTrail *) data_class->frame);
    if (cairo_path != NULL) {
        cairo_save(cr);
        cairo_transform(cr, adg_entity_get_local_matrix(entity));
        cairo_append_path(cr, cairo_path);
        cairo_restore(cr);

        cairo_set_line_width(cr, 2);
        cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
        adg_entity_apply_dress(entity, data->frame_dress, cr);

        cairo_stroke(cr);
    }
}
