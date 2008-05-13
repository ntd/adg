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

#include "adgpath.h"
#include "adgpathprivate.h"
#include "adgutil.h"
#include "adgcanvas.h"
#include "adgintl.h"

#include <math.h>


enum
{
  PROP_0,
  PROP_LINE_STYLE
};


static void	                adg_path_get_property   (GObject	*object,
                                                         guint		 prop_id,
                                                         GValue		*value,
                                                         GParamSpec	*pspec);
static void                     adg_path_set_property   (GObject	*object,
                                                         guint		 prop_id,
                                                         const GValue	*value,
                                                         GParamSpec	*pspec);
static void		        adg_path_finalize	(GObject	*object);

static const AdgLineStyle *     adg_path_get_line_style (AdgEntity      *entity);
static void                     adg_path_set_line_style (AdgEntity      *entity,
                                                         AdgLineStyle   *line_style);
static void                     adg_path_update         (AdgEntity      *entity,
                                                         gboolean        recursive);
static void                     adg_path_outdate        (AdgEntity      *entity,
                                                         gboolean        recursive);
static void                     adg_path_render         (AdgEntity      *entity,
                                                         cairo_t        *cr);

static void                     adg_path_add_portion    (AdgPath        *path,
                                                         cairo_path_data_type_t type,
                                                         ...);


G_DEFINE_TYPE (AdgPath, adg_path, ADG_TYPE_ENTITY);

#define PARENT_CLASS ((AdgEntityClass *) adg_path_parent_class)


static void
adg_path_class_init (AdgPathClass *klass)
{
  GObjectClass   *gobject_class;
  AdgEntityClass *entity_class;
  GParamSpec     *param;

  gobject_class = (GObjectClass *) klass;
  entity_class = (AdgEntityClass *) klass;

  gobject_class->get_property = adg_path_get_property;
  gobject_class->set_property = adg_path_set_property;
  gobject_class->finalize = adg_path_finalize;

  entity_class->get_line_style = adg_path_get_line_style;
  entity_class->set_line_style = adg_path_set_line_style;
  entity_class->update = adg_path_update;
  entity_class->outdate = adg_path_outdate;
  entity_class->render = adg_path_render;

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
  path->line_style = NULL;

  path->cairo_path.status = CAIRO_STATUS_SUCCESS;
  path->cairo_path.data = NULL;
  path->cairo_path.num_data = 0;

  path->cp.x = ADG_NAN;
  path->cp.y = ADG_NAN;

  path->create_func = NULL;
  path->user_data = NULL;
}

