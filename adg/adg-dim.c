/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


/**
 * SECTION:adgdim
 * @title: AdgDim
 * @short_description: Root abstract class for all dimension entities
 *
 * The #AdgDim class is the base stub of all the dimension entities.
 */

#include "adg-dim.h"
#include "adg-dim-private.h"
#include "adg-util.h"
#include "adg-intl.h"

#define PARENT_CLASS ((AdgEntityClass *) adg_dim_parent_class)


enum
{
  PROP_0,
  PROP_DIM_STYLE,
  PROP_REF1,
  PROP_REF2,
  PROP_POS1,
  PROP_POS2,
  PROP_LEVEL,
  PROP_LABEL,
  PROP_TOLERANCE_UP,
  PROP_TOLERANCE_DOWN,
  PROP_NOTE
};


static void	finalize		(GObject	*object);
static void	get_property		(GObject	*object,
					 guint		 param_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 param_id,
					 const GValue	*value,
					 GParamSpec	*pspec);
static void	invalidate		(AdgDim		*dim);
static void	invalidate_quote	(AdgDim		*dim);
static const AdgDimStyle *
		get_dim_style		(AdgEntity	*entity);
static void	set_dim_style		(AdgEntity	*entity,
					 AdgDimStyle	*dim_style);
static void	update			(AdgEntity	*entity,
					 gboolean	 recursive);
static void	outdate			(AdgEntity	*entity,
					 gboolean	 recursive);
static gchar *	default_label		(AdgDim		*dim);
static void	label_layout		(AdgDim		*dim,
					 cairo_t	*cr);


G_DEFINE_ABSTRACT_TYPE (AdgDim, adg_dim, ADG_TYPE_ENTITY);


static void
adg_dim_class_init (AdgDimClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgDimPrivate));

  gobject_class->finalize = finalize;
  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  entity_class->get_dim_style = get_dim_style;
  entity_class->set_dim_style = set_dim_style;
  entity_class->update = update;
  entity_class->outdate = outdate;

  klass->default_label = default_label;
  klass->label_layout = label_layout;

  param = g_param_spec_boxed ("dim-style",
                              P_("Dimension Style"),
                              P_("Dimension style to use while rendering"),
                              ADG_TYPE_DIM_STYLE,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_DIM_STYLE, param);

  param = g_param_spec_boxed ("ref1",
                              P_("Reference 1"),
                              P_("First reference point of the dimension"),
                              ADG_TYPE_PAIR,
                              G_PARAM_READWRITE|G_PARAM_CONSTRUCT);
  g_object_class_install_property (gobject_class, PROP_REF1, param);

  param = g_param_spec_boxed ("ref2",
                              P_("Reference 2"),
                              P_("Second reference point of the dimension"),
                              ADG_TYPE_PAIR,
                              G_PARAM_READWRITE|G_PARAM_CONSTRUCT);
  g_object_class_install_property (gobject_class, PROP_REF2, param);

  param = g_param_spec_boxed ("pos1",
                              P_("Position 1"),
                              P_("First position point: it will be computed with the level property to get the real dimension position"),
                              ADG_TYPE_PAIR,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_POS1, param);

  param = g_param_spec_boxed ("pos2",
                              P_("Position 2"),
                              P_("Second position point: it will be computed with the level property to get the real dimension position"),
                              ADG_TYPE_PAIR,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_POS2, param);

  param = g_param_spec_double ("level",
			       P_("Level"),
			       P_("The dimension level, that is the factor to multiply dim_style->baseline_spacing to get the offset (in device units) from pos1..pos2 where render the dimension baseline"),
			       -G_MAXDOUBLE, G_MAXDOUBLE, 1.0,
			       G_PARAM_READWRITE|G_PARAM_CONSTRUCT);
  g_object_class_install_property (gobject_class, PROP_LEVEL, param);

  param = g_param_spec_string ("label",
                               P_("Label"),
                               P_("The label to display: set to NULL to get the default quote"),
                               NULL,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_LABEL, param);

  param = g_param_spec_string ("tolerance-up",
                               P_("Up Tolerance"),
                               P_("The up tolerance of the quote: set to NULL to suppress"),
                               NULL,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TOLERANCE_UP, param);

  param = g_param_spec_string ("tolerance-down",
                               P_("Down Tolerance"),
                               P_("The down tolerance of the quote: set to NULL to suppress"),
                               NULL,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TOLERANCE_DOWN, param);

  param = g_param_spec_string ("note",
                               P_("Note"),
                               P_("A custom note appended to the dimension label"),
                               NULL,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NOTE, param);
}

