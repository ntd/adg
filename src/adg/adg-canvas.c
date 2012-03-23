/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-canvas
 * @short_description: The drawing container
 *
 * The canvas is the toplevel entity of an ADG drawing. It can be
 * bound to a GTK+ widget, such as #AdgGtkArea, or manually rendered
 * to a custom surface.
 *
 * Tipically, the canvas contains the description and properties of
 * the media used, such as such as size (if relevant), margins,
 * border and paddings. This approach clearly follows the block model
 * of the CSS specifications level 2.
 *
 * The paddings specify the distance between the entities contained
 * by the canvas and the border. The margins specify the distance
 * between the canvas border and the media extents.
 *
 * The canvas (hence the media) size can be explicitely specified
 * by directly writing to the #AdgCanvas:size property or using any
 * valid setter, such as adg_canvas_set_size(),
 * adg_canvas_set_size_explicit() or the convenient
 * adg_canvas_set_paper() GTK+ wrapper. You can also set explicitely
 * only one dimension and let the other one be computed automatically.
 * This is done by using the special value %0 that specifies a side
 * must be autocalculated.
 *
 * By default either width and height must be autocalculated (are
 * set to %0), so the arrange() phase on the canvas is performed.
 * Margins and paddings are then added to the extents to get the
 * border coordinates and the final bounding box.
 *
 * When the size is explicitely set, instead, the final bounding
 * box is forcibly set to this value without taking the canvas
 * extents into account. The margins are then subtracted to get
 * the coordinates of the border. In this case, the paddings are
 * simply ignored.
 *
 * Since: 1.0
 **/

