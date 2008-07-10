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
 * Contains parameters on how to build dimensions such as the different font
 * styles (for quote, tolerance and note), line style, offsets of the various
 * dimension components etc...
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
  PROP_TOLERANCE_SPACING,
  PROP_QUOTE_SHIFT,
  PROP_TOLERANCE_SHIFT,
  PROP_NOTE_SHIFT,
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

static void	set_quote_style		(AdgDimStyle	*dim_style,
					 AdgFontStyle	*style);
static void	set_tolerance_style	(AdgDimStyle	*dim_style,
					 AdgFontStyle	*style);
static void	set_note_style		(AdgDimStyle	*dim_style,
					 AdgFontStyle	*style);
static void	set_line_style		(AdgDimStyle	*dim_style,
					 AdgLineStyle	*style);
static void	set_arrow_style		(AdgDimStyle	*dim_style,
					 AdgArrowStyle	*style);
static void	set_quote_shift		(AdgDimStyle	*dim_style,
					 const AdgPair	*shift);
static void	set_tolerance_shift	(AdgDimStyle	*dim_style,
					 const AdgPair	*shift);
static void	set_note_shift		(AdgDimStyle	*dim_style,
					 const AdgPair	*shift);
static void	set_number_format	(AdgDimStyle	*dim_style,
					 const gchar	*format);
