/**
 * ADG_NAN:
 *
 * A constant that represents a NaN (not a number) double value.
 *
 * If the C library define %NAN, %ADG_NAN is an alias of %NAN. In the other
 * cases, a decent NaN value is searched using an algorithm stolen from the
 * <ulink url="http://www.gnome.org/projects/guppi/">Guppi</ulink> program.
 **/

#include "adgutil.h"


#ifndef NAN

#include "adgintl.h"
#include <signal.h>

#define FAKE_NAN        123.45678902468013579e-300


static double   adg_nan_bigendian       (void);
static double   adg_nan_littleendian    (void);
static double   adg_nan_mixedendian     (void);
static double   adg_nan_alpha           (void);
static double   adg_nan_from_sqrt       (void);

static gboolean fake_isnan = FALSE;


double
adg_nan (void)
{
  static double nan_value = 0.0;

  if G_UNLIKELY (nan_value == 0.0)
    {
      nan_value = adg_nan_bigendian ();

      if (! isnan (nan_value))
        nan_value = adg_nan_littleendian ();

      if (! isnan (nan_value))
        nan_value = adg_nan_mixedendian ();

#ifdef HUGE_VAL
      /* Morten Welinder tells me that "Some people here argue that the
         specs guarantee a NaN to come out of this."  It is certainly
         worth a try... */
      if (! isnan (nan_value))
        nan_value = -HUGE_VAL * 0.0;
#endif /* HUGE_VAL */

      /* This is abusive, but I need a quick and dirty way of handling Alpha FP right away... */
      if (! isnan (nan_value))
        nan_value = adg_nan_alpha ();

      /* This is crazy, but we are desparate at this point... */
      if (! isnan (nan_value))
        nan_value = adg_nan_from_sqrt ();

      /* All else has failed, so we have to use the awful FAKE_NAN hack. */
      if (! isnan (nan_value))
        {
          nan_value = FAKE_NAN;
          fake_isnan = TRUE;
          g_warning (_("Using awful FAKE_NAN hack."));
        }
    }

  return nan_value;
}

/**
 * adg_isnan:
 * @value: a double
 *
 * Checks if @value is not a number.
 *
 * In the simplest case, where the C library supports NANs, it is a wrapper of
 * the isnan() system function.
 *
 * Return value: %TRUE if @value is not a number, %FALSE otherwise
 **/
gboolean
adg_isnan (double value)
{
  if (fake_isnan)
    return value == FAKE_NAN;
 
  return isnan (value);
}


static double
adg_nan_bigendian (void)
{
  union
    {
      double d;
      struct
        {
          unsigned int signbit:1;
          unsigned int exp:11;
          unsigned int man0:20;
          unsigned int man1:32;

        } ieee;
    } value;

  value.ieee.signbit = 0;
  value.ieee.man0 = 1;
  value.ieee.man1 = 1;
  value.ieee.exp = 2047;

  return value.d;
}

static double
adg_nan_littleendian (void)
{
  union
    {
      double d;
      struct
        {
          unsigned int man1:32;
          unsigned int man0:20;
          unsigned int exp:11;
          unsigned int signbit:1;
        } ieee;
    } value;

  value.ieee.signbit = 0;
  value.ieee.man0 = 1;
  value.ieee.man1 = 1;
  value.ieee.exp = 2047;

  return value.d;
}

static double
adg_nan_mixedendian (void)
{
  union
    {
      double d;

      struct
        {
          unsigned int man0:20;
          unsigned int exp:11;
          unsigned int signbit:1;
          unsigned int man1:32;
        } ieee;
    } value;

  value.ieee.signbit = 0;
  value.ieee.man0 = 1;
  value.ieee.man1 = 1;
  value.ieee.exp = 2047;

  return value.d;
}

static double
adg_nan_alpha (void)
{
  union guint_double
    {
      guint  w[8];
      double d;
    };

  /* These values are WRONG */
  union guint_double test_a = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x7f } };
  union guint_double test_b = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff } };

  if (isnan (test_a.d))
    return test_a.d;

  return test_b.d;
}


static double
adg_nan_from_sqrt (void)
{
  double           value;
  struct sigaction fpe, old_fpe;

  fpe.sa_handler = SIG_IGN;
  sigaction (SIGFPE, &fpe, &old_fpe);
  value = sqrt (-1.0);
  sigaction (SIGFPE, &old_fpe, NULL);

  return value;
}

#endif /* NAN */