/**
 * AdgCanvas:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#if GTK3_ENABLED || GTK2_ENABLED
#include <gtk/gtk.h>
#endif

#include "adg-container.h"
#include "adg-table.h"
#include "adg-title-block.h"
#include "adg-style.h"
#include "adg-color-style.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"

#include "adg-canvas.h"
#include "adg-canvas-private.h"


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_canvas_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_canvas_parent_class)


G_DEFINE_TYPE(AdgCanvas, adg_canvas, ADG_TYPE_CONTAINER)

enum {
    PROP_0,
    PROP_SIZE,
    PROP_BACKGROUND_DRESS,
    PROP_FRAME_DRESS,
    PROP_TITLE_BLOCK,
    PROP_TOP_MARGIN,
    PROP_RIGHT_MARGIN,
    PROP_BOTTOM_MARGIN,
    PROP_LEFT_MARGIN,
    PROP_HAS_FRAME,
    PROP_TOP_PADDING,
    PROP_RIGHT_PADDING,
    PROP_BOTTOM_PADDING,
    PROP_LEFT_PADDING
};


static void             _adg_dispose            (GObject        *object);
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


static void
adg_canvas_class_init(AdgCanvasClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgCanvasPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->global_changed = _adg_global_changed;
    entity_class->local_changed = _adg_local_changed;
    entity_class->invalidate = _adg_invalidate;
    entity_class->arrange = _adg_arrange;
    entity_class->render = _adg_render;

    param = g_param_spec_boxed("size",
                               P_("Canvas Size"),
                               P_("The size set on this canvas: use 0 to have an automatic dimension based on the canvas extents"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SIZE, param);

    param = adg_param_spec_dress("background-dress",
                                 P_("Background Dress"),
                                 P_("The color dress to use for the canvas background"),
                                 ADG_DRESS_COLOR_BACKGROUND,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BACKGROUND_DRESS, param);

    param = adg_param_spec_dress("frame-dress",
                                 P_("Frame Dress"),
                                 P_("Line dress to use while drawing the frame around the canvas"),
                                 ADG_DRESS_LINE_FRAME,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FRAME_DRESS, param);

    param = g_param_spec_object("title-block",
                                P_("Title Block"),
                                P_("The title block to assign to this canvas"),
                                ADG_TYPE_TITLE_BLOCK,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TITLE_BLOCK, param);

    param = g_param_spec_double("top-margin",
                                P_("Top Margin"),
                                P_("The margin (in global space) to leave above the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOP_MARGIN, param);

    param = g_param_spec_double("right-margin",
                                P_("Right Margin"),
                                P_("The margin (in global space) to leave empty at the right of the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RIGHT_MARGIN, param);

    param = g_param_spec_double("bottom-margin",
                                P_("Bottom Margin"),
                                P_("The margin (in global space) to leave empty below the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BOTTOM_MARGIN, param);

    param = g_param_spec_double("left-margin",
                                P_("Left Margin"),
                                P_("The margin (in global space) to leave empty at the left of the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LEFT_MARGIN, param);

    param = g_param_spec_boolean("has-frame",
                                 P_("Has Frame Flag"),
                                 P_("If enabled, a frame using the frame dress will be drawn around the canvas extents, taking into account the margins"),
                                 TRUE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HAS_FRAME, param);

    param = g_param_spec_double("top-padding",
                                P_("Top Padding"),
                                P_("The padding (in global space) to leave empty above between the drawing and the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOP_PADDING, param);

    param = g_param_spec_double("right-padding",
                                P_("Right Padding"),
                                P_("The padding (in global space) to leave empty at the right between the drawing and the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RIGHT_PADDING, param);

    param = g_param_spec_double("bottom-padding",
                                P_("Bottom Padding"),
                                P_("The padding (in global space) to leave empty below between the drawing and the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BOTTOM_PADDING, param);

    param = g_param_spec_double("left-padding",
                                P_("Left Padding"),
                                P_("The padding (in global space) to leave empty at the left between the drawing and the frame"),
                                -G_MAXDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LEFT_PADDING, param);
}

static void
adg_canvas_init(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(canvas,
                                                         ADG_TYPE_CANVAS,
                                                         AdgCanvasPrivate);

    data->size.x = 0;
    data->size.y = 0;
    data->background_dress = ADG_DRESS_COLOR_BACKGROUND;
    data->frame_dress = ADG_DRESS_LINE_FRAME;
    data->title_block = NULL;
    data->top_margin = 15;
    data->right_margin = 15;
    data->bottom_margin = 15;
    data->left_margin = 15;
    data->has_frame = TRUE;
    data->top_padding = 15;
    data->right_padding = 15;
    data->bottom_padding = 15;
    data->left_padding = 15;

    canvas->data = data;
}

static void
_adg_dispose(GObject *object)
{
    AdgCanvasPrivate *data = ((AdgCanvas *) object)->data;

    if (data->title_block) {
        g_object_unref(data->title_block);
        data->title_block = NULL;
    }

    if (_ADG_OLD_OBJECT_CLASS->dispose)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}


static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgCanvasPrivate *data = ((AdgCanvas *) object)->data;

    switch (prop_id) {
    case PROP_SIZE:
        g_value_set_boxed(value, &data->size);
        break;
    case PROP_BACKGROUND_DRESS:
        g_value_set_int(value, data->background_dress);
        break;
    case PROP_FRAME_DRESS:
        g_value_set_int(value, data->frame_dress);
        break;
    case PROP_TITLE_BLOCK:
        g_value_set_object(value, data->title_block);
        break;
    case PROP_TOP_MARGIN:
        g_value_set_double(value, data->top_margin);
        break;
    case PROP_RIGHT_MARGIN:
        g_value_set_double(value, data->right_margin);
        break;
    case PROP_BOTTOM_MARGIN:
        g_value_set_double(value, data->bottom_margin);
        break;
    case PROP_LEFT_MARGIN:
        g_value_set_double(value, data->left_margin);
        break;
    case PROP_HAS_FRAME:
        g_value_set_boolean(value, data->has_frame);
        break;
    case PROP_TOP_PADDING:
        g_value_set_double(value, data->top_padding);
        break;
    case PROP_RIGHT_PADDING:
        g_value_set_double(value, data->right_padding);
        break;
    case PROP_BOTTOM_PADDING:
        g_value_set_double(value, data->bottom_padding);
        break;
    case PROP_LEFT_PADDING:
        g_value_set_double(value, data->left_padding);
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
    AdgCanvas *canvas;
    AdgCanvasPrivate *data;
    AdgTitleBlock *title_block;

    canvas = (AdgCanvas *) object;
    data = canvas->data;

    switch (prop_id) {
    case PROP_SIZE:
        adg_pair_copy(&data->size, g_value_get_boxed(value));
        break;
    case PROP_BACKGROUND_DRESS:
        data->background_dress = g_value_get_int(value);
        break;
    case PROP_FRAME_DRESS:
        data->frame_dress = g_value_get_int(value);
        break;
    case PROP_TITLE_BLOCK:
        title_block = g_value_get_object(value);
        if (title_block) {
            g_object_ref(title_block);
            adg_entity_set_parent((AdgEntity *) title_block,
                                  (AdgEntity *) canvas);
        }
        if (data->title_block)
            g_object_unref(data->title_block);
        data->title_block = title_block;
        break;
    case PROP_TOP_MARGIN:
        data->top_margin = g_value_get_double(value);
        break;
    case PROP_RIGHT_MARGIN:
        data->right_margin = g_value_get_double(value);
        break;
    case PROP_BOTTOM_MARGIN:
        data->bottom_margin = g_value_get_double(value);
        break;
    case PROP_LEFT_MARGIN:
        data->left_margin = g_value_get_double(value);
        break;
    case PROP_HAS_FRAME:
        data->has_frame = g_value_get_boolean(value);
        break;
    case PROP_TOP_PADDING:
        data->top_padding = g_value_get_double(value);
        break;
    case PROP_RIGHT_PADDING:
        data->right_padding = g_value_get_double(value);
        break;
    case PROP_BOTTOM_PADDING:
        data->bottom_padding = g_value_get_double(value);
        break;
    case PROP_LEFT_PADDING:
        data->left_padding = g_value_get_double(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_canvas_new:
 *
 * Creates a new empty canvas object.
 *
 * Returns: (transfer full): the newly created canvas.
 *
 * Since: 1.0
 **/