static void	set_number_tag		(AdgDimStyle	*dim_style,
					 const gchar	*tag);


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
			       P_("Distance between two consecutive baselines while stacking dimensions"),
			       0., G_MAXDOUBLE, 30.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_BASELINE_SPACING, param);

  param = g_param_spec_double ("tolerance-spacing",
			       P_("Tolerance Spacing"),
			       P_("Distance between up and down tolerance text"),
			       0., G_MAXDOUBLE, 2.,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TOLERANCE_SPACING, param);

  param = g_param_spec_boxed ("quote-shift",
			      P_("Quote Shift"),
			      P_("Used to specify a smooth displacement (in paper units) for the quote text by taking as reference the perfect compact position (the middle of the baseline on common linear quotes, for instance)"),
			      ADG_TYPE_PAIR,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_QUOTE_SHIFT, param);

  param = g_param_spec_boxed ("tolerance-shift",
			      P_("Tolerance Shift"),
			      P_("Used to specify a smooth displacement (in paper units) for the tolerance text by taking as reference the perfect compact position"),
			      ADG_TYPE_PAIR,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_TOLERANCE_SHIFT, param);

  param = g_param_spec_boxed ("note-shift",
			      P_("Note Shift"),
			      P_("Used to specify a smooth displacement (in paper units) for the note text by taking as reference the perfect compact position"),
			      ADG_TYPE_PAIR,
			      G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NOTE_SHIFT, param);

  param = g_param_spec_string ("number-format",
			       P_("Number Format"),
			       P_("The format (in printf style) of the numeric component of the quote"),
			       "%-.7g",
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_NUMBER_FORMAT, param);

  param = g_param_spec_string ("number-tag",
			       P_("Number Tag"),
			       P_("The tag to substitute inside the quote pattern"),
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
  priv->tolerance_spacing = 2.;
  priv->quote_shift.x = 0.;
  priv->quote_shift.y = -3.;
  priv->tolerance_shift.x = +5.;
  priv->tolerance_shift.y = -4.;
  priv->note_shift.x = +5.;
  priv->note_shift.y =  0.;
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
    case PROP_TOLERANCE_SPACING:
      g_value_set_double (value, dim_style->priv->tolerance_spacing);
      break;
    case PROP_QUOTE_SHIFT:
      g_value_set_boxed (value, &dim_style->priv->quote_shift);
      break;
    case PROP_TOLERANCE_SHIFT:
      g_value_set_boxed (value, &dim_style->priv->tolerance_shift);
      break;
    case PROP_NOTE_SHIFT:
      g_value_set_boxed (value, &dim_style->priv->note_shift);
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
      set_quote_style (dim_style, g_value_get_object (value));
      break;
    case PROP_TOLERANCE_STYLE:
      set_tolerance_style (dim_style, g_value_get_object (value));
      break;
    case PROP_NOTE_STYLE:
      set_note_style (dim_style, g_value_get_object (value));
      break;
    case PROP_LINE_STYLE:
      set_line_style (dim_style, g_value_get_object (value));
      break;
    case PROP_ARROW_STYLE:
      set_arrow_style (dim_style, g_value_get_object (value));
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
    case PROP_TOLERANCE_SPACING:
      dim_style->priv->tolerance_spacing = g_value_get_double (value);
      break;
    case PROP_QUOTE_SHIFT:
      set_quote_shift (dim_style, g_value_get_boxed (value));
      break;
    case PROP_TOLERANCE_SHIFT:
      set_tolerance_shift (dim_style, g_value_get_boxed (value));
      break;
    case PROP_NOTE_SHIFT:
      set_note_shift (dim_style, g_value_get_boxed (value));
      break;
    case PROP_NUMBER_FORMAT:
      set_number_format (dim_style, g_value_get_string (value));
      break;
    case PROP_NUMBER_TAG:
      set_number_tag (dim_style, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


/**
 * adg_dim_style_new:
 *
 * Constructs a new dimension style initialized with default params.
 *
 * Return value: a new dimension style
 **/
AdgStyle *
adg_dim_style_new (void)
{
  return g_object_new (ADG_TYPE_DIM_STYLE, NULL);
}

/**
 * adg_dim_style_from_id:
 * @id: a dimension style identifier
 *
 * Gets a predefined style from an #AdgDimStyleId identifier.
 *
 * Return value: the requested style or %NULL if not found
 **/
AdgStyle *
adg_dim_style_from_id (AdgDimStyleId id)
{
  static AdgStyle **builtins = NULL;

  if G_UNLIKELY (builtins == NULL)
    {
      cairo_pattern_t *pattern;

      builtins = g_new (AdgStyle *, ADG_DIM_STYLE_LAST);

      /* No need to specify further params: the default is already ISO */
      pattern = cairo_pattern_create_rgb (1., 0., 0.);
      builtins[ADG_DIM_STYLE_ISO] = g_object_new (ADG_TYPE_DIM_STYLE,
						  "pattern", pattern,
						  NULL);
      cairo_pattern_destroy (pattern);
    }

  g_return_val_if_fail (id < ADG_DIM_STYLE_LAST, NULL);
  return builtins[id];
}

/**
 * adg_dim_style_get_quote_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the quote style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Return value: the quote style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_quote_style (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return dim_style->priv->quote_style;
}

/**
 * adg_dim_style_set_quote_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new quote style
 *
 * Sets a new quote style on @dim_style. The old quote style (if any) will be
 * unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_quote_style (AdgDimStyle  *dim_style,
			       AdgFontStyle *style)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));
  g_return_if_fail (ADG_IS_FONT_STYLE (style));

  set_quote_style (dim_style, style);
  g_object_notify ((GObject *) dim_style, "quote-style");
}

/**
 * adg_dim_style_get_tolerance_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the tolerance style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Return value: the tolerance style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_tolerance_style (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return dim_style->priv->tolerance_style;
}

/**
 * adg_dim_style_set_tolerance_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new tolerance style
 *
 * Sets a new tolerance style on @dim_style. The old tolerance style (if any)
 * will be unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_tolerance_style (AdgDimStyle  *dim_style,
				   AdgFontStyle *style)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));
  g_return_if_fail (ADG_IS_FONT_STYLE (style));

  set_tolerance_style (dim_style, style);
  g_object_notify ((GObject *) dim_style, "tolerance-style");
}

/**
 * adg_dim_style_get_note_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the note style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Return value: the note style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_note_style (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return dim_style->priv->note_style;
}

/**
 * adg_dim_style_set_note_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new note style
 *
 * Sets a new note style on @dim_style. The old note style (if any) will be
 * unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_note_style (AdgDimStyle  *dim_style,
			      AdgFontStyle *style)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));
  g_return_if_fail (ADG_IS_FONT_STYLE (style));

  set_note_style (dim_style, style);
  g_object_notify ((GObject *) dim_style, "note-style");
}

/**
 * adg_dim_style_get_line_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the line style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Return value: the line style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_line_style (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return dim_style->priv->line_style;
}

/**
 * adg_dim_style_set_line_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new line style
 *
 * Sets a new line style on @dim_style. The old line style (if any) will be
 * unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_line_style (AdgDimStyle  *dim_style,
			      AdgLineStyle *style)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));
  g_return_if_fail (ADG_IS_LINE_STYLE (style));

  set_line_style (dim_style, style);
  g_object_notify ((GObject *) dim_style, "line-style");
}

/**
 * adg_dim_style_get_arrow_style:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the arrow style of @dim_style. No reference will be added to the
 * returned style; it should not be unreferenced.
 *
 * Return value: the arrow style or %NULL on errors
 **/
AdgStyle *
adg_dim_style_get_arrow_style (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return dim_style->priv->arrow_style;
}

/**
 * adg_dim_style_set_arrow_style:
 * @dim_style: an #AdgDimStyle object
 * @style: the new arrow style
 *
 * Sets a new arrow style on @dim_style. The old arrow style (if any) will be
 * unreferenced while a new reference will be added to @style.
 **/
void
adg_dim_style_set_arrow_style (AdgDimStyle   *dim_style,
			       AdgArrowStyle *style)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));
  g_return_if_fail (ADG_IS_ARROW_STYLE (style));

  set_arrow_style (dim_style, style);
  g_object_notify ((GObject *) dim_style, "arrow-style");
}