static void
adg_dim_init (AdgDim *dim)
{
  AdgDimPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (dim, ADG_TYPE_DIM,
						     AdgDimPrivate);

  priv->dim_style = adg_dim_style_from_id (ADG_DIM_STYLE_ISO);

  adg_pair_unset (&priv->ref1);
  adg_pair_unset (&priv->ref2);
  adg_pair_unset (&priv->pos1);
  adg_pair_unset (&priv->pos2);
  priv->level = 1.0;
  priv->label = NULL;
  priv->tolerance_up = NULL;
  priv->tolerance_down = NULL;
  priv->note = NULL;

  dim->priv = priv;
  invalidate (dim);
}

static void
finalize (GObject *object)
{
  AdgDimPrivate *priv = ((AdgDim *) object)->priv;

  g_free (priv->label);
  g_free (priv->tolerance_up);
  g_free (priv->tolerance_down);

  ((GObjectClass *) PARENT_CLASS)->finalize (object);
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgDim *dim = (AdgDim *) object;

  switch (prop_id)
    {
    case PROP_DIM_STYLE:
      g_value_set_boxed (value, dim->priv->dim_style);
      break;
    case PROP_REF1:
      g_value_set_boxed (value, &dim->priv->ref1);
      break;
    case PROP_REF2:
      g_value_set_boxed (value, &dim->priv->ref2);
      break;
    case PROP_POS1:
      g_value_set_boxed (value, &dim->priv->pos1);
      break;
    case PROP_POS2:
      g_value_set_boxed (value, &dim->priv->pos1);
      break;
    case PROP_LEVEL:
      g_value_set_double (value, dim->priv->level);
      break;
    case PROP_LABEL:
      g_value_set_string (value, dim->priv->label);
      break;
    case PROP_TOLERANCE_UP:
      g_value_set_string (value, dim->priv->tolerance_up);
      break;
    case PROP_TOLERANCE_DOWN:
      g_value_set_string (value, dim->priv->tolerance_down);
      break;
    case PROP_NOTE:
      g_value_set_string (value, dim->priv->note);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
set_property (GObject      *object,
	      guint         prop_id,
	      const GValue *value,
	      GParamSpec   *pspec)
{
  AdgDim *dim = (AdgDim *) object;
  
  switch (prop_id)
    {
    case PROP_DIM_STYLE:
      dim->priv->dim_style = g_value_get_boxed (value);
      invalidate (dim);
      break;
    case PROP_REF1:
      adg_pair_set (&dim->priv->ref1, (AdgPair *) g_value_get_boxed (value));
      invalidate (dim);
      break;
    case PROP_REF2:
      adg_pair_set (&dim->priv->ref2, (AdgPair *) g_value_get_boxed (value));
      invalidate (dim);
      break;
    case PROP_POS1:
      adg_pair_set (&dim->priv->pos1, (AdgPair *) g_value_get_boxed (value));
      invalidate (dim);
      break;
    case PROP_POS2:
      adg_pair_set (&dim->priv->pos2, (AdgPair *) g_value_get_boxed (value));
      invalidate (dim);
      break;
    case PROP_LEVEL:
      dim->priv->level = g_value_get_double (value);
      invalidate (dim);
      break;
    case PROP_LABEL:
      g_free (dim->priv->label);
      dim->priv->label = g_value_dup_string (value);
      invalidate_quote (dim);
      break;
    case PROP_TOLERANCE_UP:
      g_free (dim->priv->tolerance_up);
      dim->priv->tolerance_up = g_value_dup_string (value);
      invalidate_quote (dim);
      break;
    case PROP_TOLERANCE_DOWN:
      g_free (dim->priv->tolerance_down);
      dim->priv->tolerance_down = g_value_dup_string (value);
      invalidate_quote (dim);
      break;
    case PROP_NOTE:
      g_free (dim->priv->note);
      dim->priv->note = g_value_dup_string (value);
      invalidate_quote (dim);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
invalidate (AdgDim *dim)
{
  adg_pair_unset (&dim->priv->quote_org);
  dim->priv->quote_angle = ADG_NAN;
  invalidate_quote (dim);
}

static void
invalidate_quote (AdgDim *dim)
{
  adg_pair_unset (&dim->priv->quote_offset);
  adg_pair_unset (&dim->priv->tolerance_up_offset);
  adg_pair_unset (&dim->priv->tolerance_down_offset);
  adg_pair_unset (&dim->priv->note_offset);
}


static const AdgDimStyle *
get_dim_style (AdgEntity *entity)
{
  return ((AdgDim *) entity)->priv->dim_style;
}

static void
set_dim_style (AdgEntity   *entity,
	       AdgDimStyle *dim_style)
{
  ((AdgDim *) entity)->priv->dim_style = dim_style;
  g_object_notify (G_OBJECT (entity), "dim-style");
}

static void
update (AdgEntity *entity,
	gboolean   recursive)
{
  AdgDim *dim = (AdgDim *) entity;
 
  if (dim->priv->label == NULL)
    {
      dim->priv->label = ADG_DIM_GET_CLASS (dim)->default_label (dim);
      invalidate_quote (dim);
      g_object_notify ((GObject *) dim, "label");
    }

  PARENT_CLASS->update (entity, recursive);
}

static void
outdate (AdgEntity *entity,
	 gboolean   recursive)
{
  invalidate ((AdgDim *) entity);
  PARENT_CLASS->outdate (entity, recursive);
}


static gchar *
default_label (AdgDim *dim)
{
  g_warning ("AdgDim::default_label not implemented for `%s'", g_type_name (G_TYPE_FROM_INSTANCE (dim)));
  return g_strdup ("undef");
}

static void
label_layout (AdgDim  *dim,
	      cairo_t *cr)
{
  AdgPair              offset;
  AdgPair              cp;
  cairo_text_extents_t extents;

  /* Compute the label */
  adg_font_style_apply (dim->priv->dim_style->label_style, cr);

  cairo_text_extents (cr, dim->priv->label, &extents);
  cairo_user_to_device_distance (cr, &extents.width, &extents.height);
  adg_pair_set_explicit (&cp, extents.width, -extents.height / 2.0);

  /* Compute the tolerances */
  if (dim->priv->tolerance_up != NULL || dim->priv->tolerance_down != NULL)
    {
      double width;
      double midspacing;

      adg_font_style_apply (dim->priv->dim_style->tolerance_style, cr);

      width = 0.0;
      midspacing = dim->priv->dim_style->tolerance_spacing / 2.0;
      adg_pair_set (&offset, &dim->priv->dim_style->tolerance_offset);
      cp.x += offset.x;

      if (dim->priv->tolerance_up != NULL)
        {
          cairo_text_extents (cr, dim->priv->tolerance_up, &extents);
          cairo_user_to_device_distance (cr, &extents.width, &extents.height);
          adg_pair_set_explicit (&dim->priv->tolerance_up_offset,
                                 cp.x,
                                 cp.y + offset.y - midspacing);
          width = extents.width;
        }

      if (dim->priv->tolerance_down != NULL)
        {
          cairo_text_extents (cr, dim->priv->tolerance_down, &extents);
          cairo_user_to_device_distance (cr, &extents.width, &extents.height);
          adg_pair_set_explicit (&dim->priv->tolerance_down_offset,
                                 cp.x,
                                 cp.y + offset.y + midspacing + extents.height);
          if (extents.width > width)
            width = extents.width;
        }

      cp.x += width;
    }

  /* Compute the note */
  if (dim->priv->note != NULL)
    {
      adg_font_style_apply (dim->priv->dim_style->note_style, cr);

      adg_pair_set (&offset, &dim->priv->dim_style->note_offset);
      cp.x += offset.x;

      cairo_text_extents (cr, dim->priv->note, &extents);
      cairo_user_to_device_distance (cr, &extents.width, &extents.height);
      adg_pair_set_explicit (&dim->priv->note_offset,
                             cp.x,
                             cp.y + offset.y + extents.height / 2.0);

      cp.x += extents.width;
    }

  /* Calculating the offsets */
  offset.x = dim->priv->dim_style->quote_offset.x - cp.x / 2.0;
  offset.y = dim->priv->dim_style->quote_offset.y;

  adg_pair_set (&dim->priv->quote_offset, &offset);

  if (adg_pair_is_set (&dim->priv->tolerance_up_offset))
    adg_pair_add (&dim->priv->tolerance_up_offset, &offset);

  if (adg_pair_is_set (&dim->priv->tolerance_down_offset))
    adg_pair_add (&dim->priv->tolerance_down_offset, &offset);

  if (adg_pair_is_set (&dim->priv->note_offset))
    adg_pair_add (&dim->priv->note_offset, &offset);
}


const AdgPair *
adg_dim_get_ref1 (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return &dim->priv->ref1;
}

const AdgPair *
adg_dim_get_ref2 (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return &dim->priv->ref2;
}

void
adg_dim_set_ref (AdgDim        *dim,
                 const AdgPair *ref1,
                 const AdgPair *ref2)
{
  GObject *object;

  g_return_if_fail (ADG_IS_DIM (dim));

  object = G_OBJECT (dim);

  if (ref1 != NULL)
    {
      dim->priv->ref1 = *ref1;
      g_object_notify (object, "ref1");
    }

  if (ref2 != NULL)
    {
      dim->priv->ref2 = *ref2;
      g_object_notify (object, "ref2");
    }
}

void
adg_dim_set_ref_explicit (AdgDim *dim,
                          double  ref1_x,
                          double  ref1_y,
                          double  ref2_x,
                          double  ref2_y)
{
  AdgPair ref1;
  AdgPair ref2;

  ref1.x = ref1_x;
  ref1.y = ref1_y;
  ref2.x = ref2_x;
  ref2.y = ref2_y;

  adg_dim_set_ref (dim, &ref1, &ref2);
}

const AdgPair *
adg_dim_get_pos1 (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return &dim->priv->pos1;
}

const AdgPair *
adg_dim_get_pos2 (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return &dim->priv->pos2;
}

void
adg_dim_set_pos (AdgDim  *dim,
                 AdgPair *pos1,
                 AdgPair *pos2)
{
  GObject *object;

  g_return_if_fail (ADG_IS_DIM (dim));

  object = G_OBJECT (dim);
  
  g_object_freeze_notify (object);

  if (pos1 != NULL)
    {
      dim->priv->pos1 = *pos1;
      g_object_notify (object, "pos1");
    }

  if (pos2 != NULL)
    {
      dim->priv->pos2 = *pos2;
      g_object_notify (object, "pos2");
    }

  g_object_thaw_notify (object);
}

void
adg_dim_set_pos_explicit (AdgDim *dim,
                          double  pos1_x,
                          double  pos1_y,
                          double  pos2_x,
                          double  pos2_y)
{
  AdgPair pos1;
  AdgPair pos2;

  pos1.x = pos1_x;
  pos1.y = pos1_y;
  pos2.x = pos2_x;
  pos2.y = pos2_y;

  adg_dim_set_pos (dim, &pos1, &pos2);
}

double
adg_dim_get_level (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), 0.0);

  return dim->priv->level;
}

void
adg_dim_set_level (AdgDim *dim,
                   double  level)
{
  g_return_if_fail (ADG_IS_DIM (dim));

  dim->priv->level = level;
  g_object_notify ((GObject *) dim, "level");
}

const gchar *
adg_dim_get_label (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return dim->priv->label;
}

void
adg_dim_set_label (AdgDim      *dim,
                   const gchar *label)
{
  g_return_if_fail (ADG_IS_DIM (dim));

  g_free (dim->priv->label);
  dim->priv->label = g_strdup (label);
  invalidate_quote (dim);

  g_object_notify ((GObject *) dim, "label");
}

const gchar *
adg_dim_get_tolerance_up (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return dim->priv->tolerance_up;
}

void
adg_dim_set_tolerance_up (AdgDim      *dim,
                          const gchar *tolerance_up)
{
  g_return_if_fail (ADG_IS_DIM (dim));

  g_free (dim->priv->tolerance_up);
  dim->priv->tolerance_down = g_strdup (tolerance_up);
  invalidate_quote (dim);

  g_object_notify ((GObject *) dim, "tolerance-up");
}

const gchar *
adg_dim_get_tolerance_down (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return dim->priv->tolerance_down;
}

void
adg_dim_set_tolerance_down (AdgDim      *dim,
                            const gchar *tolerance_down)
{
  g_return_if_fail (ADG_IS_DIM (dim));

  g_free (dim->priv->tolerance_down);
  dim->priv->tolerance_down = g_strdup (tolerance_down);
  invalidate_quote (dim);

  g_object_notify ((GObject *) dim, "tolerance-down");
}

void
adg_dim_set_tolerances (AdgDim      *dim,
                        const gchar *tolerance_up,
                        const gchar *tolerance_down)
{
  GObject *object;

  g_return_if_fail (ADG_IS_DIM (dim));

  object = G_OBJECT (dim);

  g_object_freeze_notify (object);

  g_free (dim->priv->tolerance_up);
  dim->priv->tolerance_up = g_strdup (tolerance_up);
  g_object_notify (object, "tolerance-up");

  g_free (dim->priv->tolerance_down);
  dim->priv->tolerance_down = g_strdup (tolerance_down);
  g_object_notify (object, "tolerance-down");

  invalidate_quote (dim);

  g_object_thaw_notify (object);
}

const gchar *
adg_dim_get_note (AdgDim *dim)
{
  g_return_val_if_fail (ADG_IS_DIM (dim), NULL);

  return dim->priv->note;
}

void
adg_dim_set_note (AdgDim      *dim,
                  const gchar *note)
{
  g_return_if_fail (ADG_IS_DIM (dim));

  g_free (dim->priv->note);
  dim->priv->note = g_strdup (note);
  invalidate_quote (dim);

  g_object_notify ((GObject *) dim, "note");
}


/**
 * adg_dim_render_quote:
 * @dim: an #AdgDim object
 * @cr: a #cairo_t drawing context
 *
 * Renders the quote of @dim at the @org position.
 *
 * This function is only useful in new dimension implementations.
 */
void
adg_dim_render_quote (AdgDim  *dim,
		      cairo_t *cr)
{
  AdgPair quote_offset;
  AdgPair tolerance_up_offset;
  AdgPair tolerance_down_offset;
  AdgPair note_offset;

  g_return_if_fail (ADG_IS_DIM (dim));
  g_return_if_fail (dim->priv->dim_style != NULL);
  g_return_if_fail (!adg_isnan (dim->priv->quote_angle));
  g_return_if_fail (adg_pair_is_set (&dim->priv->quote_org));

  if (!adg_pair_is_set (&dim->priv->quote_offset))
    ADG_DIM_GET_CLASS (dim)->label_layout (dim, cr);

  adg_pair_set (&quote_offset, &dim->priv->quote_offset);
  cairo_device_to_user_distance (cr, &quote_offset.x, &quote_offset.y);
  adg_pair_set (&tolerance_up_offset, &dim->priv->tolerance_up_offset);
  cairo_device_to_user_distance (cr, &tolerance_up_offset.x, &tolerance_up_offset.y);
  adg_pair_set (&tolerance_down_offset, &dim->priv->tolerance_down_offset);
  cairo_device_to_user_distance (cr, &tolerance_down_offset.x, &tolerance_down_offset.y);

  cairo_save (cr);
  cairo_translate (cr, dim->priv->quote_org.x, dim->priv->quote_org.y);
  cairo_rotate (cr, dim->priv->quote_angle);

  /* Rendering label */
  adg_font_style_apply (dim->priv->dim_style->label_style, cr);
  cairo_move_to (cr, quote_offset.x, quote_offset.y);
  cairo_show_text (cr, dim->priv->label);

  /* Rendering tolerances */
  if (dim->priv->tolerance_up != NULL || dim->priv->tolerance_down != NULL)
    {
      adg_font_style_apply (dim->priv->dim_style->tolerance_style, cr);

      if (dim->priv->tolerance_up != NULL)
        {
          cairo_move_to (cr, tolerance_up_offset.x, tolerance_up_offset.y);
          cairo_show_text (cr, dim->priv->tolerance_up);
        }
      if (dim->priv->tolerance_down != NULL)
        {
          cairo_move_to (cr, tolerance_down_offset.x, tolerance_down_offset.y);
          cairo_show_text (cr, dim->priv->tolerance_down);
        }
    }

  /* Rendering note */
  if (dim->priv->note != NULL)
    {
      cairo_move_to (cr, note_offset.x, note_offset.y);
      cairo_show_text (cr, dim->priv->note);
    }

  cairo_restore (cr);
}
