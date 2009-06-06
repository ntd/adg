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
 * SECTION:title_block
 * @title: AdgTitleBlock
 * @short_description: A title block entity
 *
 * Title blocks are commonly used in technical drawing to include additional
 * information not strictly related to physical dimensions, such as title,
 * material of the represented part, special treatments, date and so on.
 */

#include "adg-title-block.h"
#include "adg-title-block-private.h"
#include "adg-intl.h"

#define PARENT_CLASS ((GObjectClass *) adg_title_block_parent_class)


enum {
    PROP_0,
    PROP_NAME,
    PROP_MATERIAL,
    PROP_TREATMENT
};

enum {
    MODIFIED,
    LAST_SIGNAL
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

static guint title_block_signals[LAST_SIGNAL] = { 0 };


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
    AdgTitleBlockPrivate *priv =
        G_TYPE_INSTANCE_GET_PRIVATE(title_block, ADG_TYPE_TITLE_BLOCK,
                                    AdgTitleBlockPrivate);
    priv->name = NULL;
    priv->material = NULL;
    priv->treatment = NULL;

    title_block->priv = priv;
}

static void
finalize(GObject *object)
{
    AdgTitleBlock *title_block = (AdgTitleBlock *) object;

    g_free(title_block->priv->name);
    g_free(title_block->priv->material);
    g_free(title_block->priv->treatment);

    PARENT_CLASS->finalize(object);
}


static void
get_property(GObject *object,
             guint prop_id, GValue *value, GParamSpec *pspec)
{
    AdgTitleBlockPrivate *priv = ((AdgTitleBlock *) object)->priv;

    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
    case PROP_MATERIAL:
        g_value_set_string(value, priv->material);
        break;
    case PROP_TREATMENT:
        g_value_set_string(value, priv->treatment);
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
    AdgTitleBlock *title_block = ADG_TITLE_BLOCK(object);

    switch (prop_id) {
    case PROP_NAME:
        g_free(title_block->priv->name);
        title_block->priv->name = g_value_dup_string(value);
        break;
    case PROP_MATERIAL:
        g_free(title_block->priv->material);
        title_block->priv->material = g_value_dup_string(value);
        break;
    case PROP_TREATMENT:
        g_free(title_block->priv->treatment);
        title_block->priv->treatment = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


gchar *
adg_title_block_get_name(AdgTitleBlock *title_block)
{
    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    return g_strdup(title_block->priv->name);
}

void
adg_title_block_set_name(AdgTitleBlock *title_block, const gchar *name)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));

    g_free(title_block->priv->name);
    title_block->priv->name = g_strdup(name);
    g_object_notify((GObject *) title_block, "name");
}


gchar *
adg_title_block_get_material(AdgTitleBlock *title_block)
{
    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    return g_strdup(title_block->priv->material);
}

void
adg_title_block_set_material(AdgTitleBlock *title_block,
                             const gchar *material)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));

    g_free(title_block->priv->material);
    title_block->priv->material = g_strdup(material);
    g_object_notify((GObject *) title_block, "material");
}


gchar *
adg_title_block_get_treatment(AdgTitleBlock *title_block)
{
    g_return_val_if_fail(ADG_IS_TITLE_BLOCK(title_block), NULL);

    return g_strdup(title_block->priv->treatment);
}

void
adg_title_block_set_treatment(AdgTitleBlock *title_block, const gchar *treatment)
{
    g_return_if_fail(ADG_IS_TITLE_BLOCK(title_block));

    g_free(title_block->priv->treatment);
    title_block->priv->treatment = g_strdup(treatment);
    g_object_notify((GObject *) title_block, "treatment");
}
