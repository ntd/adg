/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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
 * of the CSS specification.
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
 * This can be done by setting it to <constant>0</constant>.
 *
 * By default either width and height are autocalculated, i.e. they
 * are initially set to 0. In this case the arrange() phase is executed.
 * Margins and paddings are then added to the extents to get the
 * border coordinates and the final bounding box.
 *
 * Instead, when the size is explicitely set, the final bounding
 * box is forcibly set to this value without taking the canvas
 * extents into account. The margins are then subtracted to get
 * the coordinates of the border. In this case the paddings are
 * simply ignoredby the arrange phase. They are still used by
 * adg_canvas_autoscale() though, if called.
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

/**
 * ADG_CANVAS_ERROR:
 *
 * Error domain for canvas processing. Errors in this domain will be from the
 * #AdgCanvasError enumeration. See #GError for information on error domains.
 *
 * Since: 1.0
 **/

/**
 * AdgCanvasError:
 * @ADG_CANVAS_ERROR_SURFACE: Invalid surface type.
 * @ADG_CANVAS_ERROR_CAIRO: The underlying cairo library returned an error.
 *
 * Error codes returned by #AdgCanvas methods.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-container.h"
#include "adg-table.h"
#include "adg-title-block.h"
#include "adg-style.h"
#include "adg-color-style.h"
#include "adg-dress.h"
#include "adg-param-dress.h"

#include <adg-canvas.h>
#include "adg-canvas-private.h"

#ifdef CAIRO_HAS_PS_SURFACE
#include <cairo-ps.h>
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
#include <cairo-pdf.h>
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
#include <cairo-svg.h>
#endif


#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_canvas_parent_class)
#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_canvas_parent_class)


G_DEFINE_TYPE(AdgCanvas, adg_canvas, ADG_TYPE_CONTAINER)

enum {
    PROP_0,
    PROP_SIZE,
    PROP_SCALES,
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
static void             _adg_apply_paddings     (AdgCanvas      *canvas,
                                                 CpmlExtents    *extents);
static void             _adg_update_margin      (AdgCanvas      *canvas,
                                                 gdouble        *margin,
                                                 gdouble        *side,
                                                 gdouble         new_margin);


GQuark
adg_canvas_error_quark(void)
{
  static GQuark q;

  if G_UNLIKELY (q == 0)
    q = g_quark_from_static_string("adg-canvas-error-quark");

  return q;
}

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
                               CPML_TYPE_PAIR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SIZE, param);

    param = g_param_spec_boxed("scales",
                               P_("Valid Scales"),
                               P_("List of scales to be tested while autoscaling the drawing"),
                               G_TYPE_STRV,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SCALES, param);

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
    const gchar *scales[] = {
        "10:1", "5:1", "3:1", "2:1", "1:1", "1:2", "1:3", "1:5", "1:10",
        NULL
    };


    data->size.x = 0;
    data->size.y = 0;
    data->scales = g_strdupv((gchar **) scales);
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
    AdgCanvas *canvas;
    AdgCanvasPrivate *data;

    canvas = (AdgCanvas *) object;
    data = canvas->data;

    if (data->title_block) {
        g_object_unref(data->title_block);
        data->title_block = NULL;
    }

    if (data->scales != NULL) {
        g_strfreev(data->scales);
        data->scales = NULL;
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
    case PROP_SCALES:
        g_value_set_boxed(value, data->scales);
        break;
    case PROP_BACKGROUND_DRESS:
        g_value_set_enum(value, data->background_dress);
        break;
    case PROP_FRAME_DRESS:
        g_value_set_enum(value, data->frame_dress);
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
        cpml_pair_copy(&data->size, g_value_get_boxed(value));
        break;
    case PROP_SCALES:
        g_strfreev(data->scales);
        data->scales = g_value_dup_boxed(value);
        break;
    case PROP_BACKGROUND_DRESS:
        data->background_dress = g_value_get_enum(value);
        break;
    case PROP_FRAME_DRESS:
        data->frame_dress = g_value_get_enum(value);
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
        _adg_update_margin(canvas, &data->top_margin, &data->size.y,
                           g_value_get_double(value));
        break;
    case PROP_RIGHT_MARGIN:
        _adg_update_margin(canvas, &data->right_margin, &data->size.x,
                           g_value_get_double(value));
        break;
    case PROP_BOTTOM_MARGIN:
        _adg_update_margin(canvas, &data->bottom_margin, &data->size.y,
                           g_value_get_double(value));
        break;
    case PROP_LEFT_MARGIN:
        _adg_update_margin(canvas, &data->left_margin, &data->size.x,
                           g_value_get_double(value));
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
 * @canvas:                an #AdgCanvas
 * @size: (transfer none): the new size for the canvas
 *
 * Sets a specific size on @canvas. The x and/or y
 * component of @size can be set to 0, in which case
 * the exact value will be autocalculated, that is the
 * size component returned by adg_entity_get_extents()
 * on @canvas will be used instead.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_size(AdgCanvas *canvas, const CpmlPair *size)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_return_if_fail(size != NULL);

    g_object_set(canvas, "size", size, NULL);
}

/**
 * adg_canvas_set_size_explicit:
 * @canvas: an #AdgCanvas
 * @x:      the new width of the canvas or 0 to reset
 * @y:      the new height of the canvas or 0 to reset
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
    CpmlPair size;

    size.x = x;
    size.y = y;

    adg_canvas_set_size(canvas, &size);
}

/**
 * adg_canvas_get_size:
 * @canvas: an #AdgCanvas
 *
 * Gets the specific size set on @canvas. The x and/or y
 * components of the returned #CpmlPair could be 0, in which
 * case the size returned by adg_entity_get_extents() on @canvas
 * will be used instead.
 *
 * Returns: (transfer none): the explicit size set on this canvas or <constant>NULL</constant> on errors.
 *
 * Since: 1.0
 **/