AdgCanvas *
adg_canvas_new(void)
{
    return g_object_new(ADG_TYPE_CANVAS, NULL);
}

/**
 * adg_canvas_set_size:
 * @canvas: an #AdgCanvas
 * @size: the new size for the canvas
 *
 * Sets a specific size on @canvas. The x and/or y
 * components of the returned #AdgPair could be %0, in which
 * case the size returned by adg_entity_get_extents() on
 * @canvas will be used instead.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_size(AdgCanvas *canvas, const AdgPair *size)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_return_if_fail(size != NULL);

    g_object_set(canvas, "size", size, NULL);
}

/**
 * adg_canvas_set_size_explicit:
 * @canvas: an #AdgCanvas
 * @x: the new width of the canvas or %0 to reset
 * @y: the new height of the canvas or %0 to reset
 *
 * A convenient function to set the size of @canvas using
 * explicit coordinates. Check adg_canvas_set_size() for
 * further details.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_size_explicit(AdgCanvas *canvas, gdouble x, gdouble y)
{
    AdgPair size;

    size.x = x;
    size.y = y;

    adg_canvas_set_size(canvas, &size);
}

/**
 * adg_canvas_get_size:
 * @canvas: an #AdgCanvas
 *
 * Gets the specific size set on @canvas. The x and/or y
 * components of the returned #AdgPair could be %0, in which
 * case the size returned by adg_entity_get_extents() on
 * @canvas will be used instead.
 *
 * Returns: the explicit size set on this canvas or %NULL on errors.
 *
 * Since: 1.0
 **/
const AdgPair *
adg_canvas_get_size(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), NULL);

    data = canvas->data;
    return &data->size;
}

