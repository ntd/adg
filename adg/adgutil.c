/* ADG - Automatic Drawing Generation.
 * Copyright (C) 2007 - Fontana Nicola <ntd@users.sourceforge.net>
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
 * SECTION:adgutil
 * @title: Utilities
 * @short_description: A collection of miscellaneous functions
 **/

#include "adgutil.h"


/**
 * adg_cairo_path_tail:
 * @path: a #cairo_path_t structure
 *
 * Gets the tail, that is the #cairo_path_data_t node before the last one.
 * If @path has only one node, the function returns %NULL.
 *
 * Return value: the tail of @path, or %NULL on errors.
 **/
cairo_path_data_t *
adg_cairo_path_tail (cairo_path_t *path)
{
  cairo_path_data_t *last;
  cairo_path_data_t *tail;
  int                i;

  g_return_val_if_fail (path != NULL, NULL);

  last = NULL;
  tail = NULL;

  /* Iterate throught path to find the tail */
  for (i = 0; i < path->num_data; i += last->header.length)
    {
      tail = last;
      last = path->data + i;
    }

  return tail;
}

/**
 * adg_cairo_path_data_start_point:
 * @path_data: a #cairo_path_data_t structure
 * @p_x: destination x coordinate
 * @p_y: destination y coordinate
 *
 * Gets the startpoint of @path_data and store the result in @p_x and @p_y.
 * Passing %NULL in @path_data is valid: (*p_x,*p_y) will be set to (0,0) and
 * the function will return %TRUE.
 *
 * Return value: %TRUE if a valid startpoint is found, %FALSE on errors.
 **/
gboolean
adg_cairo_path_data_start_point (cairo_path_data_t *path_data,
                                 double            *p_x,
                                 double            *p_y)
{
  g_return_val_if_fail (p_x != NULL && p_y != NULL, FALSE);

  if (path_data == NULL)
    {
      *p_x = 0.0;
      *p_y = 0.0;
      return TRUE;
    }

  switch (path_data->header.type)
    {
      case CAIRO_PATH_MOVE_TO:
      case CAIRO_PATH_LINE_TO:
      case CAIRO_PATH_CURVE_TO:
        *p_x = path_data[1].point.x;
        *p_y = path_data[1].point.y;
        return TRUE;
      case CAIRO_PATH_CLOSE_PATH:
        /* A closed path does not have startpoints */
        return FALSE;
    }

  g_return_val_if_reached (FALSE);
}

/**
 * adg_cairo_path_data_end_point:
 * @path_data: a #cairo_path_data_t structure
 * @p_x: destination x coordinate
 * @p_y: destination y coordinate
 *
 * Gets the endpoint of @path_data and store the result in @p_x and @p_y.
 * Passing %NULL in @path_data is valid: (*p_x,*p_y) will be set to (0,0) and
 * the function will return %TRUE.
 *
 * Return value: %TRUE if a valid endpoint is found, %FALSE on errors.
 **/
gboolean
adg_cairo_path_data_end_point (cairo_path_data_t *path_data,
                               double            *p_x,
                               double            *p_y)
{
  g_return_val_if_fail (p_x != NULL && p_y != NULL, FALSE);

  if (path_data == NULL)
    {
      *p_x = 0.0;
      *p_y = 0.0;
      return TRUE;
    }

  switch (path_data->header.type)
    {
      case CAIRO_PATH_MOVE_TO:
      case CAIRO_PATH_LINE_TO:
        *p_x = path_data[1].point.x;
        *p_y = path_data[1].point.y;
        return TRUE;
      case CAIRO_PATH_CURVE_TO:
        *p_x = path_data[3].point.x;
        *p_y = path_data[3].point.y;
        return TRUE;
      case CAIRO_PATH_CLOSE_PATH:
        /* A closed path does not have endpoints */
        return FALSE;
    }

  g_return_val_if_reached (FALSE);
}