/**
 * adg_dim_style_get_from_offset:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance (in paper space) the extension lines must keep from the
 * sensed points.
 *
 * Return value: the requested distance
 **/
gdouble
adg_dim_style_get_from_offset (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), 0.);

  return dim_style->priv->from_offset;
}

/**
 * adg_dim_style_set_from_offset:
 * @dim_style: an #AdgDimStyle object
 * @offset: the new offset
 *
 * Sets a new "from-offset" value.
 **/
void
adg_dim_style_set_from_offset (AdgDimStyle *dim_style,
			       gdouble      offset)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  dim_style->priv->from_offset = offset;
  g_object_notify ((GObject *) dim_style, "from-offset");
}

/**
 * adg_dim_style_get_to_offset:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets how much (in paper space) the extension lines must extend after
 * crossing the baseline.
 *
 * Return value: the requested distance
 **/
gdouble
adg_dim_style_get_to_offset (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), 0.);

  return dim_style->priv->to_offset;
}

/**
 * adg_dim_style_set_to_offset:
 * @dim_style: an #AdgDimStyle object
 * @offset: the new offset
 *
 * Sets a new "to-offset" value.
 **/
void
adg_dim_style_set_to_offset (AdgDimStyle *dim_style,
			     gdouble      offset)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  dim_style->priv->to_offset = offset;
  g_object_notify ((GObject *) dim_style, "to-offset");
}

/**
 * adg_dim_style_get_baseline_spacing:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance between two consecutive baselines
 * while stacking dimensions.
 *
 * Return value: the requested spacing
 **/
gdouble
adg_dim_style_get_baseline_spacing (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), 0.);

  return dim_style->priv->baseline_spacing;
}

/**
 * adg_dim_style_set_baseline_spacing:
 * @dim_style: an #AdgDimStyle object
 * @spacing: the new spacing
 *
 * Sets a new "baseline-spacing" value.
 **/
void
adg_dim_style_set_baseline_spacing (AdgDimStyle *dim_style,
				    gdouble      spacing)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  dim_style->priv->baseline_spacing = spacing;
  g_object_notify ((GObject *) dim_style, "baseline-spacing");
}

/**
 * adg_dim_style_get_tolerance_spacing:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the distance (in paper space) between up and down tolerances.
 *
 * Return value: the requested spacing
 **/
gdouble
adg_dim_style_get_tolerance_spacing (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), 0.);

  return dim_style->priv->tolerance_spacing;
}

/**
 * adg_dim_style_set_tolerance_spacing:
 * @dim_style: an #AdgDimStyle object
 * @spacing: the new spacing
 *
 * Sets a new "tolerance-spacing" value.
 **/
void
adg_dim_style_set_tolerance_spacing (AdgDimStyle *dim_style,
				     gdouble      spacing)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  dim_style->priv->tolerance_spacing = spacing;
  g_object_notify ((GObject *) dim_style, "tolerance-spacing");
}

/**
 * adg_dim_style_get_quote_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement of the quote text. The returned pointer
 * refers to an internal allocated struct and must not be modified or freed.
 *
 * Return value: the requested shift
 **/
const AdgPair *
adg_dim_style_get_quote_shift (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return &dim_style->priv->quote_shift;
}

/**
 * adg_dim_style_set_quote_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new "quote-shift" value.
 **/
void
adg_dim_style_set_quote_shift (AdgDimStyle   *dim_style,
			       const AdgPair *shift)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  set_quote_shift (dim_style, shift);
  g_object_notify ((GObject *) dim_style, "quote-shift");
}

/**
 * adg_dim_style_get_tolerance_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement of the tolerance text. The returned pointer
 * refers to an internal allocated struct and must not be modified or freed.
 *
 * Return value: the requested shift
 **/
const AdgPair *
adg_dim_style_get_tolerance_shift (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return &dim_style->priv->tolerance_shift;
}

/**
 * adg_dim_style_set_tolerance_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new "tolerance-shift" value.
 **/
void
adg_dim_style_set_tolerance_shift (AdgDimStyle   *dim_style,
				   const AdgPair *shift)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  set_tolerance_shift (dim_style, shift);
  g_object_notify ((GObject *) dim_style, "tolerance-shift");
}

/**
 * adg_dim_style_get_note_shift:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the smooth displacement of the note text. The returned pointer
 * refers to an internal allocated struct and must not be modified or freed.
 *
 * Return value: the requested shift
 **/
