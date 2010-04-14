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
 * SECTION:adg-gtk-area
 * @short_description: A #GtkWidget specifically designed to contain
 *                     an #AdgCanvas entity
 *
 * This is a #GtkDrawingArea derived object that provides an easy way
 * to show an ADG based canvas. The associated canvas can be set
 * directly with the adg_gtk_area_new_with_canvas() constructor
 * function or by using adg_gtk_area_set_canvas().
 *
 * The default minimum size of this widget will depend on the canvas
 * content. The global matrix of the #AdgCanvas will be adjusted to
 * expose the drawing in the proper position. The empty space around
 * the drawing can be changed by setting the #AdgGtkArea:top-margin,
 * #AdgGtkArea:right-margin, #AdgGtkArea:bottom-margin and
 * #AdgGtkArea:left-margin properties or by using the equivalent
 * setter methods.
 *
 * The default implementation reacts to some mouse events: if you drag
 * the mouse keeping the wheel pressed the canvas will be translated;
 * if the mouse wheel is rotated the canvas will be scaled up or down
 * (accordingly to the wheel direction) by the factor specified in the
 * #AdgGtkArea:factor property.
 **/

/**
 * AdgGtkArea:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-gtk-internal.h"
#include "adg-gtk-area.h"
#include "adg-gtk-area-private.h"

#define PARENT_OBJECT_CLASS  ((GObjectClass *) adg_gtk_area_parent_class)
#define PARENT_WIDGET_CLASS  ((GtkWidgetClass *) adg_gtk_area_parent_class)


G_DEFINE_TYPE(AdgGtkArea, adg_gtk_area, GTK_TYPE_DRAWING_AREA);

enum {
    PROP_0,
    PROP_CANVAS,
    PROP_FACTOR,
    PROP_TOP_PADDING,
    PROP_RIGHT_PADDING,
    PROP_BOTTOM_PADDING,
    PROP_LEFT_PADDING
};

enum {
    CANVAS_CHANGED,
    LAST_SIGNAL
};


static void             _adg_dispose            (GObject         *object);
static void             _adg_get_property       (GObject         *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void             _adg_set_property       (GObject         *object,
                                                 guint            prop_id,
                                                 const GValue    *value,
                                                 GParamSpec      *pspec);
static gboolean         _adg_set_canvas         (AdgGtkArea      *area,
                                                 AdgCanvas       *canvas);
static gboolean         _adg_set_factor         (AdgGtkArea      *area,
                                                 gdouble          factor);
static void             _adg_size_request       (GtkWidget       *widget,
                                                 GtkRequisition  *requisition);
static void             _adg_size_allocate      (GtkWidget       *widget,
                                                 GtkAllocation   *allocation);
static gboolean         _adg_expose_event       (GtkWidget       *widget,
                                                 GdkEventExpose  *event);
static gboolean         _adg_scroll_event       (GtkWidget       *widget,
                                                 GdkEventScroll  *event);
static gboolean         _adg_button_press_event (GtkWidget       *widget,
                                                 GdkEventButton  *event);
static gboolean         _adg_motion_notify_event(GtkWidget       *widget,
                                                 GdkEventMotion  *event);
static gboolean         _adg_get_local_map      (GtkWidget       *widget,
                                                 AdgMatrix       *map,
                                                 AdgMatrix       *inverted);
static void             _adg_set_local_map      (GtkWidget       *widget,
                                                 const AdgMatrix *map);

static guint            _adg_signals[LAST_SIGNAL] = { 0 };


static void
adg_gtk_area_class_init(AdgGtkAreaClass *klass)
{
    GObjectClass *gobject_class;
    GtkWidgetClass *widget_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    widget_class = (GtkWidgetClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgGtkAreaPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    widget_class->size_request = _adg_size_request;
    widget_class->size_allocate = _adg_size_allocate;
    widget_class->expose_event = _adg_expose_event;
    widget_class->scroll_event = _adg_scroll_event;
    widget_class->button_press_event = _adg_button_press_event;
    widget_class->motion_notify_event = _adg_motion_notify_event;

    param = g_param_spec_object("canvas",
                                P_("Canvas"),
                                P_("The canvas to be shown"),
                                ADG_TYPE_CANVAS,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_CANVAS, param);

    param = g_param_spec_double("factor",
                                P_("Factor"),
                                P_("The factor used in zooming in and out"),
                                1., G_MAXDOUBLE, 1.05,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FACTOR, param);

    param = g_param_spec_double("top-padding",
                                P_("Top Padding"),
                                P_("The padding (in identity space) to leave empty above the canvas while showing the widget the first time"),
                                G_MINDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TOP_PADDING, param);

    param = g_param_spec_double("right-padding",
                                P_("Right Padding"),
                                P_("The padding (in identity space) to leave empty at the right of the canvas while showing the widget the first time"),
                                G_MINDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RIGHT_PADDING, param);

    param = g_param_spec_double("bottom-padding",
                                P_("Bottom Padding"),
                                P_("The padding (in identity space) to leave empty below the canvas while showing the widget the first time"),
                                G_MINDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BOTTOM_PADDING, param);

    param = g_param_spec_double("left-padding",
                                P_("Left Padding"),
                                P_("The padding (in identity space) to leave empty at the left of the canvas while showing the widget the first time"),
                                G_MINDOUBLE, G_MAXDOUBLE, 15,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LEFT_PADDING, param);

    /**
     * AdgGtkArea::canvas-changed:
     * @area: an #AdgGtkArea
     *
     * Emitted when the #AdgGtkArea has a new canvas.
     **/
    _adg_signals[CANVAS_CHANGED] =
        g_signal_new("canvas-changed", ADG_GTK_TYPE_AREA,
                     G_SIGNAL_RUN_LAST|G_SIGNAL_NO_RECURSE,
                     G_STRUCT_OFFSET(AdgGtkAreaClass, canvas_changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);
}