const CpmlPair *
adg_canvas_get_size(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), NULL);

    data = canvas->data;
    return &data->size;
}

/**
 * adg_canvas_set_scales:
 * @canvas: an #AdgCanvas
 * @...:    <constant>NULL</constant> terminated list of strings
 *
 * Sets the scales allowed by @canvas. Every scale identifies a
 * specific factor to be applied to the local matrix of @canvas.
 * When adg_canvas_autoscale() will be called, the greatest
 * scale that can render every entity inside a box of
 * #AdgCanvas:size dimensions will be applied. The drawing will
 * be centered inside that box.
 *
 * Every scale should be expressed with a string in the form of
 * "x:y", where x and y are positive integers that identifies
 * numerator an denominator of a fraction. That string itself
 * will be put into the title block when used.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_scales(AdgCanvas *canvas, ...)
{
    va_list var_args;

    va_start(var_args, canvas);
    adg_canvas_set_scales_valist(canvas, var_args);
    va_end(var_args);
}

/**
 * adg_canvas_set_scales_valist:
 * @canvas:   an #AdgCanvas
 * @var_args: <constant>NULL</constant> terminated list of strings
 *
 * Vararg variant of adg_canvas_set_scales().
 *
 * Since: 1.0
 **/
void
adg_canvas_set_scales_valist(AdgCanvas *canvas, va_list var_args)
{
    gchar **scales;
    const gchar *scale;
    gint n;

    g_return_if_fail(ADG_IS_CANVAS(canvas));

    scales = NULL;
    n = 0;
    while ((scale = va_arg(var_args, const gchar *)) != NULL) {
        ++n;
        scales = g_realloc(scales, sizeof(const gchar *) * (n + 1));
        scales[n-1] = g_strdup(scale);
        scales[n] = NULL;
    }

    g_object_set(canvas, "scales", scales, NULL);
    g_strfreev(scales);
}

/**
 * adg_canvas_set_scales_array: (rename-to adg_canvas_set_scales)
 * @canvas:                                         an #AdgCanvas
 * @scales: (array zero-terminated=1) (allow-none): <constant>NULL</constant> terminated array of scales
 *
 * Array variant of adg_canvas_set_scales().
 *
 * Since: 1.0
 **/
void
adg_canvas_set_scales_array(AdgCanvas *canvas, gchar **scales)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "scales", scales, NULL);
}

/**
 * adg_canvas_get_scales:
 * @canvas: an #AdgCanvas
 *
 * Gets the list of scales set on @canvas: check adg_canvas_set_scales()
 * to get an idea of what scales are supposed to be.
 *
 * If no scales are set, <constant>NULL</constant> is returned.
 *
 * Returns: (element-type utf8) (transfer none): the <constant>NULL</constant> terminated list of valid scales.
 *
 * Since: 1.0
 **/
