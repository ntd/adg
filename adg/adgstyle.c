/**
 * SECTION:adgstyle
 * @include:adg/adg.h
 * @title: Styles
 * @short_description: A collection of styles to use inside the drawings
 *
 **/

#include "adgstyle.h"
#include "adgutil.h"

#include <string.h>
#include <math.h>


static gpointer         adg_line_style_copy     (gpointer src);
static void             adg_line_style_init     (void);

static AdgLineStyle *   line_styles = NULL;
static gint             n_line_styles = ADG_LINE_STYLE_LAST;

GType
adg_line_style_get_type (void)
{
  static int line_style_type = 0;
  
  if G_UNLIKELY (line_style_type == 0)
    line_style_type = g_boxed_type_register_static ("AdgLineStyle",
                                                    adg_line_style_copy,
                                                    g_free);

  return line_style_type;
}

static gpointer
adg_line_style_copy (gpointer src)
{
  g_return_val_if_fail (src != NULL, NULL);
  return g_memdup (src, sizeof (AdgLineStyle));
}

AdgLineStyle *
adg_line_style_from_id (AdgLineStyleId id)
{
  g_return_val_if_fail (id < n_line_styles, NULL);

  if G_UNLIKELY (line_styles == NULL)
    adg_line_style_init ();

  return line_styles + id;
}

static void
adg_line_style_init (void)
{
  AdgLineStyle *style;

  line_styles = g_new (AdgLineStyle, ADG_LINE_STYLE_LAST);

  style = line_styles + ADG_LINE_STYLE_DRAW;
  style->width = 2.0;
  style->cap = CAIRO_LINE_CAP_ROUND;
  style->join = CAIRO_LINE_JOIN_ROUND;
  style->dashes = NULL;
  style->num_dashes = 0;
  style->dash_offset = 0.0;

  style = line_styles + ADG_LINE_STYLE_CENTER;
  style->width = 0.75;
  style->cap = CAIRO_LINE_CAP_ROUND;
  style->join = CAIRO_LINE_JOIN_ROUND;
  style->dashes = NULL;
  style->num_dashes = 0;
  style->dash_offset = 0.0;

  style = line_styles + ADG_LINE_STYLE_HIDDEN;
  style->width = 0.75;
  style->cap = CAIRO_LINE_CAP_ROUND;
  style->join = CAIRO_LINE_JOIN_ROUND;
  style->dashes = NULL;
  style->num_dashes = 0;
  style->dash_offset = 0.0;

  style = line_styles + ADG_LINE_STYLE_XATCH;
  style->width = 1.5;
  style->cap = CAIRO_LINE_CAP_ROUND;
  style->join = CAIRO_LINE_JOIN_ROUND;
  style->dashes = NULL;
  style->num_dashes = 0;
  style->dash_offset = 0.0;

  style = line_styles + ADG_LINE_STYLE_DIM;
  style->width = 0.75;
  style->cap = CAIRO_LINE_CAP_ROUND;
  style->join = CAIRO_LINE_JOIN_ROUND;
  style->dashes = NULL;
  style->num_dashes = 0;
  style->dash_offset = 0.0;
}

AdgLineStyleId
adg_line_style_register (AdgLineStyle *new_style)
{
  ADG_STUB ();
  return 0;
}

void
adg_line_style_apply (const AdgLineStyle *style,
                      cairo_t            *cr)
{
  double device_width;

  g_return_if_fail (style != NULL);
  g_return_if_fail (cr != NULL);

  device_width = style->width;

  cairo_device_to_user_distance (cr, &device_width, &device_width);
  cairo_set_line_width (cr, device_width);
  cairo_set_line_cap (cr, style->cap);
  cairo_set_line_join (cr, style->join);

  if (style->num_dashes > 0)
    {
      g_return_if_fail (style->dashes != NULL);

      cairo_set_dash (cr, style->dashes, style->num_dashes, style->dash_offset);
    }
}


static gpointer         adg_font_style_copy     (gpointer               src);
static void             adg_font_style_options  (cairo_font_options_t **p_options);
static void             adg_font_style_init     (void);

static AdgFontStyle *   font_styles = NULL;
static gint             n_font_styles = ADG_FONT_STYLE_LAST;

GType
adg_font_style_get_type (void)
{
  static int font_style_type = 0;
  
  if G_UNLIKELY (font_style_type == 0)
    font_style_type = g_boxed_type_register_static ("AdgFontStyle",
                                                    adg_font_style_copy,
                                                    g_free);

  return font_style_type;
}