static void
adg_gtk_area_init(AdgGtkArea *area)
{
    AdgGtkAreaPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(area,
                                                          ADG_GTK_TYPE_AREA,
                                                          AdgGtkAreaPrivate);

    data->canvas = NULL;
    data->factor = 1.05;
    data->top_padding = 15;
    data->right_padding = 15;
    data->bottom_padding = 15;
    data->left_padding = 15;

    data->x_event = 0;
    data->y_event = 0;

    area->data = data;

    /* Enable GDK events to catch wheel rotation and drag */
    gtk_widget_add_events((GtkWidget *) area,
                          GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON2_MOTION_MASK |
                          GDK_SCROLL_MASK);
}

static void
_adg_dispose(GObject *object)
{
    AdgGtkAreaPrivate *data = ((AdgGtkArea *) object)->data;

    if (data->canvas != NULL) {
        g_object_unref(data->canvas);
        data->canvas = NULL;
    }

    if (PARENT_OBJECT_CLASS->dispose)
        PARENT_OBJECT_CLASS->dispose(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgGtkAreaPrivate *data = ((AdgGtkArea *) object)->data;

    switch (prop_id) {
    case PROP_CANVAS:
        g_value_set_object(value, data->canvas);
        break;
    case PROP_FACTOR:
        g_value_set_double(value, data->factor);
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
    AdgGtkArea *area;
    AdgGtkAreaPrivate *data;

    area = (AdgGtkArea *) object;
    data = area->data;

    switch (prop_id) {
    case PROP_CANVAS:
        _adg_set_canvas(area, g_value_get_object(value));
        break;
    case PROP_FACTOR:
        _adg_set_factor(area, g_value_get_double(value));
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
 * adg_gtk_area_new:
 *
 * Creates a new empty #AdgGtkArea. The widget is useful only after
 * an #AdgCanvas has been added either using the #AdgGtkArea:canvas
 * property or with adg_gtk_area_set_canvas().
 *
 * Returns: the newly created widget
 **/
GtkWidget *
adg_gtk_area_new(void)
{
    return g_object_new(ADG_GTK_TYPE_AREA, NULL);
}

/**
 * adg_gtk_area_new_with_canvas:
 * @canvas: the #AdgCanvas shown by this widget
 *
 * Creates a new #AdgGtkArea and sets the #AdgGtkArea:canvas property
 * to @canvas.
 *
 * Returns: the newly created widget
 **/
GtkWidget *
adg_gtk_area_new_with_canvas(AdgCanvas *canvas)
{
    g_return_val_if_fail(ADG_IS_CANVAS(canvas), NULL);

    return g_object_new(ADG_GTK_TYPE_AREA, "canvas", canvas, NULL);
}

/**
 * adg_gtk_area_set_canvas:
 * @area: an #AdgGtkArea
 * @canvas: the new #AdgCanvas
 *
 * Sets a new canvas on @area. The old canvas, if presents, is
 * unreferenced.
 **/
void
adg_gtk_area_set_canvas(AdgGtkArea *area, AdgCanvas *canvas)
{
    g_return_if_fail(ADG_GTK_IS_AREA(area));

    if (_adg_set_canvas(area, canvas))
        g_object_notify((GObject *) area, "canvas");
}

/**
 * adg_gtk_area_get_canvas:
 * @area: an #AdgGtkArea
 *
 * Gets the canvas associated to @area.
 *
 * Returns: the requested #AdgCanvas object or %NULL on errors
 **/
AdgCanvas *
adg_gtk_area_get_canvas(AdgGtkArea *area)
{
    AdgGtkAreaPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_AREA(area), NULL);

    data = area->data;

    return data->canvas;
}

/**
 * adg_gtk_area_set_factor:
 * @area: an #AdgGtkArea
 * @factor: the new zoom factor
 *
 * Sets a new zoom factor to @area. If the factor is less than
 * 1, it will be clamped to 1.
 **/
void
adg_gtk_area_set_factor(AdgGtkArea *area, gdouble factor)
{
    g_return_if_fail(ADG_GTK_IS_AREA(area));

    if (_adg_set_factor(area, factor))
        g_object_notify((GObject *) area, "factor");
}

/**
 * adg_gtk_area_get_factor:
 * @area: an #AdgGtkArea
 *
 * Gets the zoom factor associated to @area. The zoom factor is
 * directly used to zoom in (that is, the default zoom factor of
 * 1.05 will zoom of 5% every iteration) and it is reversed while
 * zooming out (that is, the default factor will use 1/1.05).
 *
 * Returns: the requested zoom factor or 0 on error
 **/
gdouble
adg_gtk_area_get_factor(AdgGtkArea *area)
{
    AdgGtkAreaPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_AREA(area), 0.);

    data = area->data;
    return data->factor;
}

/**
 * adg_gtk_area_set_top_padding:
 * @area: an #AdgGtkArea
 * @value: the new padding, in identity space
 *
 * Changes the top padding of @area by setting #AdgGtkArea:top-padding
 * to @value. Negative values are allowed.
 **/
void
adg_gtk_area_set_top_padding(AdgGtkArea *area, gdouble value)
{
    g_return_if_fail(ADG_GTK_IS_AREA(area));
    g_object_set((GObject *) area, "top-padding", value, NULL);
}

/**
 * adg_gtk_area_get_top_padding:
 * @area: an #AdgGtkArea
 *
 * Gets the top padding (in identity space) of @area.
 *
 * Returns: the requested padding or %0 on error
 **/
gdouble
adg_gtk_area_get_top_padding(AdgGtkArea *area)
{
    AdgGtkAreaPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_AREA(area), 0.);

    data = area->data;
    return data->top_padding;
}

/**
 * adg_gtk_area_set_right_padding:
 * @area: an #AdgGtkArea
 * @value: the new padding, in identity space
 *
 * Changes the right padding of @area by setting #AdgGtkArea:right-padding
 * to @value. Negative values are allowed.
 **/
void
adg_gtk_area_set_right_padding(AdgGtkArea *area, gdouble value)
{
    g_return_if_fail(ADG_GTK_IS_AREA(area));
    g_object_set((GObject *) area, "right-padding", value, NULL);
}

/**
 * adg_gtk_area_get_right_padding:
 * @area: an #AdgGtkArea
 *
 * Gets the right padding (in identity space) of @area.
 *
 * Returns: the requested padding or %0 on error
 **/
gdouble
adg_gtk_area_get_right_padding(AdgGtkArea *area)
{
    AdgGtkAreaPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_AREA(area), 0.);

    data = area->data;
    return data->right_padding;
}


