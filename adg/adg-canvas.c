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
 * SECTION:adg-canvas
 * @short_description: The drawing container
 *
 * This container represents the object where the rendering process draws. All
 * the drawing must have a canvas, and only one, as master parent (as all the
 * #GtkWidget must have a #GtkWindow).
 *
 * Internally, the target is mantained as a #cairo_t context pointer.
 */

/**
 * AdgCanvas:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-canvas.h"
#include "adg-canvas-private.h"
#include "adg-line-style.h"
#include "adg-font-style.h"
#include "adg-arrow-style.h"
#include "adg-intl.h"


static AdgStyle *       context_filler  (AdgStyleClass  *style_class,
                                         gpointer        user_data);


G_DEFINE_TYPE(AdgCanvas, adg_canvas, ADG_TYPE_CONTAINER);


static void
adg_canvas_class_init(AdgCanvasClass *klass)
{
    g_type_class_add_private(klass, sizeof(AdgCanvasPrivate));
}

static void
adg_canvas_init(AdgCanvas *canvas)
{
    AdgContext *context;
    AdgCanvasPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(canvas,
                                                         ADG_TYPE_CANVAS,
                                                         AdgCanvasPrivate);

    canvas->data = data;

    context = adg_context_new(context_filler, NULL);
    adg_entity_set_context((AdgEntity *) canvas, context);
    g_object_unref(context);
}


/**
 * adg_canvas_new:
 *
 * Creates a new empty canvas object.
 *
 * Return value: the canvas
 **/
AdgCanvas *
adg_canvas_new(void)
{
    return g_object_new(ADG_TYPE_CANVAS, NULL);
}


static AdgStyle *
context_filler(AdgStyleClass *style_class, gpointer user_data)
{
    return adg_style_get_default(style_class);
}
