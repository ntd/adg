/* This code is an adaptation for Adg of cairo-arc.c, taken from Cairo 1.3.8.
 * You can download it at http://cairographics.org/
 */

#include "adgpathprivate.h"

#include <math.h>


static double
_arc_error_normalized (double angle)
{
    return 2.0/27.0 * pow (sin (angle / 4), 6) / pow (cos (angle / 4), 2);
}

static double
_arc_max_angle_for_tolerance_normalized (double tolerance)
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
      error = _arc_error_normalized (angle);
    }
  while (error > tolerance);

  return angle;
}

/* XXX: 22-12-2006 Fontana Nicola <ntd@users.sourceforge.net>
 *      Removed the ctm parameter and the calculation of the major axis: I use
 *      the radius directly, instead. Hopefully, this will break only the
 *      calculations for ellipses ...
 */
static int
_arc_segments_needed (double angle,
		      double radius,
		      double tolerance)
{
  double max_angle;

  max_angle = _arc_max_angle_for_tolerance_normalized (tolerance / radius);

  return (int) ceil (angle / max_angle);
}

static void
_arc_segment (AdgPath *path,
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
_arc_in_direction (AdgPath     *path,
                   double	xc,
                   double	yc,
                   double	radius,
                   double	angle_min,
                   double       angle_max,
                   AdgDirection dir)
{
  while (angle_max - angle_min > 4 * M_PI)
    angle_max -= 2 * M_PI;

  /* Recurse if drawing arc larger than pi */
  if (angle_max - angle_min > M_PI)
    {
      double angle_mid = angle_min + (angle_max - angle_min) / 2.0;
      /* XXX: Something tells me this block could be condensed. */
      if (dir == ADG_DIRECTION_FORWARD)
        {
          _arc_in_direction (path, xc, yc, radius,
                             angle_min, angle_mid,
                             dir);

          _arc_in_direction (path, xc, yc, radius,
                             angle_mid, angle_max,
                             dir);
        }
      else
        {
          _arc_in_direction (path, xc, yc, radius,
                             angle_mid, angle_max,
                             dir);

          _arc_in_direction (path, xc, yc, radius,
                             angle_min, angle_mid,
                             dir);
        }
    }
  else
    {
      int i, segments;
      double angle, angle_step;

      /* XXX: 22-12-2006 Fontana Nicola <ntd@users.sourceforge.net>
       *      Used the ADG_TOLERANCE constant instead of the cairo context
       *      dependent variable, because I do not have any cairo context here.
       */
      segments = _arc_segments_needed (angle_max - angle_min, radius, ADG_TOLERANCE);
      angle_step = (angle_max - angle_min) / (double) segments;

      if (dir == ADG_DIRECTION_FORWARD)
        {
          angle = angle_min;
        }
      else
        {
          angle = angle_max;
          angle_step = - angle_step;
        }

      for (i = 0; i < segments; i++, angle += angle_step)
        _arc_segment (path, xc, yc, radius, angle, angle + angle_step);
    }
}

/**
 * _cairo_arc_path
 * @path: a cairo context
 * @xc: X position of the center of the arc
 * @yc: Y position of the center of the arc
 * @radius: the radius of the arc
 * @angle1: the start angle, in radians
 * @angle2: the end angle, in radians
 *
 * Compute a path for the given arc and append it onto @path.
 * The arc will be accurate within the current tolerance and
 * given the current transformation.
 **/
void
_adg_path_arc (AdgPath *path,
               double	  xc,
               double	  yc,
               double	  radius,
               double	  angle1,
               double	  angle2)
{
  _arc_in_direction (path, xc, yc, radius, angle1, angle2, ADG_DIRECTION_FORWARD);
}

/**
 * _adg_arc_path_negative:
 * @path: the path onto which the arc will be appended
 * @xc: X position of the center of the arc
 * @yc: Y position of the center of the arc
 * @radius: the radius of the arc
 * @angle1: the start angle, in radians
 * @angle2: the end angle, in radians
 *
 * Compute a path for the given arc (defined in the negative
 * direction) and append it onto @path. The arc will be accurate within the
 * current tolerance and given the current transformation.
 **/
void
_adg_path_arc_negative (AdgPath *path,
                        double   xc,
                        double   yc,
                        double   radius,
                        double   angle1,
                        double   angle2)
{
  _arc_in_direction (path, xc, yc, radius, angle2, angle1, ADG_DIRECTION_REVERSE);
}