gchar **
adg_canvas_get_scales(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), NULL);

    data = canvas->data;
    return data->scales;
}

/**
 * adg_canvas_autoscale:
 * @canvas: an #AdgCanvas
 *
 * Applies one scale per time, in the order they have been provided
 * in the adg_canvas_set_scales() call, until the drawing can be
 * entirely contained into the current paper. If successful, the
 * scale of the title block is changed accordingly and the drawing
 * is centered inside the paper.
 *
 * The paddings are taken into account while computing the drawing
 * extents.
 *
 * Since: 1.0
 **/
void
adg_canvas_autoscale(AdgCanvas *canvas)
{
    AdgCanvasPrivate *data;
    gchar **p_scale;
    AdgEntity *entity;
    cairo_matrix_t map;
    CpmlExtents extents;
    AdgTitleBlock *title_block;
    CpmlPair delta;

    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_return_if_fail(_ADG_OLD_ENTITY_CLASS->arrange != NULL);

    data = canvas->data;
    entity = (AdgEntity *) canvas;
    title_block = data->title_block;

    /* Manually calling the arrange() method instead of emitting the "arrange"
     * signal does not invalidate the global matrix: let's do it right now */
    adg_entity_global_changed(entity);

    for (p_scale = data->scales; p_scale != NULL && *p_scale != NULL; ++p_scale) {
        const gchar *scale = *p_scale;
        gdouble factor = adg_scale_factor(scale);
        if (factor <= 0)
            continue;

        cairo_matrix_init_scale(&map, factor, factor);
        adg_entity_set_local_map(entity, &map);
        adg_entity_local_changed(entity);

        /* Arrange the entities inside the canvas, but not the canvas itself,
         * just to get the bounding box of the drawing without the paper */
        _ADG_OLD_ENTITY_CLASS->arrange(entity);
        cpml_extents_copy(&extents, adg_entity_get_extents(entity));

        /* Just in case @canvas is emtpy */
        if (! extents.is_defined)
            return;

        _adg_apply_paddings(canvas, &extents);

        if (title_block != NULL)
            adg_title_block_set_scale(title_block, scale);

        /* Bail out if paper size is not specified or invalid */
        if (data->size.x <= 0 || data->size.y <= 0)
            break;

        /* If the drawing extents are fully contained inside the paper size,
         * center the drawing in the paper and bail out */
        delta.x = data->size.x - extents.size.x;
        delta.y = data->size.y - extents.size.y;
        if (delta.x >= 0 && delta.y >= 0) {
            cairo_matrix_t transform;
            cairo_matrix_init_translate(&transform,
                                        delta.x / 2 - extents.org.x,
                                        delta.y / 2 - extents.org.y);
            adg_entity_transform_local_map(entity, &transform,
                                           ADG_TRANSFORM_AFTER);
            break;
        }
    }
}

/**
 * adg_canvas_set_background_dress:
 * @canvas: an #AdgCanvas
 * @dress:  the new #AdgDress to use
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
 * Returns: the current background dress.
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
 * @dress:  the new #AdgDress to use
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
 * Returns: the current frame dress.
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
 * @canvas:                       an #AdgCanvas
 * @title_block: (transfer full): a title block
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
adg_canvas_set_title_block(AdgCanvas *canvas,
                           AdgTitleBlock *title_block)
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
 * Returns: (transfer none): the title block object or <constant>NULL</constant>.
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
 * @value:  the new margin, in global space
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
 * Returns: the requested margin or 0 on error.
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
 * @value:  the new margin, in global space
 *
 * Changes the right margin of @canvas by setting
 * #AdgCanvas:right-margin to @value. Negative values are allowed.
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
 * Returns: the requested margin or 0 on error.
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
 * @value:  the new margin, in global space
 *
 * Changes the bottom margin of @canvas by setting
 * #AdgCanvas:bottom-margin to @value. Negative values are allowed.
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
 * Returns: the requested margin or 0 on error.
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
 * @value:  the new margin, in global space
 *
 * Changes the left margin of @canvas by setting
 * #AdgCanvas:left-margin to @value. Negative values are allowed.
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
 * Returns: the requested margin or 0 on error.
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
 * @top:    top margin, in global space
 * @right:  right margin, in global space
 * @bottom: bottom margin, in global space
 * @left:   left margin, in global space
 *
 * Convenient function to set all the margins at once.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_margins(AdgCanvas *canvas,
                       gdouble top, gdouble right,
                       gdouble bottom, gdouble left)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_object_set(canvas, "top-margin", top, "right-margin", right,
                 "bottom-margin", bottom, "left-margin", left, NULL);
}

/**
 * adg_canvas_apply_margins:
 * @canvas:  an #AdgCanvas
 * @extents: where apply the margins
 *
 * A convenient function to apply the margins of @canvas to the
 * arbitrary #CpmlExtents struct @extents. "Apply" means @extents
 * are enlarged of the specific margin values.
 *
 * Since: 1.0
 **/
