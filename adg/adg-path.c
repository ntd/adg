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
 * SECTION:adgpath
 * @title: AdgPath
 * @short_description: A stroked path entity
 *
 * The #AdgPath object is peraphs the simplest entity.
 *
 * It contains a pointer to the desired #cairo_path_t structure.
 */

#include "adg-path.h"
#include "adg-path-private.h"
#include "adg-util.h"
#include "adg-canvas.h"
#include "adg-intl.h"
#include <math.h>

#define PARENT_CLASS ((AdgEntityClass *) adg_path_parent_class)
#define ARC_TOLERANCE   0.1


enum
{
  PROP_0,
  PROP_LINE_STYLE
};

typedef enum _Direction Direction;
enum _Direction
{
  DIRECTION_FORWARD,
  DIRECTION_REVERSE
};


static void	get_property		(GObject	*object,
					 guint		 prop_id,
					 GValue		*value,
					 GParamSpec	*pspec);
static void	set_property		(GObject	*object,
					 guint		 prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);
static void	finalize		(GObject	*object);
static const AdgLineStyle *
		get_line_style		(AdgEntity	*entity);
static void	set_line_style		(AdgEntity	*entity,
					 AdgLineStyle	*line_style);
static void	update			(AdgEntity	*entity,
					 gboolean	 recursive);
static void	outdate			(AdgEntity	*entity,
					 gboolean	 recursive);
static void	render			(AdgEntity	*entity,
					 cairo_t	*cr);
static void	add_portion		(AdgPath	*path,
					 cairo_path_data_type_t type,
							 ...);
/* Adapted from cairo-1.3.8 */
static double	arc_error_normalized	(double		 angle);
static double	arc_max_angle_for_tolerance_normalized
					(double		 tolerance);
static int	arc_segments_needed	(double		 angle,
					 double		 radius,
					 double		 tolerance);
static void	arc_segment		(AdgPath	*path,
					 double		 xc,
					 double		 yc,
					 double		 radius,
					 double		 angle_A,
					 double		 angle_B);
static void	arc_in_direction	(AdgPath 	*path,
					 double		 xc,
					 double		 yc,
					 double		 radius,
					 double		 angle_min,
					 double		 angle_max,
					 Direction	 dir);


G_DEFINE_TYPE (AdgPath, adg_path, ADG_TYPE_ENTITY);


static void
adg_path_class_init (AdgPathClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;

  g_type_class_add_private (klass, sizeof (AdgPathPrivate));

  gobject_class->get_property = get_property;
  gobject_class->set_property = set_property;
  gobject_class->finalize = finalize;

  entity_class->get_line_style = get_line_style;
  entity_class->set_line_style = set_line_style;
  entity_class->update = update;
  entity_class->outdate = outdate;
  entity_class->render = render;

  param = g_param_spec_boxed ("line-style",
                              P_("Line Style"),
                              P_("Line style to use while rendering the path"),
                              ADG_TYPE_LINE_STYLE,
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_LINE_STYLE, param);
}

static void
adg_path_init (AdgPath *path)
{
  AdgPathPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (path, ADG_TYPE_PATH,
						      AdgPathPrivate);

  priv->line_style = NULL;
  priv->cairo_path.status = CAIRO_STATUS_SUCCESS;
  priv->cairo_path.data = NULL;
  priv->cairo_path.num_data = 0;
  priv->cp.x = ADG_NAN;
  priv->cp.y = ADG_NAN;
  priv->create_func = NULL;
  priv->user_data = NULL;

  path->priv = priv;
}

