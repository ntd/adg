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
 * SECTION:adg-gtk-layout
 * @short_description: A scrollable AdgGtkArea based widget
 *
 * This is an #AdgGtkArea derived object with scrolling capabilities.
 * It means an #AdgGtkLayout object can be added directly to a
 * #GtkScrolledWindow container without the need for an intermediate
 * #GtkViewport container.
 *
 * Since: 1.0
 **/

/**
 * AdgGtkLayout:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <gtk/gtk.h>

#include "adg-container.h"
#include "adg-table.h"
#include "adg-title-block.h"
#include <adg-canvas.h>
#include "adg-gtk-area.h"

#include "adg-gtk-layout.h"
#include "adg-gtk-layout-private.h"

#define _ADG_OLD_OBJECT_CLASS  ((GObjectClass *) adg_gtk_layout_parent_class)
#define _ADG_OLD_WIDGET_CLASS  ((GtkWidgetClass *) adg_gtk_layout_parent_class)
#define _ADG_OLD_AREA_CLASS    ((AdgGtkAreaClass *) adg_gtk_layout_parent_class)

#ifdef GTK2_ENABLED
enum {
    PROP_0,
    PROP_HADJUSTMENT,
    PROP_VADJUSTMENT
};

G_DEFINE_TYPE(AdgGtkLayout, adg_gtk_layout, ADG_GTK_TYPE_AREA)

static void
_adg_set_scroll_adjustments(GtkWidget *widget,
                            GtkAdjustment *hadjustment,
                            GtkAdjustment *vadjustment)
{
    g_object_set(widget,
                 "hadjustment", hadjustment,
                 "vadjustment", vadjustment,
                 NULL);
}
#endif

#ifdef GTK3_ENABLED
enum {
    PROP_0,
    PROP_HADJUSTMENT,
    PROP_VADJUSTMENT,
    PROP_HSCROLL_POLICY,
    PROP_VSCROLL_POLICY
};

G_DEFINE_TYPE_WITH_CODE(AdgGtkLayout, adg_gtk_layout, ADG_GTK_TYPE_AREA,
                        G_IMPLEMENT_INTERFACE(GTK_TYPE_SCROLLABLE, NULL))
#endif


static void
_adg_set_parent_size(AdgGtkLayout *layout)
{
    GtkWidget *widget;
    AdgGtkLayoutPrivate *data;
    GtkWidget *parent;
    const CpmlExtents *sheet;
    GtkScrolledWindow *scrolled_window;

    widget = (GtkWidget *) layout;

    /* When the widget is realized it is too late to suggest a size */
    if (gtk_widget_get_realized(widget))
        return;

    data = layout->data;
    parent = gtk_widget_get_parent(widget);
    if (!GTK_IS_WIDGET(parent))
        return;

    sheet = adg_gtk_area_get_extents((AdgGtkArea *) layout);
    if (sheet == NULL || !sheet->is_defined)
        return;

    gtk_widget_set_size_request(parent, sheet->size.x + 2, sheet->size.y + 2);

    if (GTK_IS_SCROLLED_WINDOW(parent) && !data->policy_stored) {
        scrolled_window = (GtkScrolledWindow *) parent;

        gtk_scrolled_window_get_policy(scrolled_window,
                                       &data->hpolicy, &data->vpolicy);
        gtk_scrolled_window_set_policy(scrolled_window,
                                       GTK_POLICY_NEVER, GTK_POLICY_NEVER);

        data->policy_stored = TRUE;
    }
}

static void
_adg_parent_set(GtkWidget *widget, GtkWidget *old_parent)
{
    AdgGtkLayout *layout = (AdgGtkLayout *) widget;

    if (_ADG_OLD_WIDGET_CLASS->parent_set != NULL)
        _ADG_OLD_WIDGET_CLASS->parent_set(widget, old_parent);

    _adg_set_parent_size(layout);
}

/**
 * _adg_update_adjustments:
 * @layout: an #AdgGtkLayout
 *
 * Updates the scrollbars according to the new extents of the canvas
 * of @area and to the current viewport.
 *
 * The algorithm uses three local #CpmlExtents variables: the
 * <var>viewport</var> (what physically shown by the graphic device),
 * the <var>sheet</var> (the extents of the drawing, margins
 * included) and <var>surface</var> (a helper variable that is the
 * union of the previous two extents).
 **/