static gpointer
adg_font_style_copy (gpointer src)
{
  g_return_val_if_fail (src != NULL, NULL);
  return g_memdup (src, sizeof (AdgFontStyle));
}

AdgFontStyle *
adg_font_style_from_id (AdgFontStyleId id)
{
  g_return_val_if_fail (id < n_font_styles, NULL);

  if G_UNLIKELY (font_styles == NULL)
    adg_font_style_init ();

  return font_styles + id;
}

static void
adg_font_style_init (void)
{
  AdgFontStyle *style;

  font_styles = g_new (AdgFontStyle, ADG_FONT_STYLE_LAST);

  style = font_styles + ADG_FONT_STYLE_TEXT;
  style->family = "Serif";
  style->slant = CAIRO_FONT_SLANT_NORMAL;
  style->weight = CAIRO_FONT_WEIGHT_NORMAL;
  style->size = 16.0;
  style->antialias = CAIRO_ANTIALIAS_DEFAULT;
  style->subpixel_order = CAIRO_SUBPIXEL_ORDER_DEFAULT;
  style->hint_style = CAIRO_HINT_STYLE_DEFAULT;
  style->hint_metrics = CAIRO_HINT_METRICS_DEFAULT;

  style = font_styles + ADG_FONT_STYLE_DIMLABEL;
  style->family = "Sans";
  style->slant = CAIRO_FONT_SLANT_NORMAL;
  style->weight = CAIRO_FONT_WEIGHT_BOLD;
  style->size = 14.0;
  style->antialias = CAIRO_ANTIALIAS_DEFAULT;
  style->subpixel_order = CAIRO_SUBPIXEL_ORDER_DEFAULT;
  style->hint_style = CAIRO_HINT_STYLE_DEFAULT;
  style->hint_metrics = CAIRO_HINT_METRICS_DEFAULT;

  style = font_styles + ADG_FONT_STYLE_DIMTOLERANCE;
  style->family = "Sans";
  style->slant = CAIRO_FONT_SLANT_NORMAL;
  style->weight = CAIRO_FONT_WEIGHT_NORMAL;
  style->size = 8.0;
  style->antialias = CAIRO_ANTIALIAS_DEFAULT;
  style->subpixel_order = CAIRO_SUBPIXEL_ORDER_DEFAULT;
  style->hint_style = CAIRO_HINT_STYLE_DEFAULT;
  style->hint_metrics = CAIRO_HINT_METRICS_DEFAULT;

  style = font_styles + ADG_FONT_STYLE_DIMNOTE;
  style->family = "Sans";
  style->slant = CAIRO_FONT_SLANT_NORMAL;
  style->weight = CAIRO_FONT_WEIGHT_NORMAL;
  style->size = 12.0;
  style->antialias = CAIRO_ANTIALIAS_DEFAULT;
  style->subpixel_order = CAIRO_SUBPIXEL_ORDER_DEFAULT;
  style->hint_style = CAIRO_HINT_STYLE_DEFAULT;
  style->hint_metrics = CAIRO_HINT_METRICS_DEFAULT;
}

AdgFontStyleId
adg_font_style_register (AdgFontStyle *new_style)
{
  ADG_STUB ();
  return 0;
}

void
adg_font_style_apply (const AdgFontStyle *style,
                      cairo_t            *cr)
{
  cairo_font_options_t *options;
  double                font_size;

  g_return_if_fail (style != NULL);
  g_return_if_fail (cr != NULL);

  font_size = style->size;

  cairo_select_font_face (cr, style->family, style->slant, style->weight);
  cairo_device_to_user_distance (cr, &font_size, &font_size);
  cairo_set_font_size (cr, font_size);

  options = NULL;

  if (style->antialias != CAIRO_ANTIALIAS_DEFAULT)
    {
      adg_font_style_options (&options);
      cairo_font_options_set_antialias (options, style->antialias);
    }

  if (style->subpixel_order != CAIRO_SUBPIXEL_ORDER_DEFAULT)
    {
      adg_font_style_options (&options);
      cairo_font_options_set_subpixel_order (options, style->subpixel_order);
    }

  if (style->hint_style != CAIRO_HINT_STYLE_DEFAULT)
    {
      adg_font_style_options (&options);
      cairo_font_options_set_hint_style (options, style->hint_style);
    }

  if (style->hint_metrics != CAIRO_HINT_METRICS_DEFAULT)
    {
      adg_font_style_options (&options);
      cairo_font_options_set_hint_metrics (options, style->hint_metrics);
    }

  /* Check if any option was set */
  if (options != NULL)
    {
      cairo_set_font_options (cr, options);
      cairo_font_options_destroy (options);
    }
}