void
adg_canvas_apply_margins(AdgCanvas *canvas, CpmlExtents *extents)
{
    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_return_if_fail(extents != NULL);

    if (extents->is_defined) {
        AdgCanvasPrivate *data = canvas->data;

        extents->org.x -= data->left_margin;
        extents->org.y -= data->top_margin;
        extents->size.x += data->left_margin + data->right_margin;
        extents->size.y += data->top_margin + data->bottom_margin;
    }
}

/**
 * adg_canvas_switch_frame:
 * @canvas:    an #AdgCanvas
 * @new_state: the new flag status
 *
 * Sets a new status on the #AdgCanvas:has-frame
 * property: <constant>TRUE</constant> means a border around
 * the canvas extents (less the margins) should be rendered.
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
 * margins) should be rendered (<constant>TRUE</constant>) or not
 * (<constant>FALSE</constant>).
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
 * Changes the top padding of @canvas by setting
 * #AdgCanvas:top-padding to @value. Negative values are allowed.
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
 * Returns: the requested padding or 0 on error.
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
 * @value:  the new padding, in global space
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
 * Returns: the requested padding or 0 on error.
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
 * @value:  the new padding, in global space
 *
 * Changes the bottom padding of @canvas by setting
 * #AdgCanvas:bottom-padding to @value. Negative values are allowed.
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
 * Returns: the requested padding or 0 on error.
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
 * @value:  the new padding, in global space
 *
 * Changes the left padding of @canvas by setting
 * #AdgCanvas:left-padding to @value. Negative values are allowed.
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
 * Returns: the requested padding or 0 on error.
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
 * @top:    top padding, in global space
 * @right:  right padding, in global space
 * @bottom: bottom padding, in global space
 * @left:   left padding, in global space
 *
 * Convenient function to set all the paddings at once.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_paddings(AdgCanvas *canvas,
                        gdouble top, gdouble right,
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
    AdgTitleBlock *title_block = data->title_block;

    if (_ADG_OLD_ENTITY_CLASS->local_changed)
        _ADG_OLD_ENTITY_CLASS->local_changed(entity);

    if (title_block != NULL) {
        const gchar *scale = adg_title_block_get_scale(title_block);

        if (scale != NULL && scale[0] != '\0') {
            const cairo_matrix_t *map = adg_entity_get_local_map(entity);
            gdouble factor = adg_scale_factor(scale);

            if (map->xx != factor || map->yy != factor)
                adg_title_block_set_scale(title_block, "---");
        }

        adg_entity_local_changed((AdgEntity *) title_block);
    }
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
    AdgCanvas *canvas;
    AdgCanvasPrivate *data;
    CpmlExtents extents;

    if (_ADG_OLD_ENTITY_CLASS->arrange)
        _ADG_OLD_ENTITY_CLASS->arrange(entity);

    cpml_extents_copy(&extents, adg_entity_get_extents(entity));

    /* The extents should be defined, otherwise there is no drawing */
    g_return_if_fail(extents.is_defined);

    canvas = (AdgCanvas *) entity;
    data = canvas->data;

    _adg_apply_paddings(canvas, &extents);

    if (data->size.x > 0 || data->size.y > 0) {
        const cairo_matrix_t *global = adg_entity_get_global_matrix(entity);
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

    if (data->title_block) {
        AdgEntity *title_block_entity;
        const CpmlExtents *title_block_extents;
        CpmlPair shift;

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
            cairo_matrix_t unglobal, map;
            adg_matrix_copy(&unglobal, adg_entity_get_global_matrix(entity));
            cairo_matrix_invert(&unglobal);

            cairo_matrix_transform_distance(&unglobal, &shift.x, &shift.y);
            cairo_matrix_init_translate(&map, shift.x, shift.y);
            adg_entity_transform_global_map(title_block_entity, &map,
                                            ADG_TRANSFORM_AFTER);

            adg_entity_global_changed(title_block_entity);
            adg_entity_arrange(title_block_entity);
            title_block_extents = adg_entity_get_extents(title_block_entity);
            cpml_extents_add(&extents, title_block_extents);
        }
    }

    /* Impose the new extents */
    adg_entity_set_extents(entity, &extents);
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

