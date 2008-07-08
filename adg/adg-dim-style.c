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
 * SECTION:dim-style
 * @title: AdgDimStyle
 * @short_description: Dimension style related stuff
 *
 * Contains parameters on how to build dimensions such as three font styles
 * (quote, tolerance and notes), line style, offsets of the various dimension
 * components etc...
 */

#include "adg-dim-style.h"
#include "adg-dim-style-private.h"
#include "adg-font-style.h"
#include "adg-line-style.h"
#include "adg-arrow-style.h"
#include "adg-intl.h"
#include "adg-util.h"

#define PARENT_CLASS ((AdgStyleClass *) adg_dim_style_parent_class)


enum
{
  PROP_0,
  PROP_QUOTE_STYLE,
  PROP_TOLERANCE_STYLE,
  PROP_NOTE_STYLE,
  PROP_LINE_STYLE,
  PROP_ARROW_STYLE,
  PROP_FROM_OFFSET,
  PROP_TO_OFFSET,
  PROP_BASELINE_SPACING,
  PROP_QUOTE_OFFSET,
  PROP_TOLERANCE_OFFSET,
  PROP_TOLERANCE_SPACING,
  PROP_NOTE_OFFSET,
  PROP_NUMBER_FORMAT,
  PROP_NUMBER_TAG
};


static void	get_property		(GObject	*object,
					 guint		 prop_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);


G_DEFINE_TYPE (AdgDimStyle, adg_dim_style, ADG_TYPE_STYLE)