static void
_adg_update_adjustments(AdgGtkLayout *layout)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;
    const CpmlExtents *sheet, *viewport;
    AdgGtkLayoutPrivate *data;
    GtkAdjustment *hadj, *vadj;
    CpmlExtents surface;

    area = (AdgGtkArea *) layout;
    canvas = adg_gtk_area_get_canvas(area);
    if (canvas == NULL)
        return;

    sheet = adg_gtk_area_get_extents(area);
    if (sheet == NULL || !sheet->is_defined)
        return;

    data = layout->data;
    hadj = data->hadjustment;
    vadj = data->vadjustment;
    viewport = &data->viewport;
    surface = *sheet;
    cpml_extents_add(&surface, viewport);

    if (data->policy_stored) {
        /* Restore the original policy for the scrollbars */
        GtkWidget *parent;
        GtkScrolledWindow *scrolled_window;

        parent = gtk_widget_get_parent((GtkWidget *) layout);
        scrolled_window = (GtkScrolledWindow *) parent;

        gtk_scrolled_window_set_policy(scrolled_window,
                                       data->hpolicy, data->vpolicy);

        data->policy_stored = TRUE;
    }

    g_object_set(hadj,
                 "lower", surface.org.x,
                 "upper", surface.org.x + surface.size.x,
                 "page-size", viewport->size.x,
                 "value", viewport->org.x,
                 NULL);
    g_object_set(vadj,
                 "lower", surface.org.y,
                 "upper", surface.org.y + surface.size.y,
                 "page-size", viewport->size.y,
                 "value", viewport->org.y,
                 NULL);
}

static void
_adg_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
    AdgGtkLayout *layout;
    AdgGtkLayoutPrivate *data;

    layout = (AdgGtkLayout *) widget;
    data = layout->data;

    if (_ADG_OLD_WIDGET_CLASS->size_allocate != NULL)
        _ADG_OLD_WIDGET_CLASS->size_allocate(widget, allocation);

    /* Resize the viewport on a new allocation.
     * TODO: plan other policies instead of forcibly set only the
     * size field on the viewport struct, such as modifying the
     * org to keep the sheet centered in the allocation space. */
    data->viewport.size.x = allocation->width;
    data->viewport.size.y = allocation->height;
    data->viewport.is_defined = TRUE;

    _adg_update_adjustments(layout);
}

static void
_adg_canvas_changed(AdgGtkArea *area, AdgCanvas *old_canvas)
{
    AdgGtkLayout *layout;
    AdgGtkLayoutPrivate *data;

    layout = (AdgGtkLayout *) area;
    data = layout->data;

    if (_ADG_OLD_AREA_CLASS->canvas_changed != NULL)
        _ADG_OLD_AREA_CLASS->canvas_changed(area, old_canvas);

    /* By convention, expect the origin of a new canvas to be
     * on the top/left corner of the allocation area */
    data->viewport.org.x = 0;
    data->viewport.org.y = 0;

    _adg_set_parent_size(layout);
}

static void
_adg_extents_changed(AdgGtkArea *area, const CpmlExtents *old_extents)
{
    if (_ADG_OLD_AREA_CLASS->extents_changed != NULL)
        _ADG_OLD_AREA_CLASS->extents_changed(area, old_extents);

    _adg_update_adjustments((AdgGtkLayout *) area);
}

static void
_adg_value_changed(AdgGtkLayout *layout)
{
    GtkWidget *widget;
    AdgGtkArea *area;
    AdgGtkLayoutPrivate *data;
    CpmlPair org;
    cairo_matrix_t map;

    widget = (GtkWidget *) layout;

    if (!gtk_widget_get_realized(widget))
        return;

    area = (AdgGtkArea *) layout;
    data = layout->data;
    org.x = gtk_adjustment_get_value(data->hadjustment);
    org.y = gtk_adjustment_get_value(data->vadjustment);

    cairo_matrix_init_translate(&map, data->viewport.org.x - org.x,
                                data->viewport.org.y - org.y);
    adg_gtk_area_transform_render_map(area, &map, ADG_TRANSFORM_BEFORE);

    gtk_widget_queue_draw(widget);
    _adg_update_adjustments(layout);
}

static void
_adg_set_adjustment(AdgGtkLayout *layout,
                    GtkAdjustment **dst, GtkAdjustment *src)
{
    GCallback value_changed;

    if (*dst == src)
        return;

    value_changed = G_CALLBACK(_adg_value_changed);

    if (*dst != NULL) {
        /* Release the old adjustment */
        g_signal_handlers_disconnect_by_func(*dst, (gpointer) value_changed, layout);
        g_object_unref(*dst);
    }

    g_signal_connect_swapped(src, "value-changed", value_changed, layout);
    g_object_ref_sink(src);

    *dst = src;
}

