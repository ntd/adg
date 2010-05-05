#ifndef __DEMO_H__
#define __DEMO_H__

#include <gtk/gtk.h>

/* Required for i18n */
#undef G_LOG_DOMAIN
#include <adg/adg-internal.h>


G_BEGIN_DECLS

gchar *         demo_find_data_file     (const gchar    *file,
                                         const gchar    *caller);

G_END_DECLS


#endif /* __DEMO_H__ */