/**
 * adg_canvas_set_background_dress:
 * @canvas: an #AdgCanvas
 * @dress: the new #AdgDress to use
 *
 * Sets a new background dress for rendering @canvas: the new
 * dress must be a color dress.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_background_dress(AdgCanvas *canvas, AdgDress dress)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "background-dress", dress, NULL);
}

/**
 * adg_canvas_get_background_dress:
 * @canvas: an #AdgCanvas
 *
 * Gets the background dress to be used in rendering @canvas.
 *
 * Returns: (transfer none): the current background dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_canvas_get_background_dress(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), ADG_DRESS_UNDEFINED);

    data = canvas->data;

    return data->background_dress;
}

/**
 * adg_canvas_set_frame_dress:
 * @canvas: an #AdgCanvas
 * @dress: the new #AdgDress to use
 *
 * Sets the #AdgCanvas:frame-dress property of @canvas to @dress:
 * the new dress must be a line dress.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_frame_dress(AdgCanvas *canvas, AdgDress dress)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "frame-dress", dress, NULL);
}

/**
 * adg_canvas_get_frame_dress:
 * @canvas: an #AdgCanvas
 *
 * Gets the frame dress to be used in rendering the border of @canvas.
 *
 * Returns: (transfer none): the current frame dress.
 *
 * Since: 1.0
 **/
AdgDress
adg_canvas_get_frame_dress(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), ADG_DRESS_UNDEFINED);

    data = canvas->data;
    return data->frame_dress;
}

/**
 * adg_canvas_set_title_block:
 * @canvas: an #AdgCanvas
 * @title_block: a title block
 *
 * Sets the #AdgCanvas:title-block property of @canvas to @title_block.
 *
 * Although a title block entity could be added to @canvas in the usual
 * way, that is using the adg_container_add() method, assigning a title
 * block with adg_canvas_set_title_block() is somewhat different:
 *
 * - @title_block will be automatically attached to the bottom right
 *   corner of to the @canvas frame (this could be accomplished in the
 *   usual way too, by resetting the right and bottom paddings);
 * - the @title_block boundary box is not taken into account while
 *   computing the extents of @canvas.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_title_block(AdgCanvas *canvas, AdgTitleBlock *title_block)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_return_if_fail(title_block == NULL || ADG_IS_TITLE_BLOCK(title_block));
    g_object_set(canvas, "title-block", title_block, NULL);
}

/**
 * adg_canvas_get_title_block:
 * @canvas: an #AdgCanvas
 *
 * Gets the #AdgTitleBlock object of @canvas: check
 * adg_canvas_set_title_block() for details.
 *
 * The returned entity is owned by @canvas and should not be
 * modified or freed.
 *
 * Returns: (transfer none): the title block object or %NULL.
 *
 * Since: 1.0
 **/
AdgTitleBlock *
adg_canvas_get_title_block(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), NULL);

    data = canvas->data;
    return data->title_block;
}

