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
 * SECTION:title-block
 * @title: AdgTitleBlock
 * @short_description: A title block entity
 *
 * Title blocks are commonly used in technical drawings to include
 * additional information not strictly related to physical dimensions,
 * such as title, material of the represented part, special treatments,
 * date and scale etc.
 *
 * Actually this entity is only a place-holder: it will be implemented
 * properly in a 0.6.x release, after having AdgToyTable in place.
 **/

/**
 * AdgTitleBlock:
 *
 * All fields are privates and should not be used directly.
 * Use its public methods instead.
 **/


#include "adg-title-block.h"
#include "adg-title-block-private.h"
#include "adg-intl.h"


enum {
    PROP_0,
    PROP_NAME,
    PROP_MATERIAL,
    PROP_TREATMENT
};


static void     finalize                (GObject        *object);
static void     get_property            (GObject        *object,
                                         guint           prop_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
static void     set_property            (GObject        *object,
                                         guint           prop_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);


G_DEFINE_TYPE(AdgTitleBlock, adg_title_block, ADG_TYPE_ENTITY);


static void
adg_title_block_class_init(AdgTitleBlockClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *param;

    gobject_class = (GObjectClass *) klass;

    g_type_class_add_private(klass, sizeof(AdgTitleBlockPrivate));

    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    gobject_class->finalize = finalize;

    param = g_param_spec_string("name",
                                P_("Part Name"),
                                P_("Descriptive name of this part"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NAME, param);

    param = g_param_spec_string("material",
                                P_("Material"),
                                P_("Material this part is done with"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MATERIAL, param);

    param = g_param_spec_string("treatment",
                                P_("Treatment"),
                                P_("Treatment this part must receive"),
                                NULL, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TREATMENT, param);
}

static void
adg_title_block_init(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data = G_TYPE_INSTANCE_GET_PRIVATE(title_block,
                                                             ADG_TYPE_TITLE_BLOCK,
                                                             AdgTitleBlockPrivate);
    data->name = NULL;
    data->material = NULL;
    data->treatment = NULL;

    title_block->data = data;
}

static void
finalize(GObject *object)
{
    AdgTitleBlock *title_block;
    AdgTitleBlockPrivate *data;
    GObjectClass *object_class;

    title_block = (AdgTitleBlock *) object;
    data = title_block->data;
    object_class = (GObjectClass *) adg_title_block_parent_class;

    g_free(data->name);
    g_free(data->material);
    g_free(data->treatment);

    if (object_class->finalize != NULL)
        object_class->finalize(object);
}


static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgTitleBlockPrivate *data = ((AdgTitleBlock *) object)->data;

    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, data->name);
        break;
    case PROP_MATERIAL:
        g_value_set_string(value, data->material);
        break;
    case PROP_TREATMENT:
        g_value_set_string(value, data->treatment);
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
    AdgTitleBlockPrivate *data = ((AdgTitleBlock *) object)->data;

    switch (prop_id) {
    case PROP_NAME:
        g_free(data->name);
        data->name = g_value_dup_string(value);
        break;
    case PROP_MATERIAL:
        g_free(data->material);
        data->material = g_value_dup_string(value);
        break;
    case PROP_TREATMENT:
        g_free(data->treatment);
        data->treatment = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


/**
 * adg_title_block_get_name:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the descriptive name associated to this title block.
 * The title block name usually represents what is commonly
 * referred as "title of the drawing".
 *
 * Return value: a copy of the title block name: it must be freed
 *               with g_free() when no longer needed
 **/
gchar *
adg_title_block_get_name(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return g_strdup(data->name);
}

/**
 * adg_title_block_set_name:
 * @title_block: an #AdgTitleBlock entity
 * @name: the new name
 *
 * Sets a new name on the title block.
 **/
void
adg_title_block_set_name(AdgTitleBlock *title_block, const gchar *name)
{
    AdgTitleBlockPrivate *data;

    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));

    data = title_block->data;
    g_free(data->name);
    data->name = g_strdup(name);

    g_object_notify((GObject *) title_block, "name");
}


/**
 * adg_title_block_get_material:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the material (a descriptive name) associated to this title
 * block. This property is not always significative: on drawings
 * representing more than one part (such as assemblies) the material
 * item has no meaning.
 *
 * Return value: a copy of the material name: it must be freed
 *               with g_free() when no longer needed
 **/
gchar *
adg_title_block_get_material(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return g_strdup(data->material);
}

/**
 * adg_title_block_set_material:
 * @title_block: an #AdgTitleBlock entity
 * @name: the new material
 *
 * Sets a new material on the title block.
 **/
void
adg_title_block_set_material(AdgTitleBlock *title_block,
                             const gchar *material)
{
    AdgTitleBlockPrivate *data;

    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));

    data = title_block->data;
    g_free(data->material);
    data->material = g_strdup(material);
    g_object_notify((GObject *) title_block, "material");
}


/**
 * adg_title_block_get_treatment:
 * @title_block: an #AdgTitleBlock entity
 *
 * Gets the treatment (a descriptive name) associated to this title
 * block. As for :material property, also the treatment
 * should be set only when applicable.
 *
 * Return value: a copy of the treatment description: it must be freed
 *               with g_free() when no longer needed
 **/
gchar *
adg_title_block_get_treatment(AdgTitleBlock *title_block)
{
    AdgTitleBlockPrivate *data;

    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    data = title_block->data;

    return g_strdup(data->treatment);
}

/**
 * adg_title_block_set_treatment:
 * @title_block: an #AdgTitleBlock entity
 * @name: the new treatment
 *
 * Sets a new treatment on the title block.
 **/
void
adg_title_block_set_treatment(AdgTitleBlock *title_block,
                              const gchar *treatment)
{
    AdgTitleBlockPrivate *data;

    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));

    data = title_block->data;
    g_free(data->treatment);
    data->treatment = g_strdup(treatment);
    g_object_notify((GObject *) title_block, "treatment");
}