static void
_adg_apply_paddings(AdgCanvas *canvas, CpmlExtents *extents)
{
    AdgCanvasPrivate *data = canvas->data;

    extents->org.x -= data->left_padding;
    extents->size.x += data->left_padding + data->right_padding;
    extents->org.y -= data->top_padding;
    extents->size.y += data->top_padding + data->bottom_padding;
}


/**
 * adg_canvas_export:
 * @canvas: an #AdgCanvas
 * @type: the export format
 * @file: the name of the resulting file
 * @gerror: (allow-none): return location for errors
 *
 * A helper function that provides a bare export functionality.
 * It basically exports the drawing in @canvas in the @file
 * in the @type format. Any error will be reported in @gerror,
 * if not <constant>NULL</constant>.
 *
 * Returns: <constant>TRUE</constant> on success, <constant>FALSE</constant> otherwise.
 *
 * Since: 1.0
 **/
gboolean
adg_canvas_export(AdgCanvas *canvas, cairo_surface_type_t type,
                  const gchar *file, GError **gerror)
{
    AdgEntity *entity;
    const CpmlExtents *extents;
    gdouble top, bottom, left, right, width, height;
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_status_t status;

    g_return_val_if_fail(ADG_IS_CANVAS(canvas), FALSE);
    g_return_val_if_fail(file != NULL, FALSE);
    g_return_val_if_fail(gerror == NULL || *gerror == NULL, FALSE);

    entity = (AdgEntity *) canvas;

    adg_entity_arrange(entity);
    extents = adg_entity_get_extents(entity);

    top = adg_canvas_get_top_margin(canvas);
    bottom = adg_canvas_get_bottom_margin(canvas);
    left = adg_canvas_get_left_margin(canvas);
    right = adg_canvas_get_right_margin(canvas);
    width = extents->size.x + left + right;
    height = extents->size.y + top + bottom;
    surface = NULL;

    switch (type) {
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    case CAIRO_SURFACE_TYPE_IMAGE:
        surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
        break;
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    case CAIRO_SURFACE_TYPE_PDF:
        surface = cairo_pdf_surface_create(file, width, height);
        break;
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    case CAIRO_SURFACE_TYPE_PS:
        surface = cairo_ps_surface_create(file, width, height);
        break;
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
    case CAIRO_SURFACE_TYPE_SVG:
        surface = cairo_svg_surface_create(file, width, height);
        break;
#endif
    default:
        break;
    }

    if (surface == NULL) {
        g_set_error(gerror, ADG_CANVAS_ERROR, ADG_CANVAS_ERROR_SURFACE,
                    "unable to handle surface type '%d'",
                    type);
        return FALSE;
    }

    cairo_surface_set_device_offset(surface, left, top);
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    adg_entity_render(ADG_ENTITY(canvas), cr);

    if (cairo_surface_get_type(surface) == CAIRO_SURFACE_TYPE_IMAGE) {
        status = cairo_surface_write_to_png(surface, file);
    } else {
        cairo_show_page(cr);
        status = cairo_status(cr);
    }

    cairo_destroy(cr);

    if (status != CAIRO_STATUS_SUCCESS) {
        g_set_error(gerror, ADG_CANVAS_ERROR, ADG_CANVAS_ERROR_CAIRO,
                    "cairo reported '%s'",
                    cairo_status_to_string(status));
        return FALSE;
    }

    return TRUE;
}


#if GTK3_ENABLED || GTK2_ENABLED
#include <gtk/gtk.h>