/**
 * adg_canvas_set_top_margin:
 * @canvas: an #AdgCanvas
 * @value: the new margin, in global space
 *
 * Changes the top margin of @canvas by setting #AdgCanvas:top-margin
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_top_margin(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "top-margin", value, NULL);
}

/**
 * adg_canvas_get_top_margin:
 * @canvas: an #AdgCanvas
 *
 * Gets the top margin (in global space) of @canvas.
 *
 * Returns: the requested margin or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_top_margin(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->top_margin;
}

/**
 * adg_canvas_set_right_margin:
 * @canvas: an #AdgCanvas
 * @value: the new margin, in global space
 *
 * Changes the right margin of @canvas by setting #AdgCanvas:right-margin
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_right_margin(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "right-margin", value, NULL);
}

/**
 * adg_canvas_get_right_margin:
 * @canvas: an #AdgCanvas
 *
 * Gets the right margin (in global space) of @canvas.
 *
 * Returns: the requested margin or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_right_margin(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->right_margin;
}


/**
 * adg_canvas_set_bottom_margin:
 * @canvas: an #AdgCanvas
 * @value: the new margin, in global space
 *
 * Changes the bottom margin of @canvas by setting #AdgCanvas:bottom-margin
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_bottom_margin(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "bottom-margin", value, NULL);
}

/**
 * adg_canvas_get_bottom_margin:
 * @canvas: an #AdgCanvas
 *
 * Gets the bottom margin (in global space) of @canvas.
 *
 * Returns: the requested margin or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_bottom_margin(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->bottom_margin;
}

/**
 * adg_canvas_set_left_margin:
 * @canvas: an #AdgCanvas
 * @value: the new margin, in global space
 *
 * Changes the left margin of @canvas by setting #AdgCanvas:left-margin
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_left_margin(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "left-margin", value, NULL);
}

/**
 * adg_canvas_get_left_margin:
 * @canvas: an #AdgCanvas
 *
 * Gets the left margin (in global space) of @canvas.
 *
 * Returns: the requested margin or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_left_margin(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->left_margin;
}

/**
 * adg_canvas_set_margins:
 * @canvas: an #AdgCanvas
 * @top: top margin, in global space
 * @right: right margin, in global space
 * @bottom: bottom margin, in global space
 * @left: left margin, in global space
 *
 * Convenient function to set all the margins at once.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_margins(AdgCanvas *canvas, gdouble top, gdouble right,
                       gdouble bottom, gdouble left)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "top-margin", top, "right-margin", right,
                 "bottom-margin", bottom, "left-margin", left, NULL);
}

/**
 * adg_canvas_apply_margins:
 * @canvas: an #AdgCanvas
 * @extents: where apply the margins
 *
 * A convenient function to apply the margins of @canvas to the
 * arbitrary #CpmlExtents struct @extents.
 *
 * Since: 1.0
 **/
void
adg_canvas_apply_margins(AdgCanvas *canvas, CpmlExtents *extents)
{
    AdgCanvasPrivate *data;

    g_return_if_fail(ADG_IS_CANVAS(canvas));

    data = canvas->data;

    extents->org.x -= data->left_margin;
    extents->org.y -= data->top_margin;
    extents->size.x += data->left_margin + data->right_margin;
    extents->size.y += data->top_margin + data->bottom_margin;
}

/**
 * adg_canvas_switch_frame:
 * @canvas: an #AdgCanvas
 * @new_state: the new flag status
 *
 * Sets a new status on the #AdgCanvas:has-frame property: %TRUE
 * means a border around the canvas extents (less the margins)
 * should be rendered.
 *
 * Since: 1.0
 **/
void
adg_canvas_switch_frame(AdgCanvas *canvas, gboolean new_state)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "has-frame", new_state, NULL);
}

/**
 * adg_canvas_has_frame:
 * @canvas: an #AdgCanvas
 *
 * Gets the current status of the #AdgCanvas:has-frame property,
 * that is whether a border around the canvas extents (less the
 * margins) should be rendered (%TRUE) or not (%FALSE).
 *
 * Returns: the current status of the frame flag.
 *
 * Since: 1.0
 **/
gboolean
adg_canvas_has_frame(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), FALSE);

    data = canvas->data;
    return data->has_frame;
}