/**
 * adg_gtk_area_set_bottom_padding:
 * @area: an #AdgGtkArea
 * @value: the new padding, in identity space
 *
 * Changes the bottom padding of @area by setting #AdgGtkArea:bottom-padding
 * to @value. Negative values are allowed.
 **/
void
adg_gtk_area_set_bottom_padding(AdgGtkArea *area, gdouble value)
{
    g_return_if_fail(ADG_GTK_IS_AREA(area));
    g_object_set((GObject *) area, "bottom-padding", value, NULL);
}

/**
 * adg_gtk_area_get_bottom_padding:
 * @area: an #AdgGtkArea
 *
 * Gets the bottom padding (in identity space) of @area.
 *
 * Returns: the requested padding or %0 on error
 **/
gdouble
adg_gtk_area_get_bottom_padding(AdgGtkArea *area)
{
    AdgGtkAreaPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_AREA(area), 0.);

    data = area->data;
    return data->bottom_padding;
}

/**
 * adg_gtk_area_set_left_padding:
 * @area: an #AdgGtkArea
 * @value: the new padding, in identity space
 *
 * Changes the left padding of @area by setting #AdgGtkArea:left-padding
 * to @value. Negative values are allowed.
 **/
void
adg_gtk_area_set_left_padding(AdgGtkArea *area, gdouble value)
{
    g_return_if_fail(ADG_GTK_IS_AREA(area));
    g_object_set((GObject *) area, "left-padding", value, NULL);
}