static void
adg_font_style_options (cairo_font_options_t **p_options)
{
  g_assert (p_options);

  /* Check if the options container was never created */
  if (*p_options == NULL)
    *p_options = cairo_font_options_create ();
}



static gpointer         adg_arrow_style_copy    (gpointer           src);
static void             adg_arrow_style_init    (void);
static void             adg_arrow_create_arrow  (cairo_t           *cr,
                                                 AdgArrowStyle     *arrow_style,
                                                 AdgPair           *poi,
                                                 cairo_path_data_t *path_data);
static void             adg_arrow_render_arrow  (cairo_t           *cr,
                                                 AdgArrowStyle     *style,
                                                 AdgPair           *poi,
                                                 cairo_path_data_t *path_data);
static void             adg_arrow_render_triangle
                                                (cairo_t           *cr,
                                                 AdgArrowStyle     *style,
                                                 AdgPair           *poi,
                                                 cairo_path_data_t *path_data);
static void             adg_arrow_render_dot    (cairo_t           *cr,
                                                 AdgArrowStyle     *style,
                                                 AdgPair           *poi,
                                                 cairo_path_data_t *path_data);
static void             adg_arrow_render_circle (cairo_t           *cr,
                                                 AdgArrowStyle     *style,
                                                 AdgPair           *poi,
                                                 cairo_path_data_t *path_data);
static void             adg_arrow_render_square (cairo_t           *cr,
                                                 AdgArrowStyle     *style,
                                                 AdgPair           *poi,
                                                 cairo_path_data_t *path_data);
static void             adg_arrow_render_tick   (cairo_t           *cr,
                                                 AdgArrowStyle     *style,
                                                 AdgPair           *poi,
                                                 cairo_path_data_t *path_data);


static AdgArrowStyle *  arrow_styles = NULL;
static gint             n_arrow_styles = ADG_ARROW_STYLE_LAST;


GType
adg_arrow_style_get_type (void)
{
  static int arrow_style_type = 0;
  
  if G_UNLIKELY (arrow_style_type == 0)
    arrow_style_type = g_boxed_type_register_static ("AdgArrowStyle",
                                                     adg_arrow_style_copy,
                                                     g_free);

  return arrow_style_type;
}

static gpointer
adg_arrow_style_copy (gpointer src)
{
  g_return_val_if_fail (src != NULL, NULL);
  return g_memdup (src, sizeof (AdgArrowStyle));
}

AdgArrowStyle *
adg_arrow_style_from_id (AdgArrowStyleId id)
{
  g_return_val_if_fail (id < n_arrow_styles, NULL);

  if G_UNLIKELY (arrow_styles == NULL)
    adg_arrow_style_init ();

  return arrow_styles + id;
}

static void
adg_arrow_style_init (void)
{
  AdgArrowStyle *style;

  arrow_styles = g_new (AdgArrowStyle, ADG_ARROW_STYLE_LAST);

  style = arrow_styles + ADG_ARROW_STYLE_ARROW;
  style->size = 14.0;
  style->angle = G_PI / 6.0;    /* 30 degree */
  style->margin = style->size;
  style->need_path = TRUE;
  style->render_func = adg_arrow_render_arrow;

  style = arrow_styles + ADG_ARROW_STYLE_TRIANGLE;
  style->size = 14.0;
  style->angle = G_PI / 6.0;    /* 30 degree */
  style->margin = style->size;
  style->need_path = TRUE;
  style->render_func = adg_arrow_render_triangle;

  style = arrow_styles + ADG_ARROW_STYLE_DOT;
  style->size = 5.0;
  style->angle = 0.0;
  style->margin = style->size / 2.0;
  style->need_path = FALSE;
  style->render_func = adg_arrow_render_dot;

  style = arrow_styles + ADG_ARROW_STYLE_CIRCLE;
  style->size = 10.0;
  style->angle = 0.0;
  style->margin = style->size / 2.0;
  style->need_path = FALSE;
  style->render_func = adg_arrow_render_circle;

  style = arrow_styles + ADG_ARROW_STYLE_SQUARE;
  style->size = 10.0;
  style->angle = 0.0;
  style->margin = style->size;
  style->need_path = TRUE;
  style->render_func = adg_arrow_render_square;

  style = arrow_styles + ADG_ARROW_STYLE_TICK;
  style->size = 20.0;
  style->angle = G_PI / 6.0;    /* 30 degree */
  style->margin = 0.0;
  style->need_path = TRUE;
  style->render_func = adg_arrow_render_tick;
}