/**
 * adg_canvas_set_top_padding:
 * @canvas: an #AdgCanvas
 * @value: the new padding, in global space
 *
 * Changes the top padding of @canvas by setting #AdgCanvas:top-padding
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_top_padding(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "top-padding", value, NULL);
}

/**
 * adg_canvas_get_top_padding:
 * @canvas: an #AdgCanvas
 *
 * Gets the top padding (in global space) of @canvas.
 *
 * Returns: the requested padding or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_top_padding(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->top_padding;
}

/**
 * adg_canvas_set_right_padding:
 * @canvas: an #AdgCanvas
 * @value: the new padding, in global space
 *
 * Changes the right padding of @canvas by setting #AdgCanvas:right-padding
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_right_padding(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "right-padding", value, NULL);
}

/**
 * adg_canvas_get_right_padding:
 * @canvas: an #AdgCanvas
 *
 * Gets the right padding (in global space) of @canvas.
 *
 * Returns: the requested padding or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_right_padding(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->right_padding;
}


/**
 * adg_canvas_set_bottom_padding:
 * @canvas: an #AdgCanvas
 * @value: the new padding, in global space
 *
 * Changes the bottom padding of @canvas by setting #AdgCanvas:bottom-padding
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_bottom_padding(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "bottom-padding", value, NULL);
}

/**
 * adg_canvas_get_bottom_padding:
 * @canvas: an #AdgCanvas
 *
 * Gets the bottom padding (in global space) of @canvas.
 *
 * Returns: the requested padding or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_bottom_padding(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->bottom_padding;
}

/**
 * adg_canvas_set_left_padding:
 * @canvas: an #AdgCanvas
 * @value: the new padding, in global space
 *
 * Changes the left padding of @canvas by setting #AdgCanvas:left-padding
 * to @value. Negative values are allowed.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_left_padding(AdgCanvas *canvas, gdouble value)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "left-padding", value, NULL);
}

/**
 * adg_canvas_get_left_padding:
 * @canvas: an #AdgCanvas
 *
 * Gets the left padding (in global space) of @canvas.
 *
 * Returns: the requested padding or %0 on error.
 *
 * Since: 1.0
 **/
gdouble
adg_canvas_get_left_padding(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), 0.);

    data = canvas->data;
    return data->left_padding;
}

/**
 * adg_canvas_set_paddings:
 * @canvas: an #AdgCanvas
 * @top: top padding, in global space
 * @right: right padding, in global space
 * @bottom: bottom padding, in global space
 * @left: left padding, in global space
 *
 * Convenient function to set all the paddings at once.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_paddings(AdgCanvas *canvas, gdouble top, gdouble right,
                          gdouble bottom, gdouble left)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "top-padding", top, "right-padding", right,
                 "bottom-padding", bottom, "left-padding", left, NULL);
}


static void
_adg_global_changed(AdgEntity *entity)
{
    AdgCanvasPrivate *data = ((AdgCanvas *) entity)->data;

    if (_ADG_OLD_ENTITY_CLASS->global_changed)
        _ADG_OLD_ENTITY_CLASS->global_changed(entity);

    if (data->title_block)
        adg_entity_global_changed((AdgEntity *) data->title_block);
}

static void
_adg_local_changed(AdgEntity *entity)
{
    AdgCanvasPrivate *data = ((AdgCanvas *) entity)->data;

    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);

    if (data->title_block)
        adg_entity_local_changed((AdgEntity *) data->title_block);
}

static void
_adg_invalidate(AdgEntity *entity)
{
    AdgCanvasPrivate *data = ((AdgCanvas *) entity)->data;

    if (_ADG_OLD_ENTITY_CLASS->invalidate)
        _ADG_OLD_ENTITY_CLASS->invalidate(entity);

    if (data->title_block)
        adg_entity_invalidate((AdgEntity *) data->title_block);
}

static void
_adg_arrange(AdgEntity *entity)
{
    AdgCanvasPrivate *data;
    CpmlExtents extents;

    if (_ADG_OLD_ENTITY_CLASS->arrange)
        _ADG_OLD_ENTITY_CLASS->arrange(entity);

    cpml_extents_copy(&extents, adg_entity_get_extents(entity));

    /* The extents should be defined, otherwise there is no drawing */
    g_return_if_fail(extents.is_defined);

    data = ((AdgCanvas *) entity)->data;

    if (data->size.x > 0 || data->size.y > 0) {
        const AdgMatrix *global = adg_entity_get_global_matrix(entity);
        CpmlExtents paper;

        paper.org.x = 0;
        paper.org.y = 0;
        paper.size.x = data->size.x;
        paper.size.y = data->size.y;

        cairo_matrix_transform_point(global, &paper.org.x, &paper.org.y);
        cairo_matrix_transform_distance(global, &paper.size.x, &paper.size.y);

        if (data->size.x > 0) {
            extents.org.x = paper.org.x;
            extents.size.x = paper.size.x;
        }
        if (data->size.y > 0) {
            extents.org.y = paper.org.y;
            extents.size.y = paper.size.y;
        }
    }

    if (data->size.x == 0) {
        extents.org.x -= data->left_padding;
        extents.size.x += data->left_padding + data->right_padding;
    }
    if (data->size.y == 0) {
        extents.org.y -= data->top_padding;
        extents.size.y += data->top_padding + data->bottom_padding;
    }

    /* Impose the new extents */
    adg_entity_set_extents(entity, &extents);

    if (data->title_block) {
        AdgEntity *title_block_entity;
        const CpmlExtents *title_block_extents;
        AdgPair shift;

        title_block_entity = (AdgEntity *) data->title_block;
        adg_entity_arrange(title_block_entity);
        title_block_extents = adg_entity_get_extents(title_block_entity);

        shift.x = extents.org.x + extents.size.x - title_block_extents->org.x
            - title_block_extents->size.x;
        shift.y = extents.org.y + extents.size.y - title_block_extents->org.y
            - title_block_extents->size.y;

        /* The following block could be optimized by skipping tiny shift,
         * usually left by rounding errors */
        if (shift.x != 0 || shift.y != 0) {
            AdgMatrix unglobal, map;
            adg_matrix_copy(&unglobal, adg_entity_get_global_matrix(entity));
            cairo_matrix_invert(&unglobal);

            cairo_matrix_transform_distance(&unglobal, &shift.x, &shift.y);
            cairo_matrix_init_translate(&map, shift.x, shift.y);
            adg_entity_transform_global_map(title_block_entity, &map,
                                            ADG_TRANSFORM_AFTER);

            adg_entity_global_changed(title_block_entity);
            adg_entity_arrange(title_block_entity);
        }
    }
}