/**
 * adg_gtk_area_get_left_padding:
 * @area: an #AdgGtkArea
 *
 * Gets the left padding (in identity space) of @area.
 *
 * Returns: the requested padding or %0 on error
 **/
gdouble
adg_gtk_area_get_left_padding(AdgGtkArea *area)
{
    AdgGtkAreaPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_AREA(area), 0.);

    data = area->data;
    return data->left_padding;
}


/**
 * adg_gtk_area_set_paddings:
 * @area: an #AdgGtkArea
 * @top: top padding, in identity space
 * @right: right padding, in identity space
 * @bottom: bottom padding, in identity space
 * @left: left padding, in identity space
 *
 * Convenient function to set all the paddings at once.
 **/
void
adg_gtk_area_set_paddings(AdgGtkArea *area, gdouble top, gdouble right,
                          gdouble bottom, gdouble left)
{
    g_return_if_fail(ADG_GTK_IS_AREA(area));
    g_object_set((GObject *) area,
                 "top-padding", top,
                 "right-padding", right,
                 "bottom-padding", bottom,
                 "left-padding", left,
                 NULL);
}


static gboolean
_adg_set_canvas(AdgGtkArea *area, AdgCanvas *canvas)
{
    AdgGtkAreaPrivate *data;

    g_return_val_if_fail(canvas == NULL || ADG_IS_CANVAS(canvas), FALSE);

    data = area->data;

    if (data->canvas == canvas)
        return FALSE;

    if (canvas)
        g_object_ref(canvas);

    if (data->canvas)
        g_object_unref(data->canvas);

    data->canvas = canvas;
    g_signal_emit(area, _adg_signals[CANVAS_CHANGED], 0);
    return TRUE;
}

static gboolean
_adg_set_factor(AdgGtkArea *area, gdouble factor)
{
    AdgGtkAreaPrivate *data;

    /* A better approach would be to use the GParamSpec of this property */
    g_return_val_if_fail(factor >= 1, FALSE);

    data = area->data;

    if (data->factor == factor)
        return FALSE;

    data->factor = factor;
    return TRUE;
}

static void
_adg_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
    AdgGtkAreaPrivate *data;
    AdgCanvas *canvas;
    AdgEntity *entity;
    const CpmlExtents *extents;

    data = ((AdgGtkArea *) widget)->data;
    canvas = data->canvas;

    if (canvas == NULL)
        return;

    entity = (AdgEntity *) canvas;
    adg_entity_arrange(entity);
    extents = adg_entity_get_extents(entity);

    if (extents == NULL || !extents->is_defined)
        return;

    requisition->width =
        extents->size.x + data->left_padding + data->right_padding;
    requisition->height =
        extents->size.y + data->top_padding + data->bottom_padding;
}

static void
_adg_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
    AdgGtkAreaPrivate *data;
    AdgCanvas *canvas;
    AdgEntity *entity;
    const CpmlExtents *extents;
    AdgPair org, padding, ratio;
    gdouble factor;
    AdgMatrix map;

    if (PARENT_WIDGET_CLASS->size_allocate)
        PARENT_WIDGET_CLASS->size_allocate(widget, allocation);

    data = ((AdgGtkArea *) widget)->data;
    canvas = data->canvas;

    if (canvas == NULL)
        return;

    entity = (AdgEntity *) canvas;
    extents = adg_entity_get_extents(entity);

    if (extents == NULL || !extents->is_defined ||
        extents->size.x <= 0 || extents->size.y <= 0)
        return;

    padding.x = data->left_padding + data->right_padding;
    padding.y = data->top_padding + data->bottom_padding;

    /* Check if the allocated space is enough:
     * if not, there is no much we can do... */
    if (allocation->width <= padding.x || allocation->height <= padding.y)
        return;

    ratio.x = (allocation->width - padding.x) / extents->size.x;
    ratio.y = (allocation->height - padding.y) / extents->size.y;
    factor = MIN(ratio.x, ratio.y);
    org.x = allocation->width - extents->size.x * factor;
    org.y = allocation->height - extents->size.y * factor;

    /* Now org contains the new paddings: they should be scaled
     * accordingly to the original paddings. If the original
     * paddings are not specified (== 0), the additional space
     * is distributed equally among both sides. */
    if (padding.x == 0)
        org.x /= 2;
    else
        org.x *= data->left_padding / padding.x;

    if (padding.y == 0)
        org.y /= 2;
    else
        org.y *= data->bottom_padding / padding.y;

    org.x -= extents->org.x;
    org.y -= extents->org.y;

    cairo_matrix_init_scale(&map, factor, factor);
    cairo_matrix_translate(&map, org.x, org.y);
    adg_entity_transform_global_map(entity, &map, ADG_TRANSFORM_AFTER);
}

