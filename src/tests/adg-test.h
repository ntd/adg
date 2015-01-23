/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


#ifndef __ADG_TEST_H__
#define __ADG_TEST_H__

#include <glib-object.h>


G_BEGIN_DECLS

/* Redefine the g_assert_cmpfloat() macro by using "float" instead of
 * "long double" helper variables. This is needed by FreeBSD to pass
 * the tests: g_value_get_double() seems to suffer a precision loss on
 * this OS that invalidates the comparation between doubles.
 */
#undef g_assert_cmpfloat
#define g_assert_cmpfloat(n1,cmp,n2) \
    do { \
        float __n1 = (n1), __n2 = (n2); \
        if (__n1 cmp __n2) \
            ; \
        else \
          g_assertion_message_cmpnum(G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                     #n1 " " #cmp " " #n2, __n1, #cmp, __n2, 'f'); \
    } while (0)

/* Some backward compatible macros */
#ifndef g_assert_true
#define g_assert_true(expr)             do { if G_LIKELY (expr) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should be TRUE"); \
                                           } while (0)
#endif

#ifndef g_assert_false
#define g_assert_false(expr)            do { if G_LIKELY (!(expr)) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should be FALSE"); \
                                           } while (0)
#endif

#ifndef g_assert_null
#define g_assert_null(expr)             do { if G_LIKELY ((expr) == NULL) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should be NULL"); \
                                           } while (0)
#endif

#ifndef g_assert_nonnull
#define g_assert_nonnull(expr)          do { if G_LIKELY ((expr) != NULL) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should not be NULL"); \
                                           } while (0)
#endif


void            adg_test_init                   (int            *p_argc,
                                                 char          **p_argv[]);
const gpointer  adg_test_invalid_pointer        (void);
void            adg_test_add_func               (const gchar     *testpath,
                                                 GCallback       test_func);

G_END_DECLS


#endif /* __ADG_TEST_H__ */