const AdgPair *
adg_dim_style_get_note_shift (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return &dim_style->priv->note_shift;
}

/**
 * adg_dim_style_set_note_shift:
 * @dim_style: an #AdgDimStyle object
 * @shift: the new displacement
 *
 * Sets a new "note-shift" value.
 **/
void
adg_dim_style_set_note_shift (AdgDimStyle   *dim_style,
			      const AdgPair *shift)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  set_note_shift (dim_style, shift);
  g_object_notify ((GObject *) dim_style, "note-shift");
}

/**
 * adg_dim_style_get_number_format:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the number format (in printf style) of this quoting style. The
 * returned pointer refers to internally managed text that must not be
 * modified or freed.
 *
 * Return value: the requested format
 **/
const gchar *
adg_dim_style_get_number_format (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return dim_style->priv->number_format;
}

/**
 * adg_dim_style_set_number_format:
 * @dim_style: an #AdgDimStyle object
 * @format: the new format to adopt
 *
 * Sets a new "number-format" value.
 **/
void
adg_dim_style_set_number_format (AdgDimStyle *dim_style,
				 const gchar *format)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  set_number_format (dim_style, format);
  g_object_notify ((GObject *) dim_style, "number-format");
}

/**
 * adg_dim_style_get_number_tag:
 * @dim_style: an #AdgDimStyle object
 *
 * Gets the number tag to substitute while building the quote text. The
 * returned pointer refers to internally managed text that must not be
 * modified or freed.
 *
 * Return value: the requested tag
 **/
const gchar *
adg_dim_style_get_number_tag (AdgDimStyle *dim_style)
{
  g_return_val_if_fail (ADG_IS_DIM_STYLE (dim_style), NULL);

  return dim_style->priv->number_tag;
}

/**
 * adg_dim_style_set_number_tag:
 * @dim_style: an #AdgDimStyle object
 * @tag: the new tag
 *
 * Sets a new "number-tag" value.
 **/
void
adg_dim_style_set_number_tag (AdgDimStyle *dim_style,
			      const gchar *tag)
{
  g_return_if_fail (ADG_IS_DIM_STYLE (dim_style));

  set_number_tag (dim_style, tag);
  g_object_notify ((GObject *) dim_style, "number-tag");
}


static void
set_quote_style (AdgDimStyle  *dim_style,
		 AdgFontStyle *style)
{
  if (dim_style->priv->quote_style)
    g_object_unref (dim_style->priv->quote_style);

  g_object_ref (style);
  dim_style->priv->quote_style = (AdgStyle *) style;
}

static void
set_tolerance_style (AdgDimStyle  *dim_style,
		     AdgFontStyle *style)
{
  if (dim_style->priv->tolerance_style)
    g_object_unref (dim_style->priv->tolerance_style);

  g_object_ref (style);
  dim_style->priv->tolerance_style = (AdgStyle *) style;
}

static void
set_note_style (AdgDimStyle  *dim_style,
		AdgFontStyle *style)
{
  if (dim_style->priv->note_style)
    g_object_unref (dim_style->priv->note_style);

  g_object_ref (style);
  dim_style->priv->note_style = (AdgStyle *) style;
}

static void
set_line_style (AdgDimStyle  *dim_style,
		AdgLineStyle *style)
{
  if (dim_style->priv->line_style)
    g_object_unref (dim_style->priv->line_style);

  g_object_ref (style);
  dim_style->priv->line_style = (AdgStyle *) style;
}

static void
set_arrow_style (AdgDimStyle  *dim_style,
		 AdgArrowStyle *style)
{
  if (dim_style->priv->arrow_style)
    g_object_unref (dim_style->priv->arrow_style);

  g_object_ref (style);
  dim_style->priv->arrow_style = (AdgStyle *) style;
}

static void
set_quote_shift (AdgDimStyle   *dim_style,
		 const AdgPair *shift)
{
  cpml_pair_copy (&dim_style->priv->quote_shift, shift);
}

static void
set_tolerance_shift (AdgDimStyle   *dim_style,
		     const AdgPair *shift)
{
  cpml_pair_copy (&dim_style->priv->tolerance_shift, shift);
}

static void
set_note_shift (AdgDimStyle   *dim_style,
		const AdgPair *shift)
{
  cpml_pair_copy (&dim_style->priv->note_shift, shift);
}

static void
set_number_format (AdgDimStyle *dim_style,
		   const gchar *format)
{
  g_free (dim_style->priv->number_format);
  dim_style->priv->number_format = g_strdup (format);
}

static void
set_number_tag (AdgDimStyle *dim_style,
		const gchar *tag)
{
  g_free (dim_style->priv->number_tag);
  dim_style->priv->number_tag = g_strdup (tag);
}
