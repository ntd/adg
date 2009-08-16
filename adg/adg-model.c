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
 * SECTION:adg-model
 * @short_description: The base class of the ADG model infrastructure
 *
 * A model is a conceptual representation of something. From an ADG
 * user point of view, it is a collection of data and rules that
 * defines how an object should be represented on a drawing.
 *
 * Because #AdgModel instances are only a conceptual idea, they are
 * not renderable (that is, #AdgModel is not derived from #AdgEntity).
 * Instead, it must be passed as subject to entities such as AdgStroke
 * or AdgHatch.
 **/

/**
 * AdgModel:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-model.h"
#include "adg-model-private.h"
#include "adg-intl.h"


enum {
    PROP_0
};

enum {
    CHANGED,
    LAST_SIGNAL
};


static void     get_property            (GObject        *object,
                                         guint           prop_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
static void     changed                 (AdgModel       *model);

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_ABSTRACT_TYPE(AdgModel, adg_model, G_TYPE_OBJECT);


static void
adg_model_class_init(AdgModelClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgModelPrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    klass->changed = changed;

    /**
     * AdgModel::changed:
     * @model: an #AdgModel
     *
     * Notificates that the model has changed. By default, the model
     * cache is invalidated.
     **/
    signals[CHANGED] = g_signal_new("changed", ADG_TYPE_MODEL,
                                     G_SIGNAL_RUN_LAST|G_SIGNAL_NO_RECURSE,
                                     G_STRUCT_OFFSET(AdgModelClass, changed),
                                     NULL, NULL,
                                     g_cclosure_marshal_VOID__VOID,
                                     G_TYPE_NONE, 0);
}

static void
adg_model_init(AdgModel *model)
{
    AdgModelPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(model, ADG_TYPE_MODEL,
                                                        AdgModelPrivate);
    model->data = data;
}

static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    /* TODO: this is only a placeholder */

    switch (prop_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    /* TODO: this is only a placeholder */

    switch (prop_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_model_changed:
 * @model: an #AdgModel
 *
 * Emits the "changed" signal on @model.
 *
 * This function is only useful in model implementations.
 */
void
adg_model_changed(AdgModel *model)
{
    g_return_if_fail(ADG_IS_MODEL(model));

    g_signal_emit(model, signals[CHANGED], 0);
}


static void
changed(AdgModel *model)
{
}