static void
_adg_update_margin(AdgCanvas *canvas, gdouble *margin,
                   gdouble *side, gdouble new_margin)
{
    GtkPageSetup *page_setup;
    gdouble old_margin;

    old_margin = *margin;
    *margin = new_margin;

    page_setup = adg_canvas_get_page_setup(canvas);
    if (page_setup == NULL)
        return;

    *side += old_margin - new_margin;
}

/**
 * adg_canvas_set_paper:
 * @canvas:      an #AdgCanvas
 * @paper_name:  a paper name
 * @orientation: the page orientation
 *
 * A convenient function to set size and margins of @canvas
 * using a @paper_name and an @orientation value. This should
 * be a PWG 5101.1-2002 paper name and it will be passed as
 * is to gtk_paper_size_new(), so use any valid name accepted
 * by that function.
 *
 * This has the same effect as creating a #GtkPaperSetup object
 * and binding it to @canvas with adg_canvas_set_page_setup():
 * check its documentation for knowing the implications.
 *
 * To reset the size to its default behavior (i.e. autocalculate
 * it from the entities) just call adg_canvas_set_size_explicit()
 * passing <constant>0, 0</constant>.
 *
 * If you want to use your own margins on a named paper size,
 * set them <emphasis>after</emphasis> the call to this function.
 *
 * Since: 1.0
 **/
void
adg_canvas_set_paper(AdgCanvas *canvas,
                     const gchar *paper_name,
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
 * @canvas:                                   an #AdgCanvas
 * @page_setup: (allow-none) (transfer none): the page setup
 *
 * A convenient function to setup the page of @canvas so it can
 * also be subsequentially used for printing. It is allowed to
 * pass <constant>NULL</constant> as @page_setup to restore the
 * default page setup.
 *
 * When a canvas is bound to a page setup, the paper size is kept
 * constant. This implies increasing or decreasing the margins
 * respectively decreases or increases the page size of the
 * relevant dimension, e.g. increasing the right margin decreases
 * the width (the x component of the size).
 *
 * A reference to @page_setup is added, so there is no need to keep
 * alive this object after a call to this function. @page_setup can
 * be retrieved at any time with adg_canvas_get_page_setup().
 *
 * The size and margins provided by @page_setup are immediately
 * used to set size and margins of @canvas. This means using
 * <constant>NULL</constant> as @page_setup just releases the
 * reference to the previous @page_setup object... all the page
 * settings are still retained.
 *
 * If you want to use your own margins on a page setup,
 * set them on canvas <emphasis>after</emphasis> the call to this
 * function or set them on @page_setup <emphasis>before</emphasis>.
 *
 * <informalexample><programlisting language="C">
 * // By default, canvas does not have an explicit size
 * adg_canvas_set_page_setup(canvas, a4);
 * g_object_unref(a4);
 * // Now canvas has size and margins specified by a4
 * // (and a4 should be a prefilled GtkPageSetup object).
 * adg_canvas_set_page_setup(canvas, NULL);
 * // Now canvas is no more bound to a4 and that object (if
 * // not referenced anywhere else) can be garbage-collected.
 * // The page setup of canvas has not changed though.
 * adg_canvas_set_size_explicit(canvas, 0, 0);
 * // Now the page size of canvas has been restored to
 * // their default behavior.
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/
void
adg_canvas_set_page_setup(AdgCanvas *canvas, GtkPageSetup *page_setup)
{
    gdouble top, right, bottom, left;
    CpmlPair size;

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

/**
 * adg_canvas_get_page_setup:
 * @canvas: an #AdgCanvas
 *
 * If adg_canvas_set_page_setup() is called, a #GtkPageSetup object
 * is created and bound to @canvas. This metho returns a pointer
 * to that internal object or <constant>NULL</constant> if
 * adg_canvas_set_page_setup() has not been called before.
 *
 * Returns: (allow-none) (transfer none): the #GtkPageSetup with size and margins of @canvas of <constant>NULL</constant> on no setup found or errors.
 *
 * Since: 1.0
 **/
GtkPageSetup *
adg_canvas_get_page_setup(AdgCanvas *canvas)
{
    g_return_val_if_fail(ADG_IS_CANVAS(canvas), NULL);
    return g_object_get_data((GObject *) canvas, "_adg_page_setup");
}

#else

static void
_adg_update_margin(AdgCanvas *canvas, gdouble *margin,
                   gdouble *side, gdouble new_margin)
{
    *margin = new_margin;
}
#endif