static void
_adg_render(AdgEntity *entity, cairo_t *cr)
{
    AdgCanvasPrivate *data;
    const CpmlExtents *extents;

    data = ((AdgCanvas *) entity)->data;
    extents = adg_entity_get_extents(entity);

    cairo_save(cr);

    /* Background fill */
    cairo_rectangle(cr, extents->org.x - data->left_margin,
                    extents->org.y - data->top_margin,
                    extents->size.x + data->left_margin + data->right_margin,
                    extents->size.y + data->top_margin + data->bottom_margin);
    adg_entity_apply_dress(entity, data->background_dress, cr);
    cairo_fill(cr);

    /* Frame line */
    if (data->has_frame) {
        cairo_rectangle(cr, extents->org.x, extents->org.y,
                        extents->size.x, extents->size.y);
        cairo_transform(cr, adg_entity_get_global_matrix(entity));
        adg_entity_apply_dress(entity, data->frame_dress, cr);
        cairo_stroke(cr);
    }

    cairo_restore(cr);

    if (data->title_block)
        adg_entity_render((AdgEntity *) data->title_block, cr);

    if (_ADG_OLD_ENTITY_CLASS->render)
        _ADG_OLD_ENTITY_CLASS->render(entity, cr);
}

#if GTK3_ENABLED || GTK2_ENABLED