static gboolean
_adg_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    AdgGtkAreaPrivate *data;
    AdgCanvas *canvas;

    data = ((AdgGtkArea *) widget)->data;
    canvas = data->canvas;

    if (canvas != NULL) {
        cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
        adg_entity_render((AdgEntity *) canvas, cr);
        cairo_destroy(cr);
    }

    if (PARENT_WIDGET_CLASS->expose_event == NULL)
        return FALSE;

    return PARENT_WIDGET_CLASS->expose_event(widget, event);
}

static gboolean
_adg_scroll_event(GtkWidget *widget, GdkEventScroll *event)
{
    AdgGtkAreaPrivate *data;
    AdgMatrix map, inverted;

    data = ((AdgGtkArea *) widget)->data;

    if ((event->direction == GDK_SCROLL_UP ||
         event->direction == GDK_SCROLL_DOWN) &&
        _adg_get_local_map(widget, &map, &inverted)) {
        double factor, x, y;

        if (event->direction == GDK_SCROLL_UP) {
            factor = data->factor;
        } else {
            factor = 1. / data->factor;
        }

        x = event->x;
        y = event->y;

        cairo_matrix_transform_point(&inverted, &x, &y);

        cairo_matrix_scale(&map, factor, factor);
        cairo_matrix_translate(&map, x/factor - x, y/factor - y);

        _adg_set_local_map(widget, &map);

        gtk_widget_queue_draw(widget);
    }

    if (PARENT_WIDGET_CLASS->scroll_event == NULL)
        return FALSE;

    return PARENT_WIDGET_CLASS->scroll_event(widget, event);
}

static gboolean
_adg_button_press_event(GtkWidget *widget, GdkEventButton *event)
{
    AdgGtkAreaPrivate *data = ((AdgGtkArea *) widget)->data;

    if (event->type == GDK_BUTTON_PRESS && event->button == 2) {
        data->x_event = event->x;
        data->y_event = event->y;
    }

    if (PARENT_WIDGET_CLASS->button_press_event == NULL)
        return FALSE;

    return PARENT_WIDGET_CLASS->button_press_event(widget, event);
}

static gboolean
_adg_motion_notify_event(GtkWidget *widget, GdkEventMotion *event)
{
    AdgGtkAreaPrivate *data;
    AdgMatrix map, inverted;

    data = ((AdgGtkArea *) widget)->data;

    if ((event->state & GDK_BUTTON2_MASK) > 0 &&
        _adg_get_local_map(widget, &map, &inverted)) {
        double x, y;

        x = event->x - data->x_event;
        y = event->y - data->y_event;

        cairo_matrix_transform_distance(&inverted, &x, &y);
        cairo_matrix_translate(&map, x, y);
        data->x_event = event->x;
        data->y_event = event->y;

        _adg_set_local_map(widget, &map);

        gtk_widget_queue_draw(widget);
    }

    if (PARENT_WIDGET_CLASS->motion_notify_event == NULL)
        return FALSE;

    return PARENT_WIDGET_CLASS->motion_notify_event(widget, event);
}

static gboolean
_adg_get_local_map(GtkWidget *widget, AdgMatrix *map, AdgMatrix *inverted)
{
    AdgGtkAreaPrivate *data;
    AdgCanvas *canvas;

    data = ((AdgGtkArea *) widget)->data;
    canvas = data->canvas;
    if (canvas == NULL)
        return FALSE;

    adg_matrix_copy(map, adg_entity_get_local_map((AdgEntity *) canvas));
    adg_matrix_copy(inverted, map);

    return cairo_matrix_invert(inverted) == CAIRO_STATUS_SUCCESS;
}

static void
_adg_set_local_map(GtkWidget *widget, const AdgMatrix *map)
{
    AdgGtkAreaPrivate *data;
    AdgCanvas *canvas;

    data = ((AdgGtkArea *) widget)->data;
    canvas = data->canvas;

    if (canvas != NULL)
        adg_entity_set_local_map((AdgEntity *) canvas, map);
}