AdgArrowStyleId
adg_arrow_style_register (AdgArrowStyle *new_style)
{
  ADG_STUB ();
  return 0;
}

void
adg_arrow_render  (cairo_t       *cr,
                   AdgArrowStyle *arrow_style,
                   cairo_path_t  *path,
                   AdgPathPoint   path_point)
{
  cairo_path_data_t  path_data[4];
  cairo_path_data_t *p_data;
  cairo_path_data_t *last;
  AdgPair            pair;

  g_return_if_fail (cr != NULL);
  g_return_if_fail (arrow_style != NULL);
  g_return_if_fail (arrow_style->render_func != NULL);
  g_return_if_fail (path != NULL);
  g_return_if_fail (path->num_data > 0);

  switch (path_point)
    {
    case ADG_PATH_POINT_START:
      p_data = path->data;

      switch (p_data->header.type)
        {
          case CAIRO_PATH_MOVE_TO:
            ++ p_data;
            pair.x = p_data->point.x;
            pair.y = p_data->point.y;
            ++ p_data;
            break;
          case CAIRO_PATH_LINE_TO:
          case CAIRO_PATH_CURVE_TO:
            pair.x = 0.0;
            pair.y = 0.0;
            break;
          default:
            g_return_if_reached ();
        }

      g_assert (p_data->header.length <= 4);
      memcpy (path_data, p_data, sizeof (cairo_path_data_t) * p_data->header.length);
      break;
    case ADG_PATH_POINT_END:
      p_data = adg_cairo_path_tail (path);
      last = p_data == NULL ? path->data : p_data + p_data->header.length;

      switch (last->header.type)
        {
        case CAIRO_PATH_LINE_TO:
          pair.x = last[1].point.x;
          pair.y = last[1].point.y;
          path_data[0].header.type = CAIRO_PATH_LINE_TO;
          path_data[0].header.length = 2;
          adg_cairo_path_data_end_point (p_data, &path_data[1].point.x, &path_data[1].point.y);
          break;
        case CAIRO_PATH_CURVE_TO:
          pair.x = last[3].point.x;
          pair.y = last[3].point.y;
          path_data[0].header.type = CAIRO_PATH_CURVE_TO;
          path_data[0].header.length = 4;
          path_data[1].point.x = last[2].point.x;
          path_data[1].point.y = last[2].point.y;
          path_data[2].point.x = last[1].point.x;
          path_data[2].point.y = last[1].point.y;
          adg_cairo_path_data_end_point (p_data, &path_data[3].point.x, &path_data[3].point.y);
          break;
        default:
          g_return_if_reached ();
        }
      break;
    default:
      g_return_if_reached ();
    }

  arrow_style->render_func (cr, arrow_style, &pair, path_data);
}

static void
adg_arrow_create_arrow (cairo_t           *cr,
                        AdgArrowStyle     *arrow_style,
                        AdgPair           *poi,
                        cairo_path_data_t *path_data)
{
  double    width;
  double    height_2;
  AdgPair   tail;
  AdgPair   tail1, tail2;
  AdgVector vector;

  width = arrow_style->size;
  height_2 = tan (arrow_style->angle / 2.0) * width;

  switch (path_data->header.type)
    {
    case CAIRO_PATH_LINE_TO:
      adg_pair_set_explicit (&vector, path_data[1].point.x - poi->x, path_data[1].point.y - poi->y);
      adg_vector_set_with_pair (&vector, &vector);

      adg_pair_scale (adg_pair_set (&tail, &vector), width);
      cairo_device_to_user_distance (cr, &tail.x, &tail.y);
      adg_pair_add (&tail, poi);

      adg_pair_scale (adg_vector_normal (&vector), height_2);
      cairo_device_to_user_distance (cr, &vector.x, &vector.y);
      adg_pair_add (adg_pair_set (&tail1, &tail), &vector);
      adg_pair_sub (adg_pair_set (&tail2, &tail), &vector);

      cairo_move_to (cr, poi->x, poi->y);
      cairo_line_to (cr, tail1.x, tail1.y);
      cairo_line_to (cr, tail2.x, tail2.y);
      cairo_close_path (cr);
      break;
    case CAIRO_PATH_CURVE_TO:
      ADG_STUB ();
      break;
    default:
      g_assert_not_reached ();
    }
}