static void
get_property (GObject    *object,
	      guint       prop_id,
	      GValue     *value,
	      GParamSpec *pspec)
{
  AdgPathPrivate *priv = ((AdgPath *) object)->priv;

  switch (prop_id)
    {
    case PROP_LINE_STYLE:
      g_value_set_boxed (value, priv->line_style);
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
  switch (prop_id)
    {
    case PROP_LINE_STYLE:
      set_line_style ((AdgEntity *) object, g_value_get_boxed (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
finalize (GObject *object)
{
  adg_path_clear ((AdgPath *) object);

  ((GObjectClass *) PARENT_CLASS)->finalize (object);
}


static const AdgLineStyle *
get_line_style (AdgEntity *entity)
{
  return ((AdgPath *) entity)->priv->line_style;
}

static void
set_line_style (AdgEntity    *entity,
		AdgLineStyle *line_style)
{
  ((AdgPath *) entity)->priv->line_style = line_style;
  g_object_notify (G_OBJECT (entity), "line-style");
}

static void
update (AdgEntity *entity,
	gboolean   recursive)
{
  AdgPathPrivate *priv = ((AdgPath *) entity)->priv;

  if (priv->create_func != NULL)
    priv->create_func (entity, priv->user_data);

  PARENT_CLASS->update (entity, recursive);
}

static void
outdate (AdgEntity *entity,
	 gboolean   recursive)
{
  adg_path_clear ((AdgPath *) entity);
  PARENT_CLASS->outdate (entity, recursive);
}

static void
render (AdgEntity *entity,
                 cairo_t   *cr)
{
  AdgPathPrivate *priv = ((AdgPath *) entity)->priv;

  adg_line_style_apply (adg_entity_get_line_style (entity), cr);
  cairo_append_path (cr, &priv->cairo_path);
  cairo_stroke (cr);
}

static void
add_portion (AdgPath               *path,
	     cairo_path_data_type_t type,
	     ...)
{
  AdgPathPrivate   *priv = path->priv;
  cairo_path_data_t portion;
  va_list           var_args;

  if (!priv->portions)
    priv->portions = g_array_sized_new (FALSE, FALSE,
					sizeof (cairo_path_data_t),
					10 /* Preallocated elements */);

  portion.header.type = type;
  va_start (var_args, type);

  switch (type)
    {
    case CAIRO_PATH_CLOSE_PATH:
      portion.header.length = 1;
      priv->portions = g_array_append_val (priv->portions, portion);
      portion.point.x = ADG_NAN;
      portion.point.y = ADG_NAN;
      break;

    case CAIRO_PATH_MOVE_TO:
      portion.header.length = 2;
      priv->portions = g_array_append_val (priv->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      priv->portions = g_array_append_val (priv->portions, portion);
      break;

    case CAIRO_PATH_LINE_TO:
      portion.header.length = 2;
      priv->portions = g_array_append_val (priv->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      priv->portions = g_array_append_val (priv->portions, portion);
      break;
      
    case CAIRO_PATH_CURVE_TO:
      portion.header.length = 4;
      priv->portions = g_array_append_val (priv->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      priv->portions = g_array_append_val (priv->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      priv->portions = g_array_append_val (priv->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      priv->portions = g_array_append_val (priv->portions, portion);
      break;

    default:
      g_assert_not_reached ();
    }

  priv->cairo_path.data = (cairo_path_data_t *) priv->portions->data;
  priv->cairo_path.num_data = priv->portions->len;
  priv->cairo_path.status = CAIRO_STATUS_SUCCESS;

  priv->cp.x = portion.point.x;
  priv->cp.y = portion.point.y;
}


/**
 * adg_path_new:
 * @create_func: an #AdgCallback callback
 * @user_data: user pointer
 *
 * Creates a new path entity using @create_func as creation callback.
 *
 * Return value: the new entity
 */
AdgEntity *
adg_path_new (AdgCallback create_func,
              gpointer    user_data)
{
  AdgEntity      *entity;
  AdgPathPrivate *priv;
 
  entity = (AdgEntity *) g_object_new (ADG_TYPE_PATH, NULL);
  priv = ((AdgPath *) entity)->priv;

  priv->create_func = create_func;
  priv->user_data = user_data;

  return entity;
}

void
adg_path_clear (AdgPath *path)
{
  AdgPathPrivate *priv;

  g_return_if_fail (ADG_IS_PATH (path));

  priv = path->priv;

  g_array_free (priv->portions, TRUE);
  priv->portions = NULL;
  priv->cairo_path.status = CAIRO_STATUS_SUCCESS;
  priv->cairo_path.data = NULL;
  priv->cairo_path.num_data = 0;
  priv->cp.x = ADG_NAN;
  priv->cp.y = ADG_NAN;
}


const cairo_path_t *
adg_path_get_cairo_path (AdgPath *path)
{
  g_return_val_if_fail (ADG_IS_PATH (path), NULL);

  return &path->priv->cairo_path;
}


void
adg_path_chain_ymirror (AdgPath *path)
{
  AdgPathPrivate    *priv;
  cairo_path_t      *cairo_path;
  cairo_path_data_t *src;
  cairo_path_data_t *p_src, *p_dst;
  gint               length;
  gint               n_data, n_point;
  double             last_x, last_y;

  g_return_if_fail (ADG_IS_PATH (path));

  priv = path->priv;
  cairo_path = &priv->cairo_path;

  g_return_if_fail (cairo_path->num_data > 2);
  g_return_if_fail (cairo_path->data->header.type == CAIRO_PATH_MOVE_TO);

  src = g_memdup (cairo_path->data, cairo_path->num_data * sizeof (cairo_path_data_t));
  priv->portions = g_array_set_size (priv->portions, cairo_path->num_data * 2);
  p_src = src;
  p_dst = (cairo_path_data_t *) priv->portions->data + cairo_path->num_data * 2;
  n_data = 2;

  ++ p_src;
  last_x = p_src->point.x;
  last_y = p_src->point.y;
  priv->cp.x = last_x;
  priv->cp.y = last_y;
  ++ p_src;

  while (n_data < cairo_path->num_data)
    {
      length = p_src->header.length;
      p_dst -= length;
      p_dst->header.type = p_src->header.type;
      p_dst->header.length = length;
      ++ p_src;

      for (n_point = 1; n_point < length - 1; ++ n_point)
        {
          p_dst[length-n_point-1].point.x = p_src->point.x;
          p_dst[length-n_point-1].point.y = -p_src->point.y;
          ++ p_src;
        }

      p_dst[length-1].point.x = last_x;
      p_dst[length-1].point.y = -last_y;
      last_x = p_src->point.x;
      last_y = p_src->point.y;
      ++ p_src;
      n_data += length;
    }

  p_dst -= 2;
  p_dst->header.type = CAIRO_PATH_LINE_TO;
  p_dst->header.length = 2;
  ++ p_dst;
  p_dst->point.x = last_x;
  p_dst->point.y = -last_y;

  g_free (src);

  cairo_path->num_data = cairo_path->num_data * 2;
  cairo_path->data = (cairo_path_data_t *) priv->portions->data;
}

void
adg_path_dump (AdgPath *path)
{
  cairo_path_data_t *data;
  gint               n_data, n_point;

  g_return_if_fail (ADG_IS_PATH (path));

  for (n_data = 0; n_data < path->priv->cairo_path.num_data; ++ n_data)
    {
      data = path->priv->cairo_path.data + n_data;

      switch (data->header.type)
        {
        case CAIRO_PATH_MOVE_TO:
          g_print ("Move to ");
          break;
        case CAIRO_PATH_LINE_TO:
          g_print ("Line to ");
          break;
        case CAIRO_PATH_CURVE_TO:
          g_print ("Curve to ");
          break;
        case CAIRO_PATH_CLOSE_PATH:
          g_print ("Path close");
          break;
        default:
          g_print ("Unknown entity (%d)", data->header.type);
          break;
        }

      for (n_point = 1; n_point < data->header.length; ++ n_point)
        g_print ("(%lf, %lf) ", data[n_point].point.x, data[n_point].point.y);

      n_data += n_point-1;
      g_print ("\n");
    }
}


/* Cairo wrappers */

gboolean
adg_path_get_current_point (AdgPath *path,
                            double  *x,
                            double  *y)
{
  AdgPathPrivate *priv;

  g_return_val_if_fail (ADG_IS_PATH (path), FALSE);

  priv = path->priv;

  if (adg_isnan (priv->cp.x) || adg_isnan (priv->cp.y))
    return FALSE;

  if (x != NULL)
    *x = priv->cp.x;

  if (y != NULL)
    *y = priv->cp.y;

  return TRUE;
}

void
adg_path_close (AdgPath *path)
{
  g_return_if_fail (ADG_IS_PATH (path));

  add_portion (path, CAIRO_PATH_CLOSE_PATH);
}

void
adg_path_arc (AdgPath *path,
              double   x,
              double   y,
              double   radius,
              double   angle1,
              double   angle2)
{
  g_return_if_fail (ADG_IS_PATH (path));
  g_return_if_fail (radius > 0.0);

  while (angle2 < angle1)
    angle2 += 2 * G_PI;

  adg_path_line_to (path, x + radius * cos (angle1), y + radius * sin (angle1));
  arc_in_direction (path, x, y, radius, angle1, angle2, DIRECTION_FORWARD);
}

void
adg_path_arc_negative (AdgPath *path,
                       double   x,
                       double   y,
                       double   radius,
                       double   angle1,
                       double   angle2)
{
  g_return_if_fail (ADG_IS_PATH (path));
  g_return_if_fail (radius > 0.0);

  while (angle2 > angle1)
    angle2 -= 2 * G_PI;

  adg_path_line_to (path, x + radius * cos (angle1), y + radius * sin (angle1));
  arc_in_direction (path, x, y, radius, angle2, angle1, DIRECTION_REVERSE);
}

void
adg_path_curve_to (AdgPath *path,
                   double   x1,
                   double   y1,
                   double   x2,
                   double   y2,
                   double   x3,
                   double   y3)
{
  g_return_if_fail (ADG_IS_PATH (path));

  add_portion (path, CAIRO_PATH_CURVE_TO, x1, y1, x2, y2, x3, y3);
}

void
adg_path_line_to (AdgPath *path,
                  double   x,
                  double   y)
{
  g_return_if_fail (ADG_IS_PATH (path));

  add_portion (path, CAIRO_PATH_LINE_TO, x, y);
}

void
adg_path_move_to (AdgPath *path,
                  double   x,
                  double   y)
{
  g_return_if_fail (ADG_IS_PATH (path));

  add_portion (path, CAIRO_PATH_MOVE_TO, x, y);
}

void
adg_path_rectangle (AdgPath *path,
                    double   x,
                    double   y,
                    double   width,
                    double   height)
{
  g_return_if_fail (ADG_IS_PATH (path));

  adg_path_move_to (path, x, y);
  adg_path_rel_line_to (path, width, 0);
  adg_path_rel_line_to (path, 0, height);
  adg_path_rel_line_to (path, -width, 0);
  adg_path_close (path);
}

void
adg_path_rel_curve_to (AdgPath *path,
                       double   dx1,
                       double   dy1,
                       double   dx2,
                       double   dy2,
                       double   dx3,
                       double   dy3)
{
  double x, y;

  g_return_if_fail (ADG_IS_PATH (path));
  g_return_if_fail (adg_path_get_current_point (path, &x, &y));

  adg_path_curve_to (path, x+dx1, y+dy1, x+dx2, y+dy2, x+dx3, y+dy3);
}

void
adg_path_rel_line_to (AdgPath *path,
                      double   dx,
                      double   dy)
{
  double x, y;

  g_return_if_fail (ADG_IS_PATH (path));
  g_return_if_fail (adg_path_get_current_point (path, &x, &y));

  adg_path_line_to (path, x+dx, y+dy);
}

void
adg_path_rel_move_to (AdgPath *path,
                      double   dx,
                      double   dy)
{
  double x, y;

  g_return_if_fail (ADG_IS_PATH (path));
  g_return_if_fail (adg_path_get_current_point (path, &x, &y));

  adg_path_move_to (path, x+dx, y+dy);
}


/* The following code is adapted from cairo-1.3.8 */

static double
arc_error_normalized (double angle)
{
    return 2.0/27.0 * pow (sin (angle / 4), 6) / pow (cos (angle / 4), 2);
}

static double
arc_max_angle_for_tolerance_normalized (double tolerance)
{
  double angle, error;
  int i;

  /* Use table lookup to reduce search time in most cases. */
  struct
    {
      double angle;
      double error;
    } table[] = {
      { M_PI / 1.0,   0.0185185185185185036127 },
      { M_PI / 2.0,   0.000272567143730179811158 },
      { M_PI / 3.0,   2.38647043651461047433e-05 },
      { M_PI / 4.0,   4.2455377443222443279e-06 },
      { M_PI / 5.0,   1.11281001494389081528e-06 },
      { M_PI / 6.0,   3.72662000942734705475e-07 },
      { M_PI / 7.0,   1.47783685574284411325e-07 },
      { M_PI / 8.0,   6.63240432022601149057e-08 },
      { M_PI / 9.0,   3.2715520137536980553e-08 },
      { M_PI / 10.0,  1.73863223499021216974e-08 },
      { M_PI / 11.0,  9.81410988043554039085e-09 },
    };
  int table_size = (sizeof (table) / sizeof (table[0]));

  for (i = 0; i < table_size; i++)
    if (table[i].error < tolerance)
      return table[i].angle;

  ++i;
  do
    {
      angle = M_PI / i++;
      error = arc_error_normalized (angle);
    }
  while (error > tolerance);

  return angle;
}

/* XXX: 22-12-2006 Fontana Nicola <ntd at entidi.it>
 *      Removed the ctm parameter and the calculation of the major axis: I use
 *      the radius directly, instead. Hopefully, this will break only the
 *      calculations for ellipses ...
 */
static int
arc_segments_needed (double angle,
		     double radius,
		     double tolerance)
{
  double max_angle;

  max_angle = arc_max_angle_for_tolerance_normalized (tolerance / radius);

  return (int) ceil (angle / max_angle);
}

static void
arc_segment (AdgPath *path,
	     double   xc,
	     double   yc,
	     double   radius,
	     double   angle_A,
	     double   angle_B)
{
  double r_sin_A, r_cos_A;
  double r_sin_B, r_cos_B;
  double h;

  r_sin_A = radius * sin (angle_A);
  r_cos_A = radius * cos (angle_A);
  r_sin_B = radius * sin (angle_B);
  r_cos_B = radius * cos (angle_B);

  h = 4.0/3.0 * tan ((angle_B - angle_A) / 4.0);

  adg_path_curve_to (path,
                     xc + r_cos_A - h * r_sin_A,
                     yc + r_sin_A + h * r_cos_A,
                     xc + r_cos_B + h * r_sin_B,
                     yc + r_sin_B - h * r_cos_B,
                     xc + r_cos_B,
                     yc + r_sin_B);
}

static void
arc_in_direction (AdgPath  *path,
		  double    xc,
		  double    yc,
		  double    radius,
		  double    angle_min,
		  double    angle_max,
		  Direction dir)
{
  while (angle_max - angle_min > 4 * M_PI)
    angle_max -= 2 * M_PI;

  /* Recurse if drawing arc larger than pi */
  if (angle_max - angle_min > M_PI)
    {
      double angle_mid = angle_min + (angle_max - angle_min) / 2.0;
      /* XXX: Something tells me this block could be condensed. */
      if (dir == DIRECTION_FORWARD)
        {
          arc_in_direction (path, xc, yc, radius,
			    angle_min, angle_mid,
			    dir);

          arc_in_direction (path, xc, yc, radius,
			    angle_mid, angle_max,
			    dir);
        }
      else
        {
          arc_in_direction (path, xc, yc, radius,
			    angle_mid, angle_max,
			    dir);

          arc_in_direction (path, xc, yc, radius,
			    angle_min, angle_mid,
			    dir);
        }
    }
  else
    {
      int i, segments;
      double angle, angle_step;

      /* XXX: 22-12-2006 Fontana Nicola <ntd at entidi.it>
       *      Used the ARC_TOLERANCE constant instead of the cairo context
       *      dependent variable, because I do not have any cairo context here.
       */
      segments = arc_segments_needed (angle_max - angle_min, radius, ARC_TOLERANCE);
      angle_step = (angle_max - angle_min) / (double) segments;

      if (dir == DIRECTION_FORWARD)
        {
          angle = angle_min;
        }
      else
        {
          angle = angle_max;
          angle_step = - angle_step;
        }

      for (i = 0; i < segments; i++, angle += angle_step)
        arc_segment (path, xc, yc, radius, angle, angle + angle_step);
    }
}