static void
_adg_dispose(GObject *object)
{
    AdgGtkLayoutPrivate *data = ((AdgGtkLayout *) object)->data;

    if (data->hadjustment != NULL) {
        g_object_unref(data->hadjustment);
        data->hadjustment = NULL;
    }

    if (data->vadjustment != NULL) {
        g_object_unref(data->vadjustment);
        data->vadjustment = NULL;
    }

    if (_ADG_OLD_OBJECT_CLASS->dispose != NULL)
        _ADG_OLD_OBJECT_CLASS->dispose(object);
}

static void
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
{
    AdgGtkLayoutPrivate *data = ((AdgGtkLayout *) object)->data;

    switch (prop_id) {
    case PROP_HADJUSTMENT:
        g_value_set_object(value, data->hadjustment);
        break;
    case PROP_VADJUSTMENT:
        g_value_set_object(value, data->vadjustment);
        break;
#ifdef GTK3_ENABLED
    case PROP_HSCROLL_POLICY:
        g_value_set_enum(value, data->hpolicy);
        break;
    case PROP_VSCROLL_POLICY:
        g_value_set_enum(value, data->vpolicy);
        break;
#endif
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    AdgGtkLayout *layout;
    AdgGtkLayoutPrivate *data;
    GtkAdjustment *adjustment;

    layout = (AdgGtkLayout *) object;
    data = layout->data;

    switch (prop_id) {
    case PROP_HADJUSTMENT:
        adjustment = g_value_get_object(value);
        if (adjustment == NULL)
            adjustment = (GtkAdjustment *) gtk_adjustment_new(0, 0, 0, 0, 0, 0);

        _adg_set_adjustment(layout, &data->hadjustment, adjustment);
        break;
    case PROP_VADJUSTMENT:
        adjustment = g_value_get_object(value);
        if (adjustment == NULL)
            adjustment = (GtkAdjustment *) gtk_adjustment_new(0, 0, 0, 0, 0, 0);

        _adg_set_adjustment(layout, &data->vadjustment, adjustment);
        break;
#ifdef GTK3_ENABLED
    case PROP_HSCROLL_POLICY:
        data->hpolicy = g_value_get_enum(value);
        break;
    case PROP_VSCROLL_POLICY:
        data->vpolicy = g_value_get_enum(value);
        break;
#endif
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
adg_gtk_layout_class_init(AdgGtkLayoutClass *klass)
{
    GObjectClass *gobject_class;
    GtkWidgetClass *widget_class;
    AdgGtkAreaClass *area_class;

    gobject_class = (GObjectClass *) klass;
    widget_class = (GtkWidgetClass *) klass;
    area_class = (AdgGtkAreaClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgGtkLayoutPrivate));

    gobject_class->dispose = _adg_dispose;
    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    widget_class->parent_set = _adg_parent_set;
    widget_class->size_allocate = _adg_size_allocate;

    area_class->canvas_changed = _adg_canvas_changed;
    area_class->extents_changed = _adg_extents_changed;

#ifdef GTK2_ENABLED
    g_object_class_install_property(gobject_class, PROP_HADJUSTMENT,
        g_param_spec_object("hadjustment",
                            P_("Horizontal adjustment"),
                            P_("The GtkAdjustment that determines the values of the horizontal position for this viewport"),
                            GTK_TYPE_ADJUSTMENT,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    g_object_class_install_property(gobject_class, PROP_VADJUSTMENT,
        g_param_spec_object("vadjustment",
                            P_("Vertical adjustment"),
                            P_("The GtkAdjustment that determines the values of the vertical position for this viewport"),
                            GTK_TYPE_ADJUSTMENT,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    /**
     * AdgGtkLayout::set-scroll-adjustments:
     * @layout: an #AdgGtkLayout
     * @hadjustment: the horizontal #GtkAdjustment
     * @vadjustment: the vertical #GtkAdjustment
     *
     * Emitted when the adjustments of the scroll bars must be changed.
     *
     * Since: 1.0
     **/
    widget_class->set_scroll_adjustments_signal =
        g_signal_new_class_handler("set-scroll-adjustments", ADG_GTK_TYPE_LAYOUT,
                                   G_SIGNAL_RUN_LAST,
                                   G_CALLBACK(_adg_set_scroll_adjustments),
                                   NULL, NULL,
                                   adg_marshal_VOID__OBJECT_OBJECT,
                                   G_TYPE_NONE, 2,
                                   GTK_TYPE_ADJUSTMENT, GTK_TYPE_ADJUSTMENT);
#endif

#ifdef GTK3_ENABLED
    g_object_class_override_property(gobject_class, PROP_HADJUSTMENT,
                                     "hadjustment");
    g_object_class_override_property(gobject_class, PROP_HSCROLL_POLICY,
                                     "hscroll-policy");
    g_object_class_override_property(gobject_class, PROP_VADJUSTMENT,
                                     "vadjustment");
    g_object_class_override_property(gobject_class, PROP_VSCROLL_POLICY,
                                     "vscroll-policy");
#endif
}

static void
adg_gtk_layout_init(AdgGtkLayout *layout)
{
    AdgGtkLayoutPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(layout,
                                                            ADG_GTK_TYPE_LAYOUT,
                                                            AdgGtkLayoutPrivate);

    data->hadjustment = NULL;
    data->vadjustment = NULL;
    data->policy_stored = FALSE;
    data->viewport.is_defined = FALSE;

    layout->data = data;
}


/**
 * adg_gtk_layout_new:
 *
 * Creates a new empty #AdgGtkLayout. The widget is useful only after
 * an #AdgCanvas has been added either using the #AdgGtkArea:canvas
 * property or with adg_gtk_area_set_canvas().
 *
 * Returns: (transfer full): the newly created widget.
 *
 * Since: 1.0
 **/
GtkWidget *
adg_gtk_layout_new(void)
{
    return g_object_new(ADG_GTK_TYPE_LAYOUT, NULL);
}

/**
 * adg_gtk_layout_new_with_canvas:
 * @canvas: the #AdgCanvas shown by this widget
 *
 * Creates a new #AdgGtkLayout and sets the #AdgGtkArea:canvas
 * property to @canvas.
 *
 * Returns: (transfer full): the newly created widget.
 *
 * Since: 1.0
 **/
GtkWidget *
adg_gtk_layout_new_with_canvas(AdgCanvas *canvas)
{
    g_return_val_if_fail(ADG_IS_CANVAS(canvas), NULL);

    return g_object_new(ADG_GTK_TYPE_LAYOUT, "canvas", canvas, NULL);
}

/**
 * adg_gtk_layout_set_hadjustment:
 * @layout: an #AdgGtkLayout
 * @hadjustment: the new adjustment
 *
 * Sets the new horizontal adjustment for @layout to @hadjustment.
 * The old adjustment, if present, is unreferenced.
 *
 * This is basically the same as manually setting the
 * #GtkScrollable:hadjustment property with g_object_set().
 *
 * Since: 1.0
 **/
void
adg_gtk_layout_set_hadjustment(AdgGtkLayout *layout,
                               GtkAdjustment *hadjustment)
{
    g_return_if_fail(ADG_GTK_IS_LAYOUT(layout));
    g_object_set(layout, "hadjustment", hadjustment, NULL);
}

/**
 * adg_gtk_layout_get_hadjustment:
 * @layout: an #AdgGtkLayout
 *
 * Retrieves the current horizontal adjustment of @layout.
 *
 * The returned alignment is owned by @layout and should
 * not be modified or freed.
 *
 * Returns: (transfer none): the alignment of @layout.
 *
 * Since: 1.0
 **/
GtkAdjustment *
adg_gtk_layout_get_hadjustment(AdgGtkLayout *layout)
{
    AdgGtkLayoutPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_LAYOUT(layout), NULL);

    data = layout->data;

    return data->hadjustment;
}

/**
 * adg_gtk_layout_set_vadjustment:
 * @layout: an #AdgGtkLayout
 * @vadjustment: the new adjustment
 *
 * Sets the new vertical adjustment for @layout to @vadjustment.
 * The old adjustment, if present, is unreferenced.
 *
 * This is basically the same as manually setting the
 * #GtkScrollable:vadjustment property with g_object_set().
 *
 * Since: 1.0
 **/
void
adg_gtk_layout_set_vadjustment(AdgGtkLayout *layout,
                               GtkAdjustment *vadjustment)
{
    g_return_if_fail(ADG_GTK_IS_LAYOUT(layout));
    g_object_set(layout, "vadjustment", vadjustment, NULL);
}

/**
 * adg_gtk_layout_get_vadjustment:
 * @layout: an #AdgGtkLayout
 *
 * Retrieves the current vertical adjustment of @layout.
 *
 * The returned alignment is owned by @layout and should
 * not be modified or freed.
 *
 * Returns: (transfer none): the alignment of @layout.
 *
 * Since: 1.0
 **/
GtkAdjustment *
adg_gtk_layout_get_vadjustment(AdgGtkLayout *layout)
{
    AdgGtkLayoutPrivate *data;

    g_return_val_if_fail(ADG_GTK_IS_LAYOUT(layout), NULL);

    data = layout->data;

    return data->vadjustment;
}
