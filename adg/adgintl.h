#ifndef __ADGINTL_H__
#define __ADGINTL_H__

#include <glib/gi18n.h>

#ifdef ENABLE_NLS
#define P_(String) dgettext(GETTEXT_PACKAGE "-properties",String)
#else 
#define P_(String) (String)
#endif

#endif /* __ADGINTL_H__ */