static void
adg_dim_style_class_init (AdgDimStyleClass *klass)
{
  GObjectClass *gobject_class;
  GParamSpec   *param;

  gobject_class = (GObjectClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgDimStylePrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;

  param = g_param_spec_object ("quote-style",
			       P_("Quote Style"),
			       P_("Font style for the quote"),
			       ADG_TYPE_STYLE,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_QUOTE_STYLE, param);

  param = g_param_spec_object ("tolerance-style",
			       P_("Tolerance Style"),
			       P_("Font style for the tolerances"),
			       ADG_TYPE_STYLE,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TOLERANCE_STYLE, param);

  param = g_param_spec_object ("note-style",
			       P_("Note Style"),
			       P_("Font style for the note (the text after or under the quote)"),
			       ADG_TYPE_STYLE,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NOTE_STYLE, param);

  param = g_param_spec_object ("line-style",
			       P_("Line Style"),
			       P_("Line style for the baseline and the extension lines"),
			       ADG_TYPE_STYLE,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_LINE_STYLE, param);

  param = g_param_spec_object ("arrow-style",
			       P_("Arrow Style"),
			       P_("Arrow style to use on the baseline"),
			       ADG_TYPE_STYLE,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_ARROW_STYLE, param);

  param = g_param_spec_double ("from-offset",
			       P_("From Offset"),
			       P_("Offset (in paper space) of the extension lines from the path to quote"),
			       0., G_MAXDOUBLE, 5.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_FROM_OFFSET, param);

  param = g_param_spec_double ("to-offset",
			       P_("To Offset"),
			       P_("How many extend (in paper space) the extension lines after hitting the baseline"),
			       0., G_MAXDOUBLE, 5.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TO_OFFSET, param);

  param = g_param_spec_double ("baseline-spacing",
			       P_("Baseline Spacing"),
			       P_("Dinstance between two consecutive baselines while stacking dimensions"),
			       0., G_MAXDOUBLE, 30.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_BASELINE_SPACING, param);

  param = g_param_spec_boxed ("quote-offset",
			      P_("Quote Offset"),
			      P_("Used to specify a smooth position for the quote text (in paper space units) taking as reference the perfect compact position (the middle of the baseline on common linear quotes, for instance)"),
			      ADG_TYPE_PAIR,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_QUOTE_OFFSET, param);

  param = g_param_spec_boxed ("tolerance-offset",
			      P_("Tolerance Offset"),
			      P_("Used to specify a smooth position for the tolerance text (in paper space units) taking as reference the perfect compact position"),
			      ADG_TYPE_PAIR,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TOLERANCE_OFFSET, param);

  param = g_param_spec_double ("tolerance-spacing",
			       P_("Tolerance Spacing"),
			       P_("Dinstance between up and down tolerance text"),
			       0., G_MAXDOUBLE, 2.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TOLERANCE_SPACING, param);

  param = g_param_spec_boxed ("note-offset",
			      P_("Note Offset"),
			      P_("Used to specify a smooth position for the note text (in paper space units) taking as reference the perfect compact position"),
			      ADG_TYPE_PAIR,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NOTE_OFFSET, param);

  param = g_param_spec_string ("number-format",
			       P_("Number Format"),
			       P_("The format (in printf style) of the numeric component of the quote"),
			       "%-.7g",
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NUMBER_FORMAT, param);

  param = g_param_spec_string ("number-tag",
			       P_("Number Tag"),
			       P_("The tag to substitute in pattern quote"),
			       "<>",
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NUMBER_TAG, param);
}

static void
adg_dim_style_init (AdgDimStyle *dim_style)
{
  AdgDimStylePrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (dim_style,
							  ADG_TYPE_DIM_STYLE,
							  AdgDimStylePrivate);

  priv->quote_style = adg_font_style_from_id (ADG_FONT_STYLE_QUOTE);
  priv->tolerance_style = adg_font_style_from_id (ADG_FONT_STYLE_TOLERANCE);
  priv->note_style = adg_font_style_from_id (ADG_FONT_STYLE_NOTE);
  priv->line_style = adg_line_style_from_id (ADG_LINE_STYLE_DIM);
  priv->arrow_style = adg_arrow_style_from_id (ADG_ARROW_STYLE_ARROW);
  priv->from_offset = 5.;
  priv->to_offset = 5.;
  priv->baseline_spacing = 30.;
  priv->quote_offset.x = 0.;
  priv->quote_offset.y = -3.;
  priv->tolerance_offset.x = +5.;
  priv->tolerance_offset.y = -4.;
  priv->tolerance_spacing = 2.;
  priv->note_offset.x = +5.;
  priv->note_offset.y =  0.;
  priv->number_format = g_strdup ("%-.7g");
  priv->number_tag = g_strdup ("<>");

  dim_style->priv = priv;
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgDimStyle *dim_style = (AdgDimStyle *) object;

  switch (prop_id)
    {
    case PROP_QUOTE_STYLE:
      g_value_set_object (value, dim_style->priv->quote_style);
      break;
    case PROP_TOLERANCE_STYLE:
      g_value_set_object (value, dim_style->priv->tolerance_style);
      break;
    case PROP_NOTE_STYLE:
      g_value_set_object (value, dim_style->priv->note_style);
      break;
    case PROP_LINE_STYLE:
      g_value_set_object (value, dim_style->priv->line_style);
      break;
    case PROP_ARROW_STYLE:
      g_value_set_object (value, dim_style->priv->arrow_style);
      break;
    case PROP_FROM_OFFSET:
      g_value_set_double (value, dim_style->priv->from_offset);
      break;
    case PROP_TO_OFFSET:
      g_value_set_double (value, dim_style->priv->to_offset);
      break;
    case PROP_BASELINE_SPACING:
      g_value_set_double (value, dim_style->priv->baseline_spacing);
      break;
    case PROP_QUOTE_OFFSET:
      g_value_set_boxed (value, &dim_style->priv->quote_offset);
      break;
    case PROP_TOLERANCE_OFFSET:
      g_value_set_boxed (value, &dim_style->priv->tolerance_offset);
      break;
    case PROP_TOLERANCE_SPACING:
      g_value_set_double (value, dim_style->priv->tolerance_spacing);
      break;
    case PROP_NOTE_OFFSET:
      g_value_set_boxed (value, &dim_style->priv->note_offset);
      break;
    case PROP_NUMBER_FORMAT:
      g_value_set_string (value, dim_style->priv->number_format);
      break;
    case PROP_NUMBER_TAG:
      g_value_set_string (value, dim_style->priv->number_tag);
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
  AdgDimStyle *dim_style = (AdgDimStyle *) object;

  switch (prop_id)
    {
    case PROP_QUOTE_STYLE:
      dim_style->priv->quote_style = g_value_get_object (value);
      break;
    case PROP_TOLERANCE_STYLE:
      dim_style->priv->tolerance_style = g_value_get_object (value);
      break;
    case PROP_NOTE_STYLE:
      dim_style->priv->note_style = g_value_get_object (value);
      break;
    case PROP_LINE_STYLE:
      dim_style->priv->line_style = g_value_get_object (value);
      break;
    case PROP_ARROW_STYLE:
      dim_style->priv->arrow_style = g_value_get_object (value);
      break;
    case PROP_FROM_OFFSET:
      dim_style->priv->from_offset = g_value_get_double (value);
      break;
    case PROP_TO_OFFSET:
      dim_style->priv->to_offset = g_value_get_double (value);
      break;
    case PROP_BASELINE_SPACING:
      dim_style->priv->baseline_spacing = g_value_get_double (value);
      break;
    case PROP_QUOTE_OFFSET:
      cpml_pair_copy (&dim_style->priv->quote_offset,
		      g_value_get_boxed (value));
      break;
    case PROP_TOLERANCE_OFFSET:
      cpml_pair_copy (&dim_style->priv->tolerance_offset,
		      g_value_get_boxed (value));
      break;
    case PROP_TOLERANCE_SPACING:
      dim_style->priv->tolerance_spacing = g_value_get_double (value);
      break;
    case PROP_NOTE_OFFSET:
      cpml_pair_copy (&dim_style->priv->note_offset,
		      g_value_get_boxed (value));
      break;
    case PROP_NUMBER_FORMAT:
      g_free (dim_style->priv->number_format);
      dim_style->priv->number_format = g_value_dup_string (value);
      break;
    case PROP_NUMBER_TAG:
      g_free (dim_style->priv->number_tag);
      dim_style->priv->number_tag = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


AdgStyle *
adg_dim_style_from_id (AdgDimStyleId id)
{
  static AdgStyle **builtins = NULL;

  if G_UNLIKELY (builtins == NULL)
    {
      builtins = g_new (AdgStyle *, ADG_DIM_STYLE_LAST);

      /* The default is yet the ISO style */
      builtins[ADG_DIM_STYLE_ISO] = g_object_new (ADG_TYPE_DIM_STYLE, NULL);
    }

  g_return_val_if_fail (id < ADG_DIM_STYLE_LAST, NULL);
  return builtins[id];
}
