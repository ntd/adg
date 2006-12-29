#ifndef __ADGUTIL_H__
#define __ADGUTIL_H__

#include <glib.h>
#include <cairo.h>
#include <math.h>


G_BEGIN_DECLS

#define ADG_CHECKPOINT_WITH_MESSAGE(message) \
                                g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, \
                                       "file %s: line %d: %s", \
                                       __FILE__, __LINE__, #message)
#define ADG_CHECKPOINT()        ADG_CHECKPOINT_WITH_MESSAGE ("check point")


#ifdef __GNUC__

#define ADG_STUB()              g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, \
                                       "%s: stub", __PRETTY_FUNCTION__)

#else /* !__GNUC__ */

#define ADG_STUB()              ADG_CHECKPOINT_WITH_MESSAGE("stub")

#endif /* __GNUC__ */


#ifdef NAN

#define ADG_NAN         NAN
#define adg_isnan(n)    ((gboolean) (isnan (n) != 0))

#else /* !NAN */

#define ADG_NAN         (adg_nan ())
double                  adg_nan                         (void) G_GNUC_CONST;
gboolean                adg_isnan                       (double          value);

#endif /* NAN */


#define ADG_DIR_NONE    ADG_NAN
#define ADG_DIR_RIGHT   0.0
#define ADG_DIR_UP      (3.0 * G_PI_2)
#define ADG_DIR_LEFT    G_PI
#define ADG_DIR_DOWN    G_PI_2


gboolean                adg_cairo_path_data_start_point (cairo_path_data_t
                                                                        *path_data,
                                                         double         *p_x,
                                                         double         *p_y);
gboolean                adg_cairo_path_data_end_point   (cairo_path_data_t
                                                                        *path_data,
                                                         double         *p_x,
                                                         double         *p_y);
cairo_path_data_t *     adg_cairo_path_tail             (cairo_path_t   *path);

G_END_DECLS


#endif /* __ADGUTIL_H__ */