/**
 * adg_canvas_set_paper:
 * @canvas: an #AdgCanvas
 * @paper_name: a paper name
 * @orientation: the page orientation
 *
 * A convenient function to set the size of @canvas using a
 * @paper_name and an @orientation value. This should be a
 * PWG 5101.1-2002 paper name and it will be passed as is to
 * gtk_paper_size_new(), so use any valid name accepted by
 * that function.
 *
 * To reset this size, you could use adg_canvas_set_size() with a
 * %NULL size: in this way the size will match the boundary boxes
 * of the entities contained by the canvas.
 *
 * Furthermore, the margins will be set to their default values,
 * that is the margins returned by the #GtkPaperSize API.
 * If you want to use your own margins on a named paper size,
 * set them <emphasis>after</emphasis> the call to this function.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_paper(AdgCanvas *canvas, const gchar *paper_name,
                     GtkPageOrientation orientation)
{
    GtkPageSetup *page_setup;
    GtkPaperSize *paper_size;

    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_return_if_fail(paper_name != NULL);

    page_setup = gtk_page_setup_new();
    paper_size = gtk_paper_size_new(paper_name);

    gtk_page_setup_set_paper_size(page_setup, paper_size);
    gtk_page_setup_set_orientation(page_setup, orientation);
    gtk_paper_size_free(paper_size);

    adg_canvas_set_page_setup(canvas, page_setup);
    g_object_unref(page_setup);
}

/**
 * adg_canvas_set_page_setup:
 * @canvas: an #AdgCanvas
 * @page_setup: the page setup
 *
 * A convenient function to setup the page of @canvas so it can
 * also be subsequentially used for printing. It is allowed to
 * pass %NULL for @page_setup to unset the setup data from @canvas.
 *
 * A reference to @page_setup is added, so there is no need to keep
 * alive this object outside this function. The @page_setup pointer
 * is stored in the associative key %_adg_page_setup and can be
 * retrieved at any time with:
 *
 * |[
 * page_setup = g_object_get_data(G_OBJECT(canvas), "_adg_page_setup");
 * ]|
 *
 * The size and margins provided by @page_setup are used to set the
 * size and margins of @canvas much in the same way as what
 * adg_canvas_set_paper() does. This means if you set a page and
 * then unset it, the canvas will retain size and margins of the
 * original page although @page_setup will not be used for printing.
 * You must unset the size with adg_canvas_set_size() with a %NULL size.
 *
 * |[
 * // By default, canvas does not have an explicit size
 * adg_canvas_set_page_setup(canvas, a4);
 * // Here canvas has the size and margins specified by a4
 * adg_canvas_set_page_setup(canvas, NULL);
 * // Now the only difference is that canvas is no more bound
 * // to the a4 page setup, so the following will return NULL:
 * page_setup = g_object_get_data(G_OBJECT(canvas), "_adg_page_setup");
 * // To restore the original status and have an autocomputed size:
 * adg_canvas_set_size(canvas, NULL);
 * ]|
 *
 * Since: 1.0
 **/
void
adg_canvas_set_page_setup(AdgCanvas *canvas, GtkPageSetup *page_setup)
{
    gdouble top, right, bottom, left;
    AdgPair size;

    g_return_if_fail(ADG_IS_CANVAS(canvas));

    if (page_setup == NULL) {
        /* By convention, NULL resets the default page but
         * does not change any other property */
        g_object_set_data((GObject *) canvas, "_adg_page_setup", NULL);
        return;
    }

    g_return_if_fail(GTK_IS_PAGE_SETUP(page_setup));

    top = gtk_page_setup_get_top_margin(page_setup, GTK_UNIT_POINTS);
    right = gtk_page_setup_get_right_margin(page_setup, GTK_UNIT_POINTS);
    bottom = gtk_page_setup_get_bottom_margin(page_setup, GTK_UNIT_POINTS);
    left = gtk_page_setup_get_left_margin(page_setup, GTK_UNIT_POINTS);
    size.x = gtk_page_setup_get_page_width(page_setup, GTK_UNIT_POINTS);
    size.y = gtk_page_setup_get_page_height(page_setup, GTK_UNIT_POINTS);

    adg_canvas_set_size(canvas, &size);
    adg_canvas_set_margins(canvas, top, right, bottom, left);

    g_object_ref(page_setup);
    g_object_set_data_full((GObject *) canvas, "_adg_page_setup",
                           page_setup, g_object_unref);
}

#endif
