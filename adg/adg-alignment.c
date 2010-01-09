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
 * SECTION:adg-alignment
 * @short_description: Base class for entity that can contain other entities
 *
 * The #AdgAlignment is an entity that can contains more sub-entities.
 * Moreover, it can apply a common transformation to local and/or global
 * maps: see http://adg.entidi.com/tutorial/view/3 for further details.
 **/

/**
 * AdgAlignment:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/

#include "adg-alignment.h"
#include "adg-alignment-private.h"
#include "adg-entity-private.h"
#include "adg-intl.h"

#define PARENT_ENTITY_CLASS  ((AdgEntityClass *) adg_alignment_parent_class)


enum {
    PROP_0,
    PROP_FACTOR
};


static void             get_property            (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             set_property            (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             global_changed          (AdgEntity      *entity);
static void             arrange                 (AdgEntity      *entity);
static gboolean         set_factor              (AdgAlignment   *alignment,
                                                 const AdgPair  *factor);


G_DEFINE_TYPE(AdgAlignment, adg_alignment, ADG_TYPE_CONTAINER);


static void
adg_alignment_class_init(AdgAlignmentClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgAlignmentPrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    entity_class->global_changed = global_changed;
    entity_class->arrange = arrange;

    param = g_param_spec_boxed("factor",
                               P_("Factor"),
                               P_("Portion of extents, either in x and y, the content will be displaced: a (0.5, 0.5) factor means the origin is the middle point of the extents"),
                               ADG_TYPE_PAIR,
                               G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_FACTOR, param);
}

static void
adg_alignment_init(AdgAlignment *alignment)
{
    AdgAlignmentPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(alignment,
                                                            ADG_TYPE_ALIGNMENT,
                                                            AdgAlignmentPrivate);

    data->factor.x = 0;
    data->factor.y = 0;

    alignment->data = data;
}

static void
get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgAlignmentPrivate *data = ((AdgAlignment *) object)->data;

    switch (prop_id) {
    case PROP_FACTOR:
        g_value_set_boxed(value, &data->factor);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object,
             guint prop_id, const GValue *value, GParamSpec *pspec)
{
    AdgAlignment *alignment = (AdgAlignment *) object;

    switch (prop_id) {
    case PROP_FACTOR:
        set_factor(alignment, g_value_get_boxed(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}


/**
 * adg_alignment_new:
 * @factor: the alignment factor
 *
 * Creates a new alignment container with the specified factor.
 *
 * Returns: the newly created alignment
 **/
AdgAlignment *
adg_alignment_new(const AdgPair *factor)
{
    return g_object_new(ADG_TYPE_ALIGNMENT, "factor", factor, NULL);
}

/**
 * adg_alignment_new_explicit:
 * @x_factor: x component of the factor
 * @y_factor: y component of the factor
 *
 * Convenient function that creates a new alignment accepting explicit
 * factor values.
 *
 * Returns: the newly created alignment
 **/
AdgAlignment *
adg_alignment_new_explicit(gdouble x_factor, gdouble y_factor)
{
    AdgPair factor;

    factor.x = x_factor;
    factor.y = y_factor;

    return adg_alignment_new(&factor);
}

/**
 * adg_alignment_get_factor:
 * @alignment: an #AdgAlignment container
 *
 * Gets the value of the #AdgAlignment:factor property. The returned
 * pair is owned by @alignment and must not be modified or freed.
 *
 * Returns: the factor pair
 **/
const AdgPair *
adg_alignment_get_factor(AdgAlignment *alignment)
{
    AdgAlignmentPrivate *data;

    g_return_val_if_fail(ADG_IS_ALIGNMENT(alignment), NULL);

    data = alignment->data;

    return &data->factor;
}

/**
 * adg_alignment_set_factor:
 * @alignment: an #AdgAlignment container
 * @factor: the new factor
 *
 * Sets a the #AdgAlignment:factor property to @factor on @alignment.
 * The factor is applied to the @alignment extents to compute the
 * displacement of the content, providing a way to for instance center
 * the content either vertically or horizontally. A pair factor of
 * (%0.5, %0) means the content will be centered horizontally in
 * reference to the normal flow without @alignment.
 **/
void
adg_alignment_set_factor(AdgAlignment*alignment, const AdgPair *factor)
{
    g_return_if_fail(ADG_IS_ALIGNMENT(alignment));
    g_return_if_fail(factor != NULL);

    if (set_factor(alignment, factor))
        g_object_notify((GObject *) alignment, "factor");
}

/**
 * adg_alignment_set_factor_explicit:
 * @alignment: an #AdgAlignment container
 * @x_factor: x component of the factor
 * @y_factor: y component of the factor
 *
 * Convenient wrapper around adg_alignment_set_factor() that accepts
 * explicit factors instead of an #AdgPair value.
 **/
void
adg_alignment_set_factor_explicit(AdgAlignment *alignment,
                                  gdouble x_factor, gdouble y_factor)
{
    AdgPair factor;

    factor.x = x_factor;
    factor.y = y_factor;

    adg_alignment_set_factor(alignment, &factor);
}


static void
global_changed(AdgEntity *entity)
{
    AdgEntityClass *entity_class;
    AdgEntityPrivate *entity_data;
    CpmlExtents *extents;
    AdgMatrix *global;
    AdgPair shift;

    entity_class = g_type_class_ref(ADG_TYPE_ENTITY);
    entity_data = entity->data;

    /* TODO: do not access private members of AdgEntity! */
    extents = &entity_data->extents;
    global = &entity_data->global.matrix;

    if (extents->is_defined) {
        /* The entities are displaced only if the extents are valid */
        AdgAlignmentPrivate *data = ((AdgAlignment *) entity)->data;

        cpml_pair_copy(&shift, &extents->size);
        cpml_pair_mul(&shift, &data->factor);

        cpml_pair_sub(&extents->org, &shift);
    } else {
        shift.x = 0;
        shift.y = 0;
    }

    if (entity_class->global_changed != NULL)
        entity_class->global_changed(entity);

    g_type_class_unref(entity_class);

    /* The real job: temporarily modify the global matrix
     * to align the contained entities */
    global->x0 -= shift.x;
    global->y0 -= shift.y;

    adg_container_propagate_by_name((AdgContainer *) entity, "global-changed");

    global->x0 += shift.x;
    global->y0 += shift.y;
}

static void
arrange(AdgEntity *entity)
{
    if (PARENT_ENTITY_CLASS->arrange)
        PARENT_ENTITY_CLASS->arrange(entity);

    /* Force a recomputation of the children position */
    adg_entity_global_changed(entity);
}

static gboolean
set_factor(AdgAlignment *alignment, const AdgPair *factor)
{
    AdgAlignmentPrivate *data = alignment->data;

    if (adg_pair_equal(&data->factor, factor))
        return FALSE;

    data->factor = *factor;

    return TRUE;
}
