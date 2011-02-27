/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 * The #AdgAlignment is an entity that can contains more sub-entities,
 * much in the same way as the #AdgContainer does, but allowing the
 * alignment of the content with an arbitrary fraction of the boundaring
 * box of the content itsself.
 *
 * To specify the alignment fraction, use adg_alignment_set_factor() and
 * related methods or directly set the #AdgAlignment:factor property.
 * For example, to center the children either in x and y, you can call
 * adg_alignment_set_factor_explicit(alignment, 0.5, 0.5). To align them
 * on the right, specify a (0, 1) factor.
 *
 * The displacement is done by modifing the global matrix at the end of
 * the arrange method.
 **/

/**
 * AdgAlignment:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-internal.h"
#include "adg-entity.h"
#include "adg-container.h"

#include "adg-alignment.h"
#include "adg-alignment-private.h"


#define _ADG_OLD_ENTITY_CLASS  ((AdgEntityClass *) adg_alignment_parent_class)


G_DEFINE_TYPE(AdgAlignment, adg_alignment, ADG_TYPE_CONTAINER)

enum {
    PROP_0,
    PROP_FACTOR
};


static void             _adg_get_property       (GObject        *object,
                                                 guint           param_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void             _adg_set_property       (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void             _adg_arrange            (AdgEntity      *entity);


static void
adg_alignment_class_init(AdgAlignmentClass *klass)
{
    GObjectClass *gobject_class;
    AdgEntityClass *entity_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;
    entity_class = (AdgEntityClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgAlignmentPrivate));

    gobject_class->get_property = _adg_get_property;
    gobject_class->set_property = _adg_set_property;

    entity_class->arrange = _adg_arrange;

    param = g_param_spec_boxed("factor",
                               P_("Factor"),
                               P_("Portion of extents, either in x and y, the content will be displaced: a (0.5, 0.5) factor means the origin is the middle point of the extents"),
                               ADG_TYPE_PAIR,
                               G_PARAM_READWRITE);
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
_adg_get_property(GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec)
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
_adg_set_property(GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec)
{
    AdgAlignmentPrivate *data = ((AdgAlignment *) object)->data;

    switch (prop_id) {
    case PROP_FACTOR:
        adg_pair_copy(&data->factor, g_value_get_boxed(value));
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
 * Returns: the newly created alignment or %NULL on errors
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
adg_alignment_set_factor(AdgAlignment *alignment, const AdgPair *factor)
{
    g_return_if_fail(ADG_IS_ALIGNMENT(alignment));
    g_object_set(alignment, "factor", factor, NULL);
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


static void
_adg_arrange(AdgEntity *entity)
{
    const CpmlExtents *extents;
    AdgMatrix old_map;

    adg_matrix_copy(&old_map, adg_entity_get_global_map(entity));
    adg_entity_set_global_map(entity, adg_matrix_identity());
    adg_entity_global_changed(entity);

    if (_ADG_OLD_ENTITY_CLASS->arrange)
        _ADG_OLD_ENTITY_CLASS->arrange(entity);

    extents = adg_entity_get_extents(entity);

    /* The children are displaced only if the extents are valid */
    if (extents->is_defined) {
        AdgAlignmentPrivate *data;
        AdgMatrix unglobal, map;

        data = ((AdgAlignment *) entity)->data;
        adg_matrix_copy(&unglobal, adg_entity_get_global_matrix(entity));
        cairo_matrix_invert(&unglobal);
        cairo_matrix_init_translate(&map,
                                    -extents->size.x * data->factor.x,
                                    -extents->size.y * data->factor.y);
        cairo_matrix_transform_distance(&unglobal, &map.x0, &map.y0);
        adg_matrix_transform(&map, &old_map, ADG_TRANSFORM_AFTER);

        adg_entity_set_global_map(entity, &map);
        adg_entity_global_changed(entity);

        /* Here there is room for improvements: this second arrange()
         * phase could probably be avoided... */
        if (_ADG_OLD_ENTITY_CLASS->arrange)
            _ADG_OLD_ENTITY_CLASS->arrange(entity);
    }

    adg_entity_set_global_map(entity, &old_map);
}