static void
adg_path_get_property (GObject    *object,
                       guint       prop_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
  AdgPath *path = ADG_PATH (object);

  switch (prop_id)
    {
    case PROP_LINE_STYLE:
      g_value_set_boxed (value, path->line_style);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
adg_path_set_property (GObject      *object,
                       guint         prop_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
  AdgPath *path;
  AdgEntity *entity;

  path = ADG_PATH (object);
  entity = ADG_ENTITY (object);

  switch (prop_id)
    {
    case PROP_LINE_STYLE:
      path->line_style = g_value_get_boxed (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
adg_path_finalize (GObject *object)
{
  adg_path_clear ((AdgPath *) object);

  ((GObjectClass *) PARENT_CLASS)->finalize (object);
}


static const AdgLineStyle *
adg_path_get_line_style (AdgEntity *entity)
{
  return ADG_PATH (entity)->line_style;
}

static void
adg_path_set_line_style (AdgEntity    *entity,
                         AdgLineStyle *line_style)
{
  ADG_PATH (entity)->line_style = line_style;
  g_object_notify (G_OBJECT (entity), "line-style");
}

static void
adg_path_update (AdgEntity *entity,
                 gboolean   recursive)
{
  AdgPath *path = (AdgPath *) entity;

  if (path->create_func != NULL)
    path->create_func (entity, path->user_data);

  PARENT_CLASS->update (entity, recursive);
}

static void
adg_path_outdate (AdgEntity *entity,
                  gboolean   recursive)
{
  adg_path_clear ((AdgPath *) entity);
  PARENT_CLASS->outdate (entity, recursive);
}

static void
adg_path_render (AdgEntity *entity,
                 cairo_t   *cr)
{
  AdgPath *path = (AdgPath *) entity;

  adg_line_style_apply (adg_entity_get_line_style (entity), cr);
  cairo_append_path (cr, &path->cairo_path);
  cairo_stroke (cr);
}

static void
adg_path_add_portion (AdgPath               *path,
                      cairo_path_data_type_t type,
                      ...)
{
  cairo_path_data_t portion;
  va_list           var_args;

  if (! path->portions)
    path->portions = g_array_sized_new (FALSE, FALSE,
                                        sizeof (cairo_path_data_t),
                                        10 /* Preallocated elements */);

  portion.header.type = type;
  va_start (var_args, type);

  switch (type)
    {
    case CAIRO_PATH_CLOSE_PATH:
      portion.header.length = 1;
      path->portions = g_array_append_val (path->portions, portion);
      portion.point.x = ADG_NAN;
      portion.point.y = ADG_NAN;
      break;

    case CAIRO_PATH_MOVE_TO:
      portion.header.length = 2;
      path->portions = g_array_append_val (path->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      path->portions = g_array_append_val (path->portions, portion);
      break;

    case CAIRO_PATH_LINE_TO:
      portion.header.length = 2;
      path->portions = g_array_append_val (path->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      path->portions = g_array_append_val (path->portions, portion);
      break;
      
    case CAIRO_PATH_CURVE_TO:
      portion.header.length = 4;
      path->portions = g_array_append_val (path->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      path->portions = g_array_append_val (path->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      path->portions = g_array_append_val (path->portions, portion);
      portion.point.x = va_arg (var_args, double);
      portion.point.y = va_arg (var_args, double);
      path->portions = g_array_append_val (path->portions, portion);
      break;

    default:
      g_assert_not_reached ();
    }

  path->cairo_path.data = (cairo_path_data_t *) path->portions->data;
  path->cairo_path.num_data = path->portions->len;
  path->cairo_path.status = CAIRO_STATUS_SUCCESS;

  path->cp.x = portion.point.x;
  path->cp.y = portion.point.y;
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
  AdgEntity *entity;
  AdgPath   *path;
 
  entity = (AdgEntity *) g_object_new (ADG_TYPE_PATH, NULL);
  path = (AdgPath *) entity;

  path->create_func = create_func;
  path->user_data = user_data;

  return entity;
}

void
adg_path_clear (AdgPath *path)
{
  g_return_if_fail (ADG_IS_PATH (path));

  g_array_free (path->portions, TRUE);
  path->cairo_path.data = NULL;
  path->cairo_path.num_data = 0;
  path->cp.x = ADG_NAN;
  path->cp.y = ADG_NAN;
}


const cairo_path_t *
adg_path_get_cairo_path (AdgPath *path)
{
  g_return_val_if_fail (ADG_IS_PATH (path), NULL);

  return & path->cairo_path;
}


void
adg_path_chain_ymirror (AdgPath *path)
{
  cairo_path_t      *cairo_path;
  cairo_path_data_t *src;
  cairo_path_data_t *p_src, *p_dst;
  gint               length;
  gint               n_data, n_point;
  double             last_x, last_y;

  g_return_if_fail (ADG_IS_PATH (path));

  cairo_path = & path->cairo_path;

  g_return_if_fail (cairo_path->num_data > 2);
  g_return_if_fail (cairo_path->data->header.type == CAIRO_PATH_MOVE_TO);

  src = g_memdup (cairo_path->data, cairo_path->num_data * sizeof (cairo_path_data_t));
  path->portions = g_array_set_size (path->portions, cairo_path->num_data * 2);
  p_src = src;
  p_dst = (cairo_path_data_t *) path->portions->data + cairo_path->num_data * 2;
  n_data = 2;

  ++ p_src;
  last_x = p_src->point.x;
  last_y = p_src->point.y;
  path->cp.x = last_x;
  path->cp.y = last_y;
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
  cairo_path->data = (cairo_path_data_t *) path->portions->data;
}

void
adg_path_dump (AdgPath *path)
{
  cairo_path_data_t *data;
  gint               n_data, n_point;

  g_return_if_fail (ADG_IS_PATH (path));

  for (n_data = 0; n_data < path->cairo_path.num_data; ++ n_data)
    {
      data = path->cairo_path.data + n_data;

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
  g_return_val_if_fail (ADG_IS_PATH (path), FALSE);

  if (adg_isnan (path->cp.x) || adg_isnan (path->cp.y))
    return FALSE;

  if (x != NULL)
    *x = path->cp.x;

  if (y != NULL)
    *y = path->cp.y;

  return TRUE;
}

void
adg_path_close (AdgPath *path)
{
  g_return_if_fail (ADG_IS_PATH (path));

  adg_path_add_portion (path, CAIRO_PATH_CLOSE_PATH);
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
  _adg_path_arc (path, x, y, radius, angle1, angle2);
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
  _adg_path_arc_negative (path, x, y, radius, angle1, angle2);
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

  adg_path_add_portion (path, CAIRO_PATH_CURVE_TO, x1, y1, x2, y2, x3, y3);
}

void
adg_path_line_to (AdgPath *path,
                  double   x,
                  double   y)
{
  g_return_if_fail (ADG_IS_PATH (path));

  adg_path_add_portion (path, CAIRO_PATH_LINE_TO, x, y);
}

void
adg_path_move_to (AdgPath *path,
                  double   x,
                  double   y)
{
  g_return_if_fail (ADG_IS_PATH (path));

  adg_path_add_portion (path, CAIRO_PATH_MOVE_TO, x, y);
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