static void
adg_arrow_render_arrow (cairo_t           *cr,
                        AdgArrowStyle     *arrow_style,
                        AdgPair           *poi,
                        cairo_path_data_t *path_data)
{
  g_assert (cr != NULL);
  g_assert (arrow_style != NULL);
  g_assert (poi != NULL);
  g_assert (path_data != NULL);

  adg_arrow_create_arrow (cr, arrow_style, poi, path_data);
  cairo_fill (cr);
}

static void
adg_arrow_render_triangle (cairo_t           *cr,
                           AdgArrowStyle     *arrow_style,
                           AdgPair           *poi,
                           cairo_path_data_t *path_data)
{
  g_assert (cr != NULL);
  g_assert (arrow_style != NULL);
  g_assert (poi != NULL);
  g_assert (path_data != NULL);

  adg_arrow_create_arrow (cr, arrow_style, poi, path_data);
  cairo_stroke (cr);
}

static void
adg_arrow_render_dot (cairo_t           *cr,
                      AdgArrowStyle     *arrow_style,
                      AdgPair           *poi,
                      cairo_path_data_t *path_data)
{
  ADG_STUB ();
}

static void
adg_arrow_render_circle (cairo_t           *cr,
                         AdgArrowStyle     *arrow_style,
                         AdgPair           *poi,
                         cairo_path_data_t *path_data)
{
  ADG_STUB ();
}

static void
adg_arrow_render_square (cairo_t           *cr,
                         AdgArrowStyle     *arrow_style,
                         AdgPair           *poi,
                         cairo_path_data_t *path_data)
{
  ADG_STUB ();
}

static void
adg_arrow_render_tick   (cairo_t           *cr,
                         AdgArrowStyle     *arrow_style,
                         AdgPair           *poi,
                         cairo_path_data_t *path_data)
{
  ADG_STUB ();
}



static gpointer         adg_dim_style_copy      (gpointer src);
static void             adg_dim_style_init      (void);

static AdgDimStyle *    dim_styles = NULL;
static gint             n_dim_styles = ADG_DIM_STYLE_LAST;


GType
adg_dim_style_get_type (void)
{
  static int dim_style_type = 0;
  
  if G_UNLIKELY (dim_style_type == 0)
    dim_style_type = g_boxed_type_register_static ("AdgDimStyle",
                                                   adg_dim_style_copy,
                                                   g_free);

  return dim_style_type;
}

static gpointer
adg_dim_style_copy (gpointer src)
{
  g_return_val_if_fail (src != NULL, NULL);
  return g_memdup (src, sizeof (AdgDimStyle));
}

AdgDimStyle *
adg_dim_style_from_id (AdgDimStyleId id)
{
  g_return_val_if_fail (id < n_dim_styles, NULL);

  if G_UNLIKELY (dim_styles == NULL)
    adg_dim_style_init ();

  return dim_styles + id;
}

static void
adg_dim_style_init (void)
{
  AdgDimStyle *style;

  dim_styles = g_new (AdgDimStyle, ADG_DIM_STYLE_LAST);

  style = dim_styles + ADG_DIM_STYLE_ISO;
  style->pattern = cairo_pattern_create_rgb (1.0, 0.0, 0.0);
  style->label_style = adg_font_style_from_id (ADG_FONT_STYLE_DIMLABEL);
  style->tolerance_style = adg_font_style_from_id (ADG_FONT_STYLE_DIMTOLERANCE);
  style->note_style = adg_font_style_from_id (ADG_FONT_STYLE_DIMNOTE);
  style->line_style = adg_line_style_from_id (ADG_LINE_STYLE_DIM);
  style->arrow_style = adg_arrow_style_from_id (ADG_ARROW_STYLE_ARROW);
  style->from_offset = 5.0;
  style->to_offset = 5.0;
  style->baseline_spacing = 30.0;
  style->quote_offset.x = 0.0;
  style->quote_offset.y = -3.0;
  style->tolerance_offset.x = +5.0;
  style->tolerance_offset.y = -4.0;
  style->tolerance_spacing = 2.0;
  style->note_offset.x = +5.0;
  style->note_offset.y =  0.0;
  style->measure_format = g_strdup ("%-.7g");
}

AdgDimStyleId
adg_dim_style_register (AdgDimStyle *new_style)
{
  ADG_STUB ();
  return 0;
}
